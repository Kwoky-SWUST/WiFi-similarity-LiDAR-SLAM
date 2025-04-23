#ifndef SCIROCO__UTILS__GEO_INTERSECTIONS_H_1290482095809
#define SCIROCO__UTILS__GEO_INTERSECTIONS_H_1290482095809

#include <carmen/global.h>
#include <vector>

class Pose;
template < typename T > class Vector2;

enum IntersectionResult {
  IR_DISJOINT = 0,
  IR_INTERSECTED = 1,
  IR_OVERLAPPED = 2
};

enum circularMotionDirection {
  MD_CLOCKWISE = 0,
  MD_COUNTER_CLOCKWISE = 1,
  MD_UNDEFINED = 255
};


// test whether 2 rectangles in 2D overlap - using separating axis
IntersectionResult intersectRectRect2D( carmen_point_t const * const rect1,
                                        carmen_point_t const * const rect2,
                                        circularMotionDirection dir = MD_UNDEFINED );
template < class Vec2Type >
IntersectionResult intersectRectRect2D( const std::vector< Vec2Type >& rect1,
                                        const std::vector< Vec2Type >& rect2,
                                        circularMotionDirection dir = MD_UNDEFINED );



// intersect2D_2Segments(): the intersection of 2 finite 2D segments
//    Input:  two finite segments S1 and S2
//    Output: intersection1 = intersect point (when it exists)
//            intersection2 = endpoint of intersect segment [I0,I1] (when it exists)
//    Return: 0=disjoint (no intersect)
//            1=intersect in unique point I0
//            2=overlap in segment from I0 to I1
IntersectionResult intersectLineSegLineSeg2D( const carmen_point_t& P0,
                                              const carmen_point_t& P1,
                                              const carmen_point_t& Q0,
                                              const carmen_point_t& Q1,
                                              carmen_point_t& intersection1,
                                              carmen_point_t& intersection2 );

IntersectionResult intersectLineSegLineSeg2D( const Pose& P0,
                                              const Pose& P1,
                                              const Pose& Q0,
                                              const Pose& Q1,
                                              Pose& intersection1,
                                              Pose& intersection2 );

#endif
