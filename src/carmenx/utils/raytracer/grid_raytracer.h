#ifndef AMBISENSE_RAYTRACER
#define AMBISENSE_RAYTRACER

#include <iostream>
#include <utils/geometry/point_extension.h>


/** Ray tracer on CARMEN grid maps
  * @author Philipp Vorst
  */
// --------------------------------------------------------------------------
class GridRaytracer
// --------------------------------------------------------------------------
{
  public:
    /** Create a ray tracer on the given map.
     *  @param map CARMEN grid map
     *  @param occupiedProbThreshold Threshold above which cells will be
     *           regarded as occupied
     */
    GridRaytracer( carmen_map_t* map, const double occupiedProbThreshold = 0.5 );

    /** Compute the squared distance to the first occupied cell, based on
     *  the position of the laser range finder and global beam angle as
     *  defined in p.
     *  @param treatUnknownCellsAsInfinity If true, the ray tracer will 
     *           return INFINITY whenever the beam hits a cell of value
     *           "unknown"
     */
    double getExpectedSquaredDistance( const carmen_point_t* p,
                                       bool treatUnknownCellsAsInfinity = false );

  protected:
    /// The underlying CARMEN grid map
    carmen_map_t* m_pMap;

    /// The user-supplied occupancy threshold
    double        m_dOccupiedProbThreshold;

    int           m_iMaxNumCells;
    double        m_dInverseResolution;
};

#endif // AMBISENSE_RAYTRACER
