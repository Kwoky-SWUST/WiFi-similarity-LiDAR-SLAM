#ifndef SCIROCO__UTILS__GEO_REGULAR_GRID_GEO_3D
#define SCIROCO__UTILS__GEO_REGULAR_GRID_GEO_3D

#include <iosfwd>

#include "vector3d.h"
#include "regular_grid_geometry_base.hpp"

// forward decl. of the 6d version for conversions inside the 3d version
class RegularGridGeometry6D;


/*************************************************
 *  RegularGridGeometry3D
 */


// grid geometry for 3 degrees of freedom (x,y,z)
class RegularGridGeometry3D : public RegularGridGeometry {
  public:
    enum {
      GEO3_DIM_X = 0,
      GEO3_DIM_Y = 1,
      GEO3_DIM_Z = 2,
      GEO3_NUM_DIMENSIONS = 3
    };

    RegularGridGeometry3D();
    RegularGridGeometry3D( const RegularGridGeometry3D &other );
    RegularGridGeometry3D( RegularGridGeometry3D const * const other );
    RegularGridGeometry3D( const RegularGridGeometry6D &other );

    RegularGridGeometry3D& operator= ( const RegularGridGeometry3D& other );
    RegularGridGeometry3D& operator= ( const RegularGridGeometry6D& other );

    // set the coverage of the grid, also updates covered dimensions flags
    // if min != max for each dimension
    void setMinCoords( const Vector3d& min );
    void setMinCoords( const double x, const double y, const double z );
    void setMaxCoords( const Vector3d& max );
    void setMaxCoords( const double x, const double y, const double z );

    // set the grid cell width
    void setCellWidth( const Vector3d& widthXYZ );
    void setCellWidth( const double wx, const double wy, const double wz );
    void setCellWidth( const double widthXYZ );

    // set the dimensions covered by the grid (implicitly called/updated
    // if the grid's min-/max-coords are changed)
    void setCoveredDimensions( const bool x, const bool y, const bool z );
    //void setCoveredDimensions( const unsigned int x, const unsigned int y, const unsigned int z );
    void setCoveredDimensions( const Vector3u& dimFlags );
    void enable2d();
    void enable3d();

    // enable centering of cells along a given axis
    void setCenteredAxis( const bool x, const bool y, const bool z );
    //void setCenteredAxis( const unsigned int x, const unsigned int y, const unsigned int z );
    void setCenteredAxis( const Vector3u& centeredFlags );

    // enable symmetric behavior of target values along a given axis (along x means symmetric in regard to y-z-plane)
    void setSymmetricAxis( const bool x, const bool y, const bool z );
    //void setSymmetricAxis( const unsigned int x, const unsigned int y, const unsigned int z );
    void setSymmetricAxis( const Vector3u& symmetricFlags );

    // compute and initialize grid values, given the min, max and width of the grid
    void initialize();
    void initialize( const Vector3d& min, const Vector3d& max, const double cellWidth );
    void initialize( const Vector3d& min, const Vector3d& max, const Vector3d& cellWidth );

    // transforms world to grid coordinate frame (@see m_gridOrigin) and returns the indices to the appropriate grid cell
    // @note does not ensure that the indices are within grid boundaries!
    const Vector3i getCellIndex( const Vector3d& coords_world ) const;
    const Vector3i getCellIndex( const double wX, const double wY, const double wZ ) const { return getCellIndex( Vector3d( wX, wY, wZ ) ); }

    // get the grid's width over each dimension
    const Vector3d& getGridWidth() const { return m_gridWidth; }

    // get the lut multipliers
    const Vector3u& getLutMultipliers() const { return m_lutMultipliers; }


    /////////////////
    ///// DATA  /////
    /////////////////

    /// Flags indicating which dimensions are covered by the grid (x,y,z)
    Vector3u    m_desiredDimensions;
    Vector3u    m_activeDimensions;

    /// The resolution (in m) of the look-up table w.r.t. the axes
    Vector3d    m_lutResolutionXYZ;

    /// Number of cells required/used (for each dimension)
    Vector3u    m_numCells;

    /// This flag indicates whether grid cells are centered (treated as boolean[3])
    Vector3u    m_centeredAtAxis;

    /// Lower coordinate bounds of data
    Vector3d    m_minCoord;

    /// Upper coordinate bounds of data
    Vector3d    m_maxCoord;

    /// The true origin of the grid in the world coordinate frame, computed from lower, upper bounds and grid cell width
    Vector3d    m_gridOrigin;

    /// This flag indicates whether samples are doubled by using them
    /// symmetrically along the axes (treated as boolean[3])
    /// e.g. symmetric in x means actually symmetry in y-z-plane: measurement (x0,y,z) should be added at (x0,y,z) and (-x0,y,z) - so this is actually a symmetry in regard to the y-z-plane!
    Vector3u    m_symmetricMeasurements;

  protected:
    /// width (max-min) of the grid for each dimension
    Vector3d    m_gridWidth;

    /// Multipliers for each dimension of the lookup-table
    Vector3u    m_lutMultipliers;
};


std::ostream & operator << ( std::ostream & os, const RegularGridGeometry3D & geometry );

#endif
