#include <iostream>

#include "regular_grid_geometry_6d.h"
#include "regular_grid_geometry_3d.h"


std::ostream & operator << ( std::ostream & os, const RegularGridGeometry3D & geometry ) {
  const std::string separator( "    " );
  os << "Grid geometry settings:\n";
  os << separator << "Desired: " << geometry.m_desiredDimensions << " and active dimensions: " << geometry.m_activeDimensions << '\n';
  os << separator << "Min and max coords: " << geometry.m_minCoord << " -> " << geometry.m_maxCoord << "\n";
  os << separator << "Grid origin and width: " << geometry.m_gridOrigin << " , " << geometry.getGridWidth() << "\n";
  os << separator << "Grid cell width (m_lutResolutionXYZ): " << geometry.m_lutResolutionXYZ << "\n";
  os << separator << "Number of cells per dimension (m_numCells): " << geometry.m_numCells << "\n";
  os << separator << "Axis centering (m_centeredAtAxis) enabled for: " << geometry.m_centeredAtAxis << "\n";
  os << separator << "Symmetric measurements (m_symmetricMeasurements) enabled for: " << geometry.m_symmetricMeasurements << "\n";
  os << separator << "Lookup table multipliers: " << geometry.getLutMultipliers() << "\n";
  return os;
}


RegularGridGeometry3D::RegularGridGeometry3D()
  : m_desiredDimensions(1,1,1),
    m_lutResolutionXYZ( 1.,1.,1. )
{
  // relying on default initialization!
}

RegularGridGeometry3D::RegularGridGeometry3D( const RegularGridGeometry3D& other )
  : m_desiredDimensions( other.m_desiredDimensions ),
    m_activeDimensions( other.m_activeDimensions ),
    m_lutResolutionXYZ( other.m_lutResolutionXYZ ),
    m_numCells( other.m_numCells ),
    m_centeredAtAxis( other.m_centeredAtAxis ),
    m_minCoord( other.m_minCoord ),
    m_maxCoord( other.m_maxCoord ),
    m_gridOrigin( other.m_gridOrigin ),
    m_symmetricMeasurements( other.m_symmetricMeasurements ),
    m_gridWidth( other.getGridWidth() ),
    m_lutMultipliers( other.m_lutMultipliers )
{
  m_numDimensionsUsed = other.m_numDimensionsUsed;
}

RegularGridGeometry3D::RegularGridGeometry3D( RegularGridGeometry3D const * const other )
  : m_desiredDimensions( other->m_desiredDimensions ),
    m_activeDimensions( other->m_activeDimensions ),
    m_lutResolutionXYZ( other->m_lutResolutionXYZ ),
    m_numCells( other->m_numCells ),
    m_centeredAtAxis( other->m_centeredAtAxis ),
    m_minCoord( other->m_minCoord ),
    m_maxCoord( other->m_maxCoord ),
    m_gridOrigin( other->m_gridOrigin ),
    m_symmetricMeasurements( other->m_symmetricMeasurements ),
    m_gridWidth( other->getGridWidth() ),
    m_lutMultipliers( other->m_lutMultipliers )
{
  m_numDimensionsUsed = other->m_numDimensionsUsed;
}


RegularGridGeometry3D::RegularGridGeometry3D( const RegularGridGeometry6D& other ) {
  // loop through the x,y,z fields of the 6d version and apply it to the 3d version
  m_numDimensionsUsed = 0;
  for( int i=0; i<3; ++i ) {
    m_centeredAtAxis[i] = other.m_centeredAtAxis[i];
    m_desiredDimensions[i] = other.m_desiredDimensions[i];
    m_gridWidth[i] = other.getGridWidth()[i];
    m_lutResolutionXYZ[i] = other.m_lutResolutionXYZTheta[i];
    m_maxCoord[i] = other.m_maxCoord[i];
    m_minCoord[i] = other.m_minCoord[i];
    m_symmetricMeasurements[i] = other.m_symmetricMeasurements[i];
    if( m_desiredDimensions[i] )
      ++m_numDimensionsUsed;
  }
}


RegularGridGeometry3D& RegularGridGeometry3D::operator =( const RegularGridGeometry3D& other ) {
  if(this != &other) {
    m_centeredAtAxis = other.m_centeredAtAxis;
    m_desiredDimensions = other.m_desiredDimensions;
    m_activeDimensions = other.m_activeDimensions;
    m_gridWidth = other.getGridWidth();
    m_lutResolutionXYZ = other.m_lutResolutionXYZ;
    m_maxCoord = other.m_maxCoord;
    m_minCoord = other.m_minCoord;
    m_numDimensionsUsed = other.m_numDimensionsUsed;
    m_symmetricMeasurements = other.m_symmetricMeasurements;
    m_numCells = other.m_numCells;
    m_lutMultipliers = other.m_lutMultipliers;
    m_gridOrigin = other.m_gridOrigin;
  }
  return *this;
}

