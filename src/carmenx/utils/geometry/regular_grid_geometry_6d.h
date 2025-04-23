#ifndef SCIROCO__UTILS__GEO_REGULAR_GRID_GEO_6D
#define SCIROCO__UTILS__GEO_REGULAR_GRID_GEO_6D

#include <iosfwd>

#include "regular_grid_geometry_base.hpp"

// forward decl
class RegularGridGeometry3D;


/*************************************************
 *  RegularGridGeometry6D
 */


// grid geometry for 6 degrees of freedom (x,y,z,theta_x,theta_y,theta_z)
class RegularGridGeometry6D : public RegularGridGeometry {
  public:
    enum {
      GEO6_DIM_X = 0,
      GEO6_DIM_Y = 1,
      GEO6_DIM_Z = 2,
      GEO6_DIM_THETA_X = 3,
      GEO6_DIM_THETA_Y = 4,
      GEO6_DIM_THETA_Z = 5,
      GEO6_NUM_DIMENSIONS = 6
    };

    RegularGridGeometry6D();
    RegularGridGeometry6D( const RegularGridGeometry6D &other );
    RegularGridGeometry6D( RegularGridGeometry6D const * const other );
    RegularGridGeometry6D( const RegularGridGeometry3D &other );

    RegularGridGeometry6D& operator= ( const RegularGridGeometry6D& other );

    // getters
    double const * const getGridWidth() const { return &m_gridWidth[0]; }
    const bool* getActiveDimensions() const;
    const bool* getCenteredAtAxis() const;
    const bool* getDesiredDimensions() const;
    const double* getGridOrigin() const;
    const unsigned int* getLutMultipliers() const;
    const double* getLutResolutionXyzTheta() const;
    const double* getMaxCoord() const;
    const double* getMinCoord() const;
    const unsigned int* getNumCells() const;
    const bool* getSymmetricMeasurements() const;

    /////////////////
    ///// DATA  /////
    /////////////////

    /// Flags indicating which dimensions are covered by the grid
    /// @see x,y,z,theta_x,theta_y,theta_z
    bool            m_desiredDimensions[GEO6_NUM_DIMENSIONS];
    bool            m_activeDimensions[GEO6_NUM_DIMENSIONS];

    /// The resolution (in m) of the look-up table w.r.t. the axes
    double          m_lutResolutionXYZTheta[GEO6_NUM_DIMENSIONS];

    /// Number of cells required/used (for each dimension)
    unsigned int    m_numCells[GEO6_NUM_DIMENSIONS];

    /// This flag indicates whether grid cells are centered
    bool            m_centeredAtAxis[GEO6_NUM_DIMENSIONS];

    /// Lower coordinate bounds of grid
    double          m_minCoord[GEO6_NUM_DIMENSIONS];

    /// Upper coordinate bounds of grid
    double          m_maxCoord[GEO6_NUM_DIMENSIONS];

    /// The true origin of the grid in the world coordinate frame, computed from lower, upper bounds and grid cell width
    double          m_gridOrigin[GEO6_NUM_DIMENSIONS];

    /// This flag indicates whether samples are doubled by using them
    /// symmetrically along the axes (treated as boolean[3])
    /// e.g. symmetric in x means: measurement (x0,y,z,...) should be added at (x0,y,z,...) and (-x0,y,z,...)
    bool            m_symmetricMeasurements[GEO6_NUM_DIMENSIONS];

  protected:
    /// width of the grid for each dimension
    double          m_gridWidth[GEO6_NUM_DIMENSIONS];

    /// Multipliers for each dimension of the lookup-table
    unsigned int    m_lutMultipliers[GEO6_NUM_DIMENSIONS];
};

std::ostream & operator << ( std::ostream & os, const RegularGridGeometry6D & geometry );

#endif
