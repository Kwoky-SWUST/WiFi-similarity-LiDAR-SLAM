#ifndef SCIROCO__UTILS__GEO_REGULAR_GRID_BASE_TYPES_H
#define SCIROCO__UTILS__GEO_REGULAR_GRID_BASE_TYPES_H


/************************************************************
 * sample types: position and value
 ************************************************************/

template < class TargetValueT = double, class VectorT = Vector3d >
class GridSampleValuePosition {
  public:
    GridSampleValuePosition() {}
    GridSampleValuePosition( const TargetValueT& _val, const VectorT& _pos ) : position(_pos), value(_val) {}
    GridSampleValuePosition( const VectorT& _pos, const TargetValueT& _val ) : position(_pos), value(_val) {}
    GridSampleValuePosition( const GridSampleValuePosition& other ) : position(other.position), value(other.value) {}

    GridSampleValuePosition& operator= ( const GridSampleValuePosition& other ) {
      if( this != &other ) {
        position = other.position;
        value = other.value;
      }
      return *this;
    }

    VectorT        position;
    TargetValueT   value;
};

template < class TargetValueT, class VectorT >
std::ostream & operator << ( std::ostream& os, const GridSampleValuePosition< TargetValueT, VectorT > & gridCell );


/************************************************************
 * grid cell type: container of sample types (vector of samples for a cell)
 ************************************************************/

template < class TargetValueT = double, class VectorT = Vector3d >
class GridCell {
  public:

    typedef TargetValueT                                     ValueType;
    typedef VectorT                                          VectorType;
    typedef GridSampleValuePosition< TargetValueT, VectorT > SampleType;

    GridCell();
    GridCell( const GridCell< TargetValueT, VectorT >& cell ) : m_samples(cell.m_samples), m_cellCenter(cell.m_cellCenter), m_mean(cell.m_mean), m_variance(cell.m_variance) {}
    GridCell( const VectorT & centerPosition ) : m_cellCenter(centerPosition) {}
    GridCell( const VectorT & centerPosition, const SampleType& s ) : m_cellCenter(centerPosition) { m_samples.push_back(s); }
    GridCell( const SampleType& s ) { m_samples.push_back( s ); }
    GridCell( const std::vector< SampleType >& samples ) : m_samples(samples) {}

    void addSample( const SampleType& s ) { m_samples.push_back( s ); }
    void clearSamples() { m_samples.clear(); }

    const std::vector< SampleType >& getSamples() const { return m_samples; }
    std::vector< SampleType >& getSamples() { return m_samples; }

    const SampleType& operator[] ( size_t index ) const { return m_samples[index]; }
    SampleType& operator[] ( size_t index ) { return m_samples[index]; }

    const SampleType& getSample ( size_t index ) const { return m_samples[index]; }
    SampleType& getSample ( size_t index ) { return m_samples[index]; }

    std::vector< SampleType > m_samples;
    VectorT                   m_cellCenter;
    TargetValueT              m_mean;
    TargetValueT              m_variance;
};

template < class TargetValueT, class VectorT >
std::ostream & operator << ( std::ostream & os, const GridCell< TargetValueT, VectorT > & grid_cell );

// some typedefs
typedef GridCell< double, Vector3d >          GridCell3d;

#include "regular_grid_base_types.inl"

#endif
