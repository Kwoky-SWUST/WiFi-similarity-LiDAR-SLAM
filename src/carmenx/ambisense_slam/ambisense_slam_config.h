#ifndef SCIROCO_CARMEN__AMBISENSE_SLAM__AMBISENSE_SLAM_CONFIG_H
#define SCIROCO_CARMEN__AMBISENSE_SLAM__AMBISENSE_SLAM_CONFIG_H

#ifndef ENABLE_OUTPUT_STYLING
#define ENABLE_OUTPUT_STYLING
#endif

#include <utils/text/colors.h>

#define IF_PERFORMANCE_EVAL(x) {}


#define VERBOSE_OUT                if ( carmen_carp_get_verbose() ) std::cout

#define HAVE_PARTICLE_TRAJECTORY
// #undef HAVE_PARTICLE_TRAJECTORY
#define HAVE_LOG_WEIGHTS

#include "ambisense_slam_messages.h"
#include "filters/pf2_6d.h"
// #include "filters/pf2_xytheta.h"
//
#include <algorithm> // for copy
#include <iostream>
#include <map>
#include <vector>


typedef pf2::Particle6D                              TagParticle;
typedef pf2::ParticleFilter6D                        TagParticleFilter;
typedef std::map<std::string, TagParticleFilter* >   TagParticleFilterMap;

typedef pf2::Particle6D        TrackingParticle;
typedef pf2::ParticleFilter6D  TrackingParticleFilter;

typedef pf2::Particle6D        Particle;
typedef pf2::ParticleFilter6D  LocalizationParticleFilter;

#include "filters/pf2_history_6d.h"
typedef pf2::ParticleFilterHistory6D  LocalizationPFHistory;

// typedef pf2::ParticleXYTheta        Particle;
// typedef pf2::ParticleFilterXYTheta  LocalizationParticleFilter;


#endif // SCIROCO_CARMEN__AMBISENSE_SLAM__AMBISENSE_SLAM_CONFIG_H
