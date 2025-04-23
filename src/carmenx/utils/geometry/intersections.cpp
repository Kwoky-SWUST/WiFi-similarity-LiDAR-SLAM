#include <vector>
#include <assert.h>
#include <iostream>
#include "vector2d.h"
#include "pose.h"

#include "intersections.h"

template < class Vec2Type >
IntersectionResult intersectRectRect2D( const std::vector< Vec2Type >& rect1,
                                        const std::vector< Vec2Type >& rect2,
                                        circularMotionDirection dir /* = MD_UNDEFINED */ )
{
  assert( rect1.size() == 4 );
  assert( rect2.size() == 4 );

  ////////////
  // culling

  // simple x-y-interval overlaps (look for separating axis in x and y)
  if(1) {
    // x
    const double minx1 = std::min( std::min( std::min( rect1[0].x, rect1[1].x ), rect1[2].x ), rect1[3].x );
    const double minx2 = std::min( std::min( std::min( rect2[0].x, rect2[1].x ), rect2[2].x ), rect2[3].x );
    const double maxx1 = std::max( std::max( std::max( rect1[0].x, rect1[1].x ), rect1[2].x ), rect1[3].x );
    const double maxx2 = std::max( std::max( std::max( rect2[0].x, rect2[1].x ), rect2[2].x ), rect2[3].x );
    if( !(minx1 < maxx2 && maxx1 > minx2) ) return IR_DISJOINT;
    // y
    const double miny1 = std::min( std::min( std::min( rect1[0].y, rect1[1].y ), rect1[2].y ), rect1[3].y );
    const double miny2 = std::min( std::min( std::min( rect2[0].y, rect2[1].y ), rect2[2].y ), rect2[3].y );
    const double maxy1 = std::max( std::max( std::max( rect1[0].y, rect1[1].y ), rect1[2].y ), rect1[3].y );
    const double maxy2 = std::max( std::max( std::max( rect2[0].y, rect2[1].y ), rect2[2].y ), rect2[3].y );
    if( !(miny1 < maxy2 && maxy1 > miny2) ) return IR_DISJOINT;
  }

  // END: culling
  ////////////

  ////////////
  // look whether any edge is a separating axis

  // TODO: - optimize with min/max intervals (less dot products needed)
  //       - may also optimize out the switch statement!
  //       - if the constraint of a rectangle is hard, then actually
  //         only 2 axes per rectangle needed, since 2x2 axes are parallel!

  // edges of first rectangle
  for( int i=0; i<4; ++i ) {

    // get the normal (perpendicular)
    Vec2Type normal( (rect1[((i+1)&3)] - rect1[i]).getNormal() );
    switch (dir)
    {
      case MD_COUNTER_CLOCKWISE:
        normal = -normal;
        break;
      case MD_UNDEFINED:
        if( normal.dot( rect1[((i+2)&3)] ) > 0 )
          normal = -normal;
        break;
      case MD_CLOCKWISE:
      default:
        break;
    }

    // check dot products of all points in rect2
    if( normal.dot( rect2[0]-rect1[i] ) >= 0 &&
        normal.dot( rect2[1]-rect1[i] ) >= 0 &&
        normal.dot( rect2[2]-rect1[i] ) >= 0 &&
        normal.dot( rect2[3]-rect1[i] ) >= 0 )
      return IR_DISJOINT;
  }

  // edges of second rectangle
  for( int i=0; i<4; ++i ) {

    // get the normal (perpendicular)
    Vec2Type normal( (rect2[((i+1)&3)] - rect2[i]).getNormal() );
    switch (dir)
    {
      case MD_COUNTER_CLOCKWISE:
        normal = -normal;
        break;
      case MD_UNDEFINED:
        if( normal.dot( rect2[((i+2)&3)] ) > 0 )
          normal = -normal;
        break;
      case MD_CLOCKWISE:
      default:
        break;
    }

    // check dot products of all points in rect1
    if( normal.dot( rect1[0]-rect2[i] ) >= 0 &&
        normal.dot( rect1[1]-rect2[i] ) >= 0 &&
        normal.dot( rect1[2]-rect2[i] ) >= 0 &&
        normal.dot( rect1[3]-rect2[i] ) >= 0 )
      return IR_DISJOINT;
  }

  // no separating axis found -> intersecting!
  return IR_INTERSECTED;
}

