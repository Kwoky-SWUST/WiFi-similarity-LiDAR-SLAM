#include "regular_grid_geometry_3d.h"

#include "regular_grid_geometry_6d.h"

RegularGridGeometry6D::RegularGridGeometry6D()
{
  // relying on default initialization!
}

RegularGridGeometry6D::RegularGridGeometry6D( const RegularGridGeometry6D& other )
{
  // todo ...
}

RegularGridGeometry6D::RegularGridGeometry6D( RegularGridGeometry6D const * const other )
{
  // todo ...
}

RegularGridGeometry6D::RegularGridGeometry6D( const RegularGridGeometry3D& other )
{
  // todo ...
}

RegularGridGeometry6D& RegularGridGeometry6D::operator =( const RegularGridGeometry6D& other )
{
  // todo ...
  return *this;
}

const bool* RegularGridGeometry6D::getActiveDimensions() const
{
  return m_activeDimensions;
}

const bool* RegularGridGeometry6D::getCenteredAtAxis() const
{
  return m_centeredAtAxis;
}

const bool* RegularGridGeometry6D::getDesiredDimensions() const
{
  return m_desiredDimensions;
}

const double* RegularGridGeometry6D::getGridOrigin() const
{
  return m_gridOrigin;
}

const unsigned int* RegularGridGeometry6D::getLutMultipliers() const
{
  return m_lutMultipliers;
}

const double* RegularGridGeometry6D::getLutResolutionXyzTheta() const
{
  return m_lutResolutionXYZTheta;
}

const double* RegularGridGeometry6D::getMaxCoord() const
{
  return m_maxCoord;
}

const double* RegularGridGeometry6D::getMinCoord() const
{
  return m_minCoord;
}

const unsigned int* RegularGridGeometry6D::getNumCells() const
{
  return m_numCells;
}

const bool* RegularGridGeometry6D::getSymmetricMeasurements() const
{
  return m_symmetricMeasurements;
}
