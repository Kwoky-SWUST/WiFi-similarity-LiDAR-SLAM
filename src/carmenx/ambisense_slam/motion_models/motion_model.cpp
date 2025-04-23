#include <carmen/carmen.h>
#include <iostream>
#include "motion_model.h" 

using namespace std;


// --------------------------------------------------------------------------
MotionModel::
    MotionModel( MotionModelParameters* mmParams,
                 std::string            type )
// --------------------------------------------------------------------------
{ 
  this->params = mmParams; 
  m_sType = type; 
}


// --------------------------------------------------------------------------
void MotionModel::
    print( std::string indentation, std::ostream & ostr )
// --------------------------------------------------------------------------
{
#define IND ostr << indentation
  IND << "Type:                                         " << m_sType << "\n";
  IND << "Parameters:\n";
  params->print( indentation + "  ", ostr );
}


// --------------------------------------------------------------------------
double MotionModel::
    getProbability( const carmen_point_t& /*transformation*/,
                    const carmen_point_t& /*odometry*/ ) const
// --------------------------------------------------------------------------
{
  cout << "WARNING: getProbability not implemented for used motion model!" << endl;
  return 1.0;
}


// --------------------------------------------------------------------------
std::string MotionModel::
    getType() const
// --------------------------------------------------------------------------
{
  return m_sType;
}