// epxlicit instantiation
template IntersectionResult intersectRectRect2D< Vector2d >( const std::vector< Vector2d >&,
                                                             const std::vector< Vector2d >&,
                                                             circularMotionDirection dir /* = MD_UNDEFINED */ );
template IntersectionResult intersectRectRect2D< Vector2f >( const std::vector< Vector2f >&,
                                                             const std::vector< Vector2f >&,
                                                             circularMotionDirection dir /* = MD_UNDEFINED */ );
template IntersectionResult intersectRectRect2D< Vector2u >( const std::vector< Vector2u >&,
                                                             const std::vector< Vector2u >&,
                                                             circularMotionDirection dir /* = MD_UNDEFINED */ );
template IntersectionResult intersectRectRect2D< Vector2i >( const std::vector< Vector2i >&,
                                                             const std::vector< Vector2i >&,
                                                             circularMotionDirection dir /* = MD_UNDEFINED */ );



// wrapper ...
IntersectionResult intersectRectRect2D( const carmen_point_t* const rect1,
                                        const carmen_point_t* const rect2,
                                        circularMotionDirection dir /* = MD_CLOCKWISE */ )
{
  std::vector<Vector2d> r1;
  std::vector<Vector2d> r2;

  r1.push_back( Vector2d( rect1[0] ) );
  r1.push_back( Vector2d( rect1[1] ) );
  r1.push_back( Vector2d( rect1[2] ) );
  r1.push_back( Vector2d( rect1[3] ) );

  r2.push_back( Vector2d( rect2[0] ) );
  r2.push_back( Vector2d( rect2[1] ) );
  r2.push_back( Vector2d( rect2[2] ) );
  r2.push_back( Vector2d( rect2[3] ) );

  return intersectRectRect2D( r1, r2, dir );
}



// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.


typedef Vector2d Point;

struct Segment {
    Vector2d P0, P1;
};

int inSegment( Point, Segment );

#define SMALL_NUM  0.00000001 // anything that avoids division overflow
#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y)
#define perp(u,v)  ((u).x * (v).y - (u).y * (v).x)  // perp product (2D)

