#include <carmen/carmen.h>
#include "grid_raytracer.h"
#include <iostream>
#include <stdlib.h>
#include <algorithm>


// --------------------------------------------------------------------------
GridRaytracer::
GridRaytracer( carmen_map_t* map,
               const double occupiedProbThreshold )
// --------------------------------------------------------------------------
{
  m_pMap = map;
  m_dOccupiedProbThreshold = occupiedProbThreshold;
  m_dInverseResolution = 1.0 / m_pMap->config.resolution;
  m_iMaxNumCells = std::max( m_pMap->config.x_size, m_pMap->config.y_size );
}


// --------------------------------------------------------------------------
inline int
sgn( int x )
// --------------------------------------------------------------------------
{
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}


// --------------------------------------------------------------------------
double GridRaytracer::
getExpectedSquaredDistance( const carmen_point_t* p,
                            bool treatUnknownCellsAsInfinity )
// --------------------------------------------------------------------------
{
  // Remember start cells:
  int sx = lrint( p->x * m_dInverseResolution );
  int sy = lrint( p->y * m_dInverseResolution );

  int max_x = m_pMap->config.x_size;
  int max_y = m_pMap->config.y_size;

  if ( sx < 0 || sy < 0 || sx >= max_x || sy >= max_y )
    return INFINITY;

  int pdx, pdy, ddx, ddy, es, el, err;

  /* Entfernung in beiden Dimensionen berechnen */
  int dx = (int)( cos( p->theta ) * m_iMaxNumCells );
  int dy = (int)( sin( p->theta ) * m_iMaxNumCells );

  /* Vorzeichen des Inkrements bestimmen */
  int incx = sgn( dx );
  int incy = sgn( dy );
  if ( dx < 0 ) dx = -dx;
  if ( dy < 0 ) dy = -dy;

  /* feststellen, welche Entfernung größer ist */
  if ( dx > dy )
  {
    /* x ist schnelle Richtung */
    pdx=incx; pdy=0;    /* pd. ist Parallelschritt */
    ddx=incx; ddy=incy; /* dd. ist Diagonalschritt */
    es =dy;   el =dx;   /* Fehlerschritte schnell, langsam */
  }
  else
  {
    /* y ist schnelle Richtung */
    pdx=0;    pdy=incy; /* pd. ist Parallelschritt */
    ddx=incx; ddy=incy; /* dd. ist Diagonalschritt */
    es =dx;   el =dy;   /* Fehlerschritte schnell, langsam */
  }

  /* Initialisierungen vor Schleifenbeginn */
  int x = sx;
  int y = sy;
  err = el / 2;

  /* Pixel berechnen */
  //   for ( int t = 0; t < el; ++t ) /* t zaehlt die Pixel, el ist auch Anzahl */
  while ( m_pMap->map[x][y] < m_dOccupiedProbThreshold )
  {
    if ( treatUnknownCellsAsInfinity && m_pMap->map[x][y] < 0 )
      return INFINITY;

    /* Aktualisierung Fehlerterm */
    err -= es;
    if ( err < 0 )
    {
      /* Fehlerterm wieder positiv (>=0) machen */
      err += el;
      /* Schritt in langsame Richtung, Diagonalschritt */
      x += ddx;
      y += ddy;
    }
    else
    {
      /* Schritt in schnelle Richtung, Parallelschritt */
      x += pdx;
      y += pdy;
    }

    if ( x < 0 || y < 0 || x >= max_x || y >= max_y )
      return INFINITY;
  }

  double sqr_distance = ( carmen_square( sx - x ) + carmen_square( sy - y ) ) * carmen_square( m_pMap->config.resolution );
  return sqr_distance;
}
