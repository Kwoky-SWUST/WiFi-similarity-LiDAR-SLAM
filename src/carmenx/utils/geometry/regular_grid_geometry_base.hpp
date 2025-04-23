#ifndef SCIROCO__UTILS__GEO_REGULAR_GRID_GEO_BASE
#define SCIROCO__UTILS__GEO_REGULAR_GRID_GEO_BASE

/************************************************************
 * grid geometry
 ************************************************************/

// base class for grid geometry
class RegularGridGeometry {
  public:
    RegularGridGeometry() : m_numDimensionsUsed(0) {}

    /// The number of dimensions used for the grid.
    /// @note If the grid is built over X and Y, numDimensionsUsed is 2.
    int          m_numDimensionsUsed;
};


#endif