// intersect2D_2Segments(): the intersection of 2 finite 2D segments
//    Input:  two finite segments S1 and S2
//    Output: I0 = intersect point (when it exists)
//            I1 = endpoint of intersect segment [I0,I1] (when it exists)
//    Return: 0=disjoint (no intersect)
//            1=intersect in unique point I0
//            2=overlap in segment from I0 to I1
int
intersect2D_Segments( Segment S1, Segment S2, Point& I0, Point& I1 )
{
  Vector2d    u = S1.P1 - S1.P0;
  Vector2d    v = S2.P1 - S2.P0;
  Vector2d    w = S1.P0 - S2.P0;
  double      D = perp(u,v);

  // test if they are parallel (includes either being a point)
  if (fabs(D) < SMALL_NUM) {      // S1 and S2 are parallel
    if (perp(u,w) != 0 || perp(v,w) != 0) {
      return 0;                   // they are NOT collinear
    }
    // they are collinear or degenerate
    // check if they are degenerate points
    double du = dot(u,u);
    double dv = dot(v,v);
    if (du==0 && dv==0) {         // both segments are points
      if (S1.P0 != S2.P0)         // they are distinct points
        return 0;
      I0 = S1.P0;                 // they are the same point
      return 1;
    }
    if (du==0) {                  // S1 is a single point
      if (inSegment(S1.P0, S2) == 0) // but is not in S2
        return 0;
      I0 = S1.P0;
      return 1;
    }
    if (dv==0) {                  // S2 a single point
      if (inSegment(S2.P0, S1) == 0) // but is not in S1
        return 0;
      I0 = S2.P0;
      return 1;
    }
    // they are collinear segments - get overlap (or not)
    double t0, t1;                 // endpoints of S1 in eqn for S2
    Vector2d w2 = S1.P1 - S2.P0;
    if (v.x != 0) {
      t0 = w.x / v.x;
      t1 = w2.x / v.x;
    }
    else {
      t0 = w.y / v.y;
      t1 = w2.y / v.y;
    }
    if (t0 > t1) {                // must have t0 smaller than t1
      double t=t0; t0=t1; t1=t;    // swap if not
    }
    if (t0 > 1 || t1 < 0) {
      return 0;     // NO overlap
    }
    t0 = t0<0? 0 : t0;            // clip to min 0
    t1 = t1>1? 1 : t1;            // clip to max 1
    if (t0 == t1) {               // intersect is a point
      I0 = S2.P0 + t0 * v;
      return 1;
    }

    // they overlap in a valid subsegment
    I0 = S2.P0 + t0 * v;
    I1 = S2.P0 + t1 * v;
    return 2;
  }

  // the segments are skew and may intersect in a point
  // get the intersect parameter for S1
  double     sI = perp(v,w) / D;
  if (sI < 0 || sI > 1)           // no intersect with S1
    return 0;

  // get the intersect parameter for S2
  double     tI = perp(u,w) / D;
  if (tI < 0 || tI > 1)           // no intersect with S2
    return 0;

  I0 = S1.P0 + sI * u;            // compute S1 intersect point
  return 1;
}


// inSegment(): determine if a point is inside a segment
//    Input:  a point P, and a collinear segment S
//    Return: 1 = P is inside S
//            0 = P is not inside S
int
inSegment( Point P, Segment S )
{
  if (S.P0.x != S.P1.x) { // S is not vertical
    if (S.P0.x <= P.x && P.x <= S.P1.x)
      return 1;
    if (S.P0.x >= P.x && P.x >= S.P1.x)
      return 1;
  }
  else { // S is vertical, so test y coordinate
    if (S.P0.y <= P.y && P.y <= S.P1.y)
      return 1;
    if (S.P0.y >= P.y && P.y >= S.P1.y)
      return 1;
  }
  return 0;
}


// wrappers ...

IntersectionResult intersectLineSegLineSeg2D( const carmen_point_t& P0,
                                              const carmen_point_t& P1,
                                              const carmen_point_t& Q0,
                                              const carmen_point_t& Q1,
                                              carmen_point_t& intersection1,
                                              carmen_point_t& intersection2 )
{
  Segment S1, S2;
  S1.P0 = Vector2d(P0);
  S1.P1 = Vector2d(P1);
  S2.P0 = Vector2d(Q0);
  S2.P1 = Vector2d(Q1);
  Vector2d i1, i2;
  int ret = intersect2D_Segments( S1, S2, i1, i2 );
  intersection1.x = i1.x;
  intersection1.y = i1.y;
  intersection2.x = i2.x;
  intersection2.y = i2.y;
  return (IntersectionResult)ret;
}

IntersectionResult intersectLineSegLineSeg2D( const Pose& P0,
                                              const Pose& P1,
                                              const Pose& Q0,
                                              const Pose& Q1,
                                              Pose& intersection1,
                                              Pose& intersection2 )
{
  Segment S1, S2;
  S1.P0 = Vector2d(P0);
  S1.P1 = Vector2d(P1);
  S2.P0 = Vector2d(Q0);
  S2.P1 = Vector2d(Q1);
  Vector2d i1, i2;
  int ret = intersect2D_Segments( S1, S2, i1, i2 );
  intersection1.x = i1.x;
  intersection1.y = i1.y;
  intersection2.x = i2.x;
  intersection2.y = i2.y;
  return (IntersectionResult)ret;
}

