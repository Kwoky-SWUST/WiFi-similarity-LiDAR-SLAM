/** @addtogroup ambisense_slam libambisense_slam_interface **/
// @{

/** @file   ambisense_slam.h
 *  @brief  Localization and mapping module in the scope of AmbiSense
 *  @todo   Timing consistency may be violated, because odometry readings might
 *          be newer than the latest RFID measurement
 *  @author Philipp Vorst
 */
#ifndef SCIROCO_CARMEN__AMBISENSE_SLAM_UTILS_H
#define SCIROCO_CARMEN__AMBISENSE_SLAM_UTILS_H


/** Write the tag particle filter data to output stream */
std::ostream&
    operator << ( std::ostream& ostr, pf2::ParticleFilter6D& pf );


#endif // SCIROCO_CARMEN__AMBISENSE_SLAM_UTILS_H