RegularGridGeometry3D& RegularGridGeometry3D::operator =( const RegularGridGeometry6D& other ) {
  if(this != (RegularGridGeometry3D*)&other) {
    // loop through the x,y,z fields of the 6d version and apply it to the 3d version
    m_numDimensionsUsed = 0;
    for( int i=0; i<3; ++i ) {
      m_centeredAtAxis[i] = other.m_centeredAtAxis[i];
      m_desiredDimensions[i] = other.m_desiredDimensions[i];
      m_gridWidth[i] = other.getGridWidth()[i];
      m_lutResolutionXYZ[i] = other.m_lutResolutionXYZTheta[i];
      m_maxCoord[i] = other.m_maxCoord[i];
      m_minCoord[i] = other.m_minCoord[i];
      m_symmetricMeasurements[i] = other.m_symmetricMeasurements[i];
      if( m_desiredDimensions[i] )
        ++m_numDimensionsUsed;
    }
  }
  return *this;
}


void RegularGridGeometry3D::setMinCoords( const Vector3d& min ) {
  m_minCoord = min;
}

void RegularGridGeometry3D::setMinCoords( const double x, const double y, const double z ) {
  setMinCoords( Vector3d( x,y,z ) );
}

void RegularGridGeometry3D::setMaxCoords( const Vector3d& max ) {
  m_maxCoord = max;
}

void RegularGridGeometry3D::setMaxCoords( const double x, const double y, const double z ) {
  setMaxCoords( Vector3d(x,y,z) );
}


void RegularGridGeometry3D::setCoveredDimensions( const bool x, const bool y, const bool z ) {
  setCoveredDimensions( Vector3u( x,y,z ) );
}

//void RegularGridGeometry3D::setCoveredDimensions( const unsigned int x, const unsigned int y, const unsigned int z ) {
//  setCoveredDimensions( Vector3u( x,y,z ) );
//}

void RegularGridGeometry3D::setCoveredDimensions( const Vector3u& dimFlags ) {
  m_desiredDimensions = dimFlags;
}

void RegularGridGeometry3D::enable2d() {
  m_desiredDimensions = Vector3u( 1,1,0 );
}

void RegularGridGeometry3D::enable3d() {
  m_desiredDimensions = Vector3u( 1,1,1 );
}


void RegularGridGeometry3D::setCenteredAxis( const bool x, const bool y, const bool z ) {
  setCenteredAxis( Vector3u( x,y,z ) );
}

//void RegularGridGeometry3D::setCenteredAxis( const unsigned int x, const unsigned int y, const unsigned int z ) {
//  setCenteredAxis( Vector3u( x,y,z ) );
//}

void RegularGridGeometry3D::setCenteredAxis( const Vector3u& centeredFlags ) {
  m_centeredAtAxis = centeredFlags;
}


void RegularGridGeometry3D::setSymmetricAxis( const bool x, const bool y, const bool z ) {
  setSymmetricAxis( Vector3u( x,y,z ) );
}

//void RegularGridGeometry3D::setSymmetricAxis( const unsigned int x, const unsigned int y, const unsigned int z ) {
//  setSymmetricAxis( Vector3u( x,y,z ) );
//}

void RegularGridGeometry3D::setSymmetricAxis( const Vector3u& symmetricFlags ) {
  m_symmetricMeasurements = symmetricFlags;
}


void RegularGridGeometry3D::setCellWidth( const Vector3d& widthXYZ ) {
  m_lutResolutionXYZ = widthXYZ;
}

void RegularGridGeometry3D::setCellWidth( const double wx, const double wy, const double wz ) {
  setCellWidth( Vector3d(wx,wy,wz) );
}

void RegularGridGeometry3D::setCellWidth( const double widthXYZ ) {
  setCellWidth( widthXYZ, widthXYZ, widthXYZ );
}


