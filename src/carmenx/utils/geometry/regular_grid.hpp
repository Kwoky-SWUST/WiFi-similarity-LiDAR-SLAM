#ifndef SCIROCO__UTILS__GEO_REGULAR_GRID
#define SCIROCO__UTILS__GEO_REGULAR_GRID

#include <vector>
#include <assert.h>

#include "vector3d.h"
#include "regular_grid_base_types.hpp"

/************************************************************
 * regular grid encapsulating grid geometry and grid cells:
 * abstract base class, derived classes should/may implement and use grid geometry
 ************************************************************/

template < class ValueT = double, class VectorT = Vector3d >
class RegularGrid {
  public:
    typedef GridCell< ValueT, VectorT >           GridCellType;
    typedef typename GridCellType::ValueType      ValueType;
    typedef typename GridCellType::VectorType     VectorType;
    typedef typename GridCellType::SampleType     SampleType;

    RegularGrid() : m_bNeedsRebuild(true),
                    m_numSamples(0),
                    m_numCells(0),
                    m_numOccupiedCells(0) {}
    RegularGrid( const RegularGrid& other ) : m_cells(other.m_cells),
                                              m_bNeedsRebuild(other.m_bNeedsRebuild),
                                              m_numSamples(other.m_numSamples),
                                              m_numCells(other.m_numCells),
                                              m_numOccupiedCells(other.m_numOccupiedCells) {}
    virtual ~RegularGrid() {}

    const std::vector< GridCellType >& getCellData() const { return m_cells; }
    std::vector< GridCellType >& getCellData() { return m_cells; }

    virtual void initialize() = 0;
    virtual void reset() { m_cells.clear(); m_numCells = 0; m_numSamples = 0; m_numOccupiedCells = 0; m_bNeedsRebuild = true; initialize(); }

    virtual void addSample( const ValueType& value, const VectorType& pos ) {}
    virtual void addSample( const VectorType& pos, const ValueType& value ) { addSample( value, pos ); }
    virtual void addSample( const SampleType& sample ) { addSample( sample.value, sample.position ); }

    void addSamplesFromCell( const GridCellType& gridCell ) {
      for( size_t iSample = 0; iSample < gridCell.getSamples().size(); ++iSample )
        addSample( gridCell.getSample( iSample ) );
    }
    void addSamplesFromCells( const std::vector< GridCellType >& gridCells ) {
      for( size_t iCell = 0; iCell < gridCells.size(); ++iCell )
        addSamplesFromCell( gridCells[iCell] );
    }

    void reassignSamplesToCells() {
      if( m_numSamples > 0 ) {
        std::vector< GridCellType > oldSamples;
        oldSamples.swap( m_cells );
        m_cells.clear();
        m_cells.resize( m_numCells );
        m_numOccupiedCells = 0;
        addSamplesFromCells( oldSamples );
      } else {
        m_cells.clear();
        m_cells.resize( m_numCells );
      }
      m_bNeedsRebuild = false;
    }

    const GridCellType& getGridCell( const size_t index ) const { assert(index < m_cells.size()); return m_cells[index]; }
    GridCellType& getGridCell( const size_t index ) { assert(index < m_cells.size()); return m_cells[index]; }

    virtual const int getLutIndexOfCell( const VectorType& coords ) const = 0;

    /** Returns the grid cell containing coords
     * @param coords world coordinates
     * @return the grid cell containing the desired coordinates
     */
    const GridCellType& getGridCell( const VectorType& coords ) const { return m_cells[ getLutIndexOfCell( coords ) ]; }
    GridCellType& getGridCell( const VectorType& coords ) { return m_cells[ getLutIndexOfCell( coords ) ]; }

  protected:
    /** vector of gridCells containing samples
     *  usage: m_cells[cellIdx][i] returns ith sample in cell cellIdx
     */
    std::vector< GridCellType > m_cells;

    // flag indicating whether the samples should be reassigned to cells due to grid parameter changes
    bool                        m_bNeedsRebuild;

  public:
    // counters for underlying data fields
    size_t                      m_numSamples;
    size_t                      m_numCells;
    size_t                      m_numOccupiedCells;
};

#endif