void RegularGridGeometry3D::initialize() {
  // initialize with the desired dimensions, then validate parameters for each dimension
  // m_desiredDimensions should always be set manually!
  m_activeDimensions = m_desiredDimensions;

  // check for valid grid ranges
  m_gridWidth = m_maxCoord - m_minCoord;
  m_activeDimensions[ GEO3_DIM_X ] &= (m_gridWidth[ GEO3_DIM_X ] > 0);
  m_activeDimensions[ GEO3_DIM_Y ] &= (m_gridWidth[ GEO3_DIM_Y ] > 0);
  m_activeDimensions[ GEO3_DIM_Z ] &= (m_gridWidth[ GEO3_DIM_Z ] > 0);

  // check for valid grid resolutions (cell widths)
  m_activeDimensions[ GEO3_DIM_X ] &= ((m_lutResolutionXYZ[ GEO3_DIM_X ] > 0) && (m_lutResolutionXYZ[ GEO3_DIM_X ] < m_gridWidth[ GEO3_DIM_X ]));
  m_activeDimensions[ GEO3_DIM_Y ] &= ((m_lutResolutionXYZ[ GEO3_DIM_Y ] > 0) && (m_lutResolutionXYZ[ GEO3_DIM_Y ] < m_gridWidth[ GEO3_DIM_Y ]));
  m_activeDimensions[ GEO3_DIM_Z ] &= ((m_lutResolutionXYZ[ GEO3_DIM_Z ] > 0) && (m_lutResolutionXYZ[ GEO3_DIM_Z ] < m_gridWidth[ GEO3_DIM_Z ]));

  // get the number of validly configured dimensions
  m_numDimensionsUsed = m_activeDimensions[GEO3_DIM_X] + m_activeDimensions[GEO3_DIM_Y] + m_activeDimensions[GEO3_DIM_Z];

  // compute grid parameters
  unsigned int multiplier = 1;
  for( int iDim=0; iDim<GEO3_NUM_DIMENSIONS; ++iDim ) {
    if( m_activeDimensions[iDim] ) {
      double grid_index_min, grid_index_max;
      if( m_centeredAtAxis[iDim] ) {
        // map to grid coordinates, with centered cells
        grid_index_min = floor( (m_minCoord[iDim] - 0.5*m_lutResolutionXYZ[iDim]) / m_lutResolutionXYZ[iDim] );
        grid_index_max = ceil( (m_maxCoord[iDim] - 0.5*m_lutResolutionXYZ[iDim]) / m_lutResolutionXYZ[iDim] );
        // compute grid origin
        m_gridOrigin[iDim] = (grid_index_min + 0.5) * m_lutResolutionXYZ[iDim];
      } else {
        // map to grid coordinates
        grid_index_min = floor( m_minCoord[iDim] / m_lutResolutionXYZ[iDim] );
        grid_index_max = ceil( m_maxCoord[iDim] / m_lutResolutionXYZ[iDim] );
        // compute grid origin
        m_gridOrigin[iDim] = (double)grid_index_min * m_lutResolutionXYZ[iDim];
      }
      // set the number of cells for this dimension
      m_numCells[iDim] = grid_index_max - grid_index_min;
      // update grid width
      m_gridWidth[iDim] = m_numCells[iDim] * m_lutResolutionXYZ[iDim];
      // update lookup table multipliers
      m_lutMultipliers[iDim] = multiplier;
      multiplier *= m_numCells[iDim];
    } else {
      m_numCells[iDim] = 1;
      m_lutMultipliers[iDim] = 0;
      m_gridWidth[iDim] = 0;
      m_gridOrigin[iDim] = 0.;
    }
  }
}


void RegularGridGeometry3D::initialize( const Vector3d& min, const Vector3d& max, const double cellWidth ) {
  initialize( min, max, Vector3d(cellWidth,cellWidth,cellWidth) );
}


void RegularGridGeometry3D::initialize( const Vector3d& min, const Vector3d& max, const Vector3d& cellWidth ) {
  setMinCoords(min);
  setMaxCoords(max);
  setCellWidth(cellWidth);
  initialize();
}

const Vector3i RegularGridGeometry3D::getCellIndex( const Vector3d& world_coords ) const {
  Vector3d offset( world_coords - m_gridOrigin );
  // compute cell index
  offset.x /= m_lutResolutionXYZ.x;
  offset.y /= m_lutResolutionXYZ.y;
  offset.z /= m_lutResolutionXYZ.z;
  // make last interval inclusive [(N-1)*width,N*width] while all others are exclusive [(N-1)*width,N*width)
  if( offset.x >= m_numCells.x && (offset.x - FLT_EPSILON) <= m_numCells.x ) offset.x = m_numCells.x - 1;
  if( offset.y >= m_numCells.y && (offset.y - FLT_EPSILON) <= m_numCells.y ) offset.y = m_numCells.y - 1;
  if( offset.z >= m_numCells.z && (offset.z - FLT_EPSILON) <= m_numCells.z ) offset.z = m_numCells.z - 1;
  return Vector3i( floor(offset.x), floor(offset.y), floor(offset.z) ); // todo: minor floating point rounding errors may occur
  //return Vector3i( floor(offset.x+0.5), floor(offset.y+0.5), floor(offset.z+0.5) );
  //return Vector3i( static_cast<int>(std::floor(offset.x)), static_cast<int>(std::floor(offset.y)), static_cast<int>(std::floor(offset.z)) );
  //return Vector3i( static_cast<int>(round(offset.x-0.5)), static_cast<int>(round(offset.y-0.5)), static_cast<int>(round(offset.z-0.5)) );
}
