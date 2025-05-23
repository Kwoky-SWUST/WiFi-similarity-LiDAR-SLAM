/*********************************************************
 *
 * This source code is part of the Carnegie Mellon Robot
 * Navigation Toolkit (CARMEN)
 *
 * CARMEN Copyright (c) 2002 Michael Montemerlo, Nicholas
 * Roy, Sebastian Thrun, Dirk Haehnel, Cyrill Stachniss,
 * and Jared Glover
 *
 * CARMEN is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option)
 * any later version.
 *
 * CARMEN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General
 * Public License along with CARMEN; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307 USA
 *
 ********************************************************/

#include <carmen/carmen.h>

#ifndef COMPILE_WITHOUT_LASER_SUPPORT
#include "robot_laser.h"
#endif

#include "robot_sonar.h"
#include "robot_bumper.h"

#include "robot_central.h"

carmen_robot_config_t carmen_robot_config;
carmen_base_odometry_message carmen_robot_latest_odometry;
carmen_base_odometry_message carmen_robot_odometry[CARMEN_ROBOT_MAX_READINGS];
double carmen_robot_collision_avoidance_frequency = 10.0;
double carmen_robot_sensor_time_of_last_update = -1;

static char *robot_host;
static double turn_before_driving_if_heading_bigger_than = M_PI / 2;
static int odometry_count = 0;
static double odometry_local_timestamp[CARMEN_ROBOT_MAX_READINGS];

#ifndef COMPILE_WITHOUT_LASER_SUPPORT
static int use_laser = 1;
#endif

static int use_sonar = 1;
static int use_bumper = 0;

static int collision_avoidance = 1;

// Don't change; 3d controller isn't ready for main distribution yet - Jared
static int use_3d_control = 0;
static double control_lookahead = 10.0;
static double control_lookahead_approach_dist = 0.3;

static double theta_gain;
static double theta_d_gain;
static double disp_gain;
static double robot_sensor_timeout = 3.0;
static double command_tv = 0, command_rv = 0;
static double time_of_last_command;

static carmen_traj_point_t start_position;
static carmen_traj_point_t goal;
static int goal_is_final = 1;
static double vector_distance;
static double vector_angle;
static int following_vector = 0;
static int following_trajectory = 0;

static void publish_vector_status( double distance, double angle );

static inline double get_odometry_skew( void ) {
  if( strcmp(robot_host, carmen_robot_latest_odometry.host) == 0 )
    return 0;
  else
    return carmen_running_average_report(CARMEN_ROBOT_ODOMETRY_AVERAGE);
}

int carmen_robot_get_skew( int msg_count, double *skew, int data_type, char *hostname ) {
  if( msg_count < CARMEN_ROBOT_ESTIMATES_CONVERGE )
    return 0;

  if( strcmp(robot_host, hostname) == 0 )
    *skew = 0;
  else
    *skew = carmen_running_average_report(data_type);

  if( odometry_count < CARMEN_ROBOT_ESTIMATES_CONVERGE ) {
    carmen_warn("Waiting for odometry to accumulate\n");
    return 0;
  }

  return 1;
}

void carmen_robot_update_skew( int data_type, int *count, double time, char *hostname ) {
  if( strcmp(robot_host, hostname) == 0 )
    *count = CARMEN_ROBOT_ESTIMATES_CONVERGE;

  if( *count <= CARMEN_ROBOT_ESTIMATES_CONVERGE )
    (*count)++;

  carmen_running_average_add(data_type, carmen_get_time() - time);
}

double carmen_robot_get_fraction( double timestamp, double skew, int *low, int *high ) {
  double fraction;
  double corrected_timestamp;
  double low_timestamp, high_timestamp;
  double odometry_skew;
  int i;

  corrected_timestamp = timestamp + skew;
  odometry_skew = get_odometry_skew();

  *low = 0;
  *high = 1;
  for( i = 0; i < CARMEN_ROBOT_MAX_READINGS; i++ )
    if( corrected_timestamp < carmen_robot_odometry[i].timestamp + odometry_skew ) {
      if( i == 0 ) {
        *low = 0;
        *high = 1;
      } else {
        *low = i - 1;
        *high = i;
      }
      break;
    }

  if( i == CARMEN_ROBOT_MAX_READINGS ) {
    *low = i - 2;
    *high = i - 1;
  }

  low_timestamp = carmen_robot_odometry[*low].timestamp + odometry_skew;
  high_timestamp = carmen_robot_odometry[*high].timestamp + odometry_skew;

  fraction = (corrected_timestamp - low_timestamp) / (high_timestamp - low_timestamp);

  return fraction;
}

double carmen_robot_interpolate_heading( double head1, double head2, double fraction ) {
  double result;

  if( head1 == head2 || fraction == 0 )
    return head1;
  if( fraction == 1. )
    return head2;

  if( head1 > 0 && head2 < 0 && head1 - head2 > M_PI ) {
    head2 += 2 * M_PI;
    result = head1 + fraction * (head2 - head1);
    if( result > M_PI )
      result -= 2 * M_PI;
    return result;
  } else if( head1 < 0 && head2 > 0 && head2 - head1 > M_PI ) {
    head1 += 2 * M_PI;
    result = head1 + fraction * (head2 - head1);
    if( result > M_PI )
      result -= 2 * M_PI;
    return result;
  } else
    return head1 + fraction * (head2 - head1);
}

void carmen_robot_send_base_velocity_command( void ) {
  IPC_RETURN_TYPE err;
  static carmen_base_velocity_message v;

  printf("command_tv=%f\n", command_tv);

  v.host = carmen_get_host();

  if( collision_avoidance ) {
#ifndef COMPILE_WITHOUT_LASER_SUPPORT
    if( use_laser ) {
      command_tv = carmen_clamp(carmen_robot_laser_min_rear_velocity(), command_tv,
                                carmen_robot_laser_max_front_velocity());
      printf("debug II: carmen_robot_laser_min_rear_velocity=%f\n", carmen_robot_laser_min_rear_velocity());
    }
#endif
    if( use_sonar )
      command_tv = carmen_clamp(carmen_robot_sonar_min_rear_velocity(), command_tv,
                                carmen_robot_sonar_max_front_velocity());

    if( use_bumper && carmen_robot_bumper_on() ) {
      printf("Setting velocity zero because of bumpers\n");
      command_tv = 0;
      command_rv = 0;
    }
  }
  printf("debug: allow_rear_motion=%d\n", carmen_robot_config.allow_rear_motion);
  if( !carmen_robot_config.allow_rear_motion && command_tv < 0 )
    command_tv = 0.0;

  v.tv = carmen_clamp(-carmen_robot_config.max_t_vel, command_tv, carmen_robot_config.max_t_vel);

  printf("debug II: v.tv=%f\n", v.tv);
  v.rv = carmen_clamp(-carmen_robot_config.max_r_vel, command_rv, carmen_robot_config.max_r_vel);

  v.timestamp = carmen_get_time();

  printf("debug: sending velocity tv=%f, rv=%f\n", v.tv, v.rv);

  err = IPC_publishData(CARMEN_BASE_VELOCITY_NAME, &v);
  carmen_test_ipc(err, "Could not publish", CARMEN_BASE_VELOCITY_NAME);
}

void carmen_robot_stop_robot( int how ) {
  command_tv = 0.0;
  if( how == CARMEN_ROBOT_ALL_STOP )
    command_rv = 0.0;
  if( following_vector || following_trajectory )
    command_rv = 0.0;
  following_vector = 0;
  following_trajectory = 0;
  publish_vector_status(0, 0);

  carmen_robot_send_base_velocity_command();
}

static void base_odometry_handler( void ) {
  int i;

  if( strcmp(robot_host, carmen_robot_latest_odometry.host) == 0 )
    odometry_count = CARMEN_ROBOT_ESTIMATES_CONVERGE;
  else
    odometry_count++;

  carmen_warn("o");

  for( i = 0; i < CARMEN_ROBOT_MAX_READINGS - 1; i++ ) {
    carmen_robot_odometry[i] = carmen_robot_odometry[i + 1];
    odometry_local_timestamp[i] = odometry_local_timestamp[i + 1];
  }
  carmen_robot_odometry[CARMEN_ROBOT_MAX_READINGS - 1] = carmen_robot_latest_odometry;
  odometry_local_timestamp[CARMEN_ROBOT_MAX_READINGS - 1] = carmen_get_time();

  carmen_running_average_add(
      CARMEN_ROBOT_ODOMETRY_AVERAGE,
      odometry_local_timestamp[CARMEN_ROBOT_MAX_READINGS - 1] - carmen_robot_latest_odometry.timestamp);

  if( collision_avoidance ) {

    if( carmen_robot_bumper_on() ) {
      fprintf(stderr, "S");
      carmen_robot_stop_robot(CARMEN_ROBOT_ALL_STOP);
      command_tv = 0;
      command_rv = 0;
    }

#ifndef COMPILE_WITHOUT_LASER_SUPPORT
    if( carmen_robot_latest_odometry.tv > 0 && carmen_robot_laser_max_front_velocity() < carmen_robot_latest_odometry.tv
        && command_tv > carmen_robot_laser_max_front_velocity() ) {

      if( carmen_robot_laser_max_front_velocity() <= 0.0 ) {
        command_tv = 0;
        fprintf(stderr, "S");
        carmen_robot_stop_robot(CARMEN_ROBOT_ALLOW_ROTATE);
      } else {
        command_tv = carmen_robot_laser_max_front_velocity();
        carmen_robot_send_base_velocity_command();
      }
    } else if( carmen_robot_latest_odometry.tv < 0
               && carmen_robot_laser_min_rear_velocity() > carmen_robot_latest_odometry.tv
               && command_tv < carmen_robot_laser_min_rear_velocity() ) {
      if( carmen_robot_laser_min_rear_velocity() >= 0.0 ) {
        fprintf(stderr, "S");
        command_tv = 0;
        carmen_robot_stop_robot(CARMEN_ROBOT_ALLOW_ROTATE);
      } else {
        command_tv = carmen_robot_laser_min_rear_velocity();
        carmen_robot_send_base_velocity_command();
      }
    }
#endif
  } // End of if (collision_avoidance)

  if( use_sonar )
    carmen_robot_correct_sonar_and_publish();
  if( use_bumper )
    carmen_robot_correct_bumper_and_publish();
#ifndef COMPILE_WITHOUT_LASER_SUPPORT
  if( use_laser )
    carmen_robot_correct_laser_and_publish();
#endif
}

static void publish_vector_status( double distance, double angle ) {
  static carmen_robot_vector_status_message msg;
  int err;

  msg.host = carmen_get_host();
  msg.timestamp = carmen_get_time();
  msg.vector_distance = distance;
  msg.vector_angle = angle;

  err = IPC_publishData(CARMEN_ROBOT_VECTOR_STATUS_NAME, &msg);
  carmen_test_ipc(err, "Could not publish", CARMEN_ROBOT_VECTOR_STATUS_NAME);
}

static void velocity_handler( MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData __attribute__ ((unused)) ) {
  carmen_robot_velocity_message v;
  FORMATTER_PTR formatter;
  IPC_RETURN_TYPE err;

  formatter = IPC_msgInstanceFormatter(msgRef);
  err = IPC_unmarshallData(formatter, callData, &v, sizeof(carmen_robot_velocity_message));
  IPC_freeByteArray(callData);

  carmen_test_ipc_return(err, "Could not unmarshall", IPC_msgInstanceName(msgRef));

  time_of_last_command = carmen_get_time();

  command_rv = v.rv;
  command_tv = v.tv;

  following_vector = following_trajectory = 0;
  carmen_robot_send_base_velocity_command();
  publish_vector_status(0, 0);
}

static void follow_vector( void ) {
  double true_angle_difference, angle_difference, displacement;
  double radius;

  double angle_change = carmen_normalize_theta(carmen_robot_latest_odometry.theta - start_position.theta);
  double distance_change = hypot(carmen_robot_latest_odometry.x - start_position.x,
                                 carmen_robot_latest_odometry.y - start_position.y);

  angle_difference = carmen_normalize_theta(vector_angle - angle_change);

  carmen_verbose("angle: o %f s %f : v %f d: %f\n", carmen_robot_latest_odometry.theta, start_position.theta,
                 vector_angle,
                 angle_difference);

  if( fabs(vector_distance) > 0.0 )
    displacement = vector_distance - distance_change;
  else
    displacement = 0.0;

  if( fabs(angle_difference) < carmen_degrees_to_radians(5.0) && fabs(displacement)
                                                                 < carmen_robot_config.approach_dist ) {
    command_tv = 0;
    command_rv = 0;
    following_vector = 0;
    carmen_robot_stop_robot(CARMEN_ROBOT_ALL_STOP);
    publish_vector_status(0, 0);
    return;
  }

  true_angle_difference = angle_difference;

  command_rv = theta_gain * angle_difference;

  if( fabs(angle_difference) > 0.0 && fabs(angle_difference) < carmen_degrees_to_radians(45) )
    command_rv -= theta_d_gain * carmen_robot_latest_odometry.rv;

  command_rv = carmen_clamp(-carmen_robot_config.max_r_vel, command_rv, carmen_robot_config.max_r_vel);

  if( (fabs(carmen_robot_latest_odometry.tv) <= 0.001
       && fabs(angle_difference) > 0.5 * turn_before_driving_if_heading_bigger_than)
      || fabs(angle_difference) > turn_before_driving_if_heading_bigger_than )
    command_tv = 0;

  /* This part here is iffy. If the rotational velocity is substantial,
   then try and set the translational velocity such that the robot is
   pointing in the right direction about 1/4th of the way
   there. Hopefully, the control loop will correct things by about 1/2
   of the way there, arriving at the destination with the right
   orientation.
   */

  else if( fabs(command_rv) > carmen_robot_config.max_r_vel / 4 ) {
    radius = fabs(displacement / 4 / sin(angle_difference));
    command_tv = fabs(radius * command_rv);
  } else {
    if( goal_is_final ) {
      command_tv = displacement * disp_gain;
      if( fabs(displacement) > 0 && fabs(displacement) < 0.15 )
        command_tv -= carmen_robot_latest_odometry.tv / 2;
    } else {
      command_tv = carmen_robot_config.max_t_vel;
    }
  }

  publish_vector_status(displacement, true_angle_difference);
  carmen_robot_send_base_velocity_command();
}

static void follow_trajectory_3d( void ) {

  /* gain matrix: by rows, [ga gb, gc gd] */
  double ga, gb, gc, gd;
  double x, y, theta, cos_theta, sin_theta;
  double ahead_x, ahead_y, pursuit_x, pursuit_y, dx, dy;

  theta = carmen_normalize_theta(carmen_robot_latest_odometry.theta - start_position.theta);
  cos_theta = cos(theta);
  sin_theta = sin(theta);

  ga = cos_theta;
  gb = sin_theta;
  gc = -sin_theta / control_lookahead;
  gd = cos_theta / control_lookahead;

  x = carmen_robot_latest_odometry.x - start_position.x;
  y = carmen_robot_latest_odometry.y - start_position.y;
  ahead_x = x + cos_theta * control_lookahead;
  ahead_y = y + sin_theta * control_lookahead;

  //dbug
  pursuit_x = goal.x + /*cos(goal.theta) * */control_lookahead;
  pursuit_y = goal.y /* + sin(goal.theta) * control_lookahead */;

  dx = pursuit_x - ahead_x;
  dy = pursuit_y - ahead_y;

  if( sqrt(dx * dx + dy * dy) < control_lookahead_approach_dist ) {
    command_tv = 0;
    command_rv = 0;
    following_trajectory = 0;
    carmen_robot_stop_robot(CARMEN_ROBOT_ALL_STOP);
    publish_vector_status(0, 0);
    return;
  }

  command_tv = disp_gain * (ga * dx + gb * dy);
  command_rv = disp_gain * (gc * dx + gd * dy);

  carmen_robot_send_base_velocity_command();
}

static void vector_move_handler( MSG_INSTANCE msgRef, BYTE_ARRAY callData, void *clientData __attribute__ ((unused)) ) {
  carmen_robot_vector_move_message msg;
  FORMATTER_PTR formatter;
  IPC_RETURN_TYPE err;

  formatter = IPC_msgInstanceFormatter(msgRef);
  err = IPC_unmarshallData(formatter, callData, &msg, sizeof(carmen_robot_vector_move_message));
  IPC_freeByteArray(callData);

  carmen_test_ipc_return(err, "Could not unmarshall", IPC_msgInstanceName(msgRef));

  time_of_last_command = carmen_get_time();

  following_vector = 1;
  if( following_trajectory )
    following_trajectory = 0;

  start_position.x = carmen_robot_latest_odometry.x;
  start_position.y = carmen_robot_latest_odometry.y;
  start_position.theta = carmen_robot_latest_odometry.theta;
  start_position.t_vel = carmen_robot_latest_odometry.tv;
  start_position.r_vel = carmen_robot_latest_odometry.rv;

  vector_angle = msg.theta;
  vector_distance = msg.distance;

  follow_vector();
}

static void follow_trajectory_handler( MSG_INSTANCE msgRef, BYTE_ARRAY callData,
                                       void *clientData __attribute__ ((unused)) ) {
  carmen_robot_follow_trajectory_message msg;
  FORMATTER_PTR formatter;
  IPC_RETURN_TYPE err;

  formatter = IPC_msgInstanceFormatter(msgRef);
  err = IPC_unmarshallData(formatter, callData, &msg, sizeof(carmen_robot_follow_trajectory_message));
  IPC_freeByteArray(callData);

  carmen_test_ipc_return(err, "Could not unmarshall", IPC_msgInstanceName(msgRef));

  time_of_last_command = carmen_get_time();

  if( msg.trajectory_length == 0 )
    return;

  start_position.x = carmen_robot_latest_odometry.x;
  start_position.y = carmen_robot_latest_odometry.y;
  start_position.theta = carmen_robot_latest_odometry.theta;
  start_position.t_vel = carmen_robot_latest_odometry.tv;
  start_position.r_vel = carmen_robot_latest_odometry.rv;

  if( use_3d_control ) {
    following_vector = 0;
    following_trajectory = 1;
    goal = msg.trajectory[0];
    follow_trajectory_3d();
  } else {
    following_vector = 1;
    following_trajectory = 0;

    goal = msg.trajectory[0];
    goal.x -= msg.robot_position.x;
    goal.y -= msg.robot_position.y;

    if( msg.trajectory_length == 1 )
      goal_is_final = 1;
    else
      goal_is_final = 0;

    vector_distance = hypot(goal.x, goal.y);

    if( vector_distance < carmen_robot_config.approach_dist )
      vector_angle = carmen_normalize_theta(goal.theta);
    else {
      vector_angle = carmen_normalize_theta(atan2(goal.y, goal.x));
      vector_angle = carmen_normalize_theta(vector_angle - msg.robot_position.theta);
    }

    follow_vector();
  }
  free(msg.trajectory);
}

static int initialize_robot_ipc( void ) {
  IPC_RETURN_TYPE err;

  err = IPC_defineMsg(CARMEN_BASE_VELOCITY_NAME, IPC_VARIABLE_LENGTH,
  CARMEN_BASE_VELOCITY_FMT);
  carmen_test_ipc_exit(err, "Could not define", CARMEN_BASE_VELOCITY_NAME);

  err = IPC_defineMsg(CARMEN_ROBOT_VELOCITY_NAME,
  IPC_VARIABLE_LENGTH,
                      CARMEN_ROBOT_VELOCITY_FMT);
  carmen_test_ipc_exit(err, "Could not define", CARMEN_ROBOT_VELOCITY_NAME);

  err = IPC_defineMsg(CARMEN_ROBOT_VECTOR_MOVE_NAME, IPC_VARIABLE_LENGTH,
  CARMEN_ROBOT_VECTOR_MOVE_FMT);
  carmen_test_ipc_exit(err, "Could not define", CARMEN_ROBOT_VECTOR_MOVE_NAME);

  err = IPC_defineMsg(CARMEN_ROBOT_FOLLOW_TRAJECTORY_NAME, IPC_VARIABLE_LENGTH,
  CARMEN_ROBOT_FOLLOW_TRAJECTORY_FMT);
  carmen_test_ipc_exit(err, "Could not define", CARMEN_ROBOT_FOLLOW_TRAJECTORY_NAME);

  err = IPC_defineMsg(CARMEN_ROBOT_VECTOR_STATUS_NAME, IPC_VARIABLE_LENGTH,
  CARMEN_ROBOT_VECTOR_STATUS_FMT);
  carmen_test_ipc_exit(err, "Could not define", CARMEN_ROBOT_VECTOR_STATUS_NAME);

  /* setup incoming message handlers */
  err = IPC_subscribe(CARMEN_ROBOT_VELOCITY_NAME, velocity_handler, NULL);
  carmen_test_ipc_exit(err, "Could not subscribe", CARMEN_ROBOT_VELOCITY_NAME);
  IPC_setMsgQueueLength(CARMEN_ROBOT_VELOCITY_NAME, 1);

  err = IPC_subscribe(CARMEN_ROBOT_VECTOR_MOVE_NAME, vector_move_handler,
  NULL);
  carmen_test_ipc_exit(err, "Could not subscribe", CARMEN_ROBOT_VECTOR_MOVE_NAME);
  IPC_setMsgQueueLength(CARMEN_ROBOT_VECTOR_MOVE_NAME, 1);

  err = IPC_subscribe(CARMEN_ROBOT_FOLLOW_TRAJECTORY_NAME, follow_trajectory_handler, NULL);

  carmen_test_ipc_exit(err, "Could not subscribe", CARMEN_ROBOT_FOLLOW_TRAJECTORY_NAME);
  IPC_setMsgQueueLength(CARMEN_ROBOT_FOLLOW_TRAJECTORY_NAME, 1);

  return 0;
}

void carmen_robot_shutdown( int x __attribute__ ((unused)) ) {
  carmen_robot_stop_robot(CARMEN_ROBOT_ALL_STOP);
}

void carmen_robot_usage( char *progname, char *fmt, ... ) {
  va_list args;

  if( fmt != NULL ) {
    fprintf(stderr, "\n[31;1m");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "[0m\n\n");
  } else {
    fprintf(stderr, "\n");
  }

  if( strrchr(progname, '/') != NULL ) {
    progname = strrchr(progname, '/');
    progname++;
  }

  fprintf(stderr, "Usage: %s <args> \n", progname);
  fprintf(stderr, "\t-sonar {on|off}   - turn sonar use on and off (unsupported).\n");
  exit(-1);
}

static int read_robot_parameters( int argc, char **argv ) {
  int num_items;

  carmen_param_t param_list[] = {
      { "robot", "max_t_vel", CARMEN_PARAM_DOUBLE, &carmen_robot_config.max_t_vel, 1, NULL }, {
          "robot", "max_r_vel", CARMEN_PARAM_DOUBLE, &carmen_robot_config.max_r_vel, 1, NULL },
      { "robot", "min_approach_dist", CARMEN_PARAM_DOUBLE, &carmen_robot_config.approach_dist, 1, NULL }, {
          "robot", "min_side_dist", CARMEN_PARAM_DOUBLE, &carmen_robot_config.side_dist, 1, NULL },
      { "robot", "length", CARMEN_PARAM_DOUBLE, &carmen_robot_config.length, 0, NULL }, { "robot", "width",
      CARMEN_PARAM_DOUBLE,
                                                                                          &carmen_robot_config.width, 0,
                                                                                          NULL },
      { "robot", "acceleration", CARMEN_PARAM_DOUBLE, &carmen_robot_config.acceleration, 1, NULL }, {
          "robot", "deceleration", CARMEN_PARAM_DOUBLE, &carmen_robot_config.deceleration, 1, NULL },
      { "robot", "reaction_time", CARMEN_PARAM_DOUBLE, &carmen_robot_config.reaction_time, 0, NULL }, {
          "robot", "theta_gain", CARMEN_PARAM_DOUBLE, &theta_gain, 1, NULL },
      { "robot", "theta_d_gain", CARMEN_PARAM_DOUBLE, &theta_d_gain, 1, NULL }, { "robot", "displacement_gain",
      CARMEN_PARAM_DOUBLE,
                                                                                  &disp_gain, 1,
                                                                                  NULL },

      /* 3d controller isn't ready for main distribution yet - Jared Glover
       *
       * {"robot", "use_3d_control", CARMEN_PARAM_ONOFF, &use_3d_control, 1, NULL},
       * {"robot", "control_lookahead", CARMEN_PARAM_DOUBLE, &control_lookahead, 1, NULL},
       * {"robot", "control_lookahead_approach_dist", CARMEN_PARAM_DOUBLE,
       * &control_lookahead_approach_dist, 1, NULL},
       */

      { "robot", "allow_rear_motion", CARMEN_PARAM_ONOFF, &carmen_robot_config.allow_rear_motion, 1, NULL },
#ifndef COMPILE_WITHOUT_LASER_SUPPORT
      { "robot", "use_laser", CARMEN_PARAM_ONOFF, &use_laser, 1, NULL },
#endif
      { "robot", "use_sonar", CARMEN_PARAM_ONOFF, &use_sonar, 1, NULL },
      { "robot", "sensor_timeout", CARMEN_PARAM_DOUBLE, &robot_sensor_timeout, 1, NULL }, { "robot",
                                                                                            "collision_avoidance",
                                                                                            CARMEN_PARAM_ONOFF,
                                                                                            &collision_avoidance,
                                                                                            1,
                                                                                            NULL },
      { "robot", "collision_avoidance_frequency", CARMEN_PARAM_DOUBLE, &carmen_robot_collision_avoidance_frequency, 1,
      NULL },
      { "robot", "turn_before_driving_if_heading_bigger_than",
      CARMEN_PARAM_DOUBLE,
        &turn_before_driving_if_heading_bigger_than, 1, NULL },
      { "robot", "interpolate_odometry",
      CARMEN_PARAM_ONOFF,
        &carmen_robot_config.interpolate_odometry, 1, NULL } };

  num_items = sizeof (param_list) / sizeof (param_list[0]);
  carmen_param_install_params(argc, argv, param_list, num_items);

  if( use_sonar )
    carmen_robot_add_sonar_parameters(argv[0]);
  if( use_bumper )
    carmen_robot_add_bumper_parameters(argv[0]);
#ifndef COMPILE_WITHOUT_LASER_SUPPORT
  if( use_laser )
    carmen_robot_add_laser_parameters(argc, argv);
#endif

  // that sucks!
  // carmen_robot_config.acceleration = 0.5;

  printf("debug: max_r_vel=%f\n", carmen_robot_config.max_t_vel);
  return 0;
}

int carmen_robot_start( int argc, char **argv ) {
  robot_host = carmen_get_host();

  carmen_running_average_clear(CARMEN_ROBOT_ODOMETRY_AVERAGE);

  if( read_robot_parameters(argc, argv) < 0 )
    return -1;

  if( initialize_robot_ipc() < 0 ) {
    carmen_warn("Error: could not connect to IPC Server\n");
    return -1;;
  }

  carmen_base_subscribe_odometry_message(&carmen_robot_latest_odometry, (carmen_handler_t)base_odometry_handler,
                                         CARMEN_SUBSCRIBE_LATEST);

#ifndef COMPILE_WITHOUT_LASER_SUPPORT
  if( use_laser )
    carmen_robot_add_laser_handlers();
#endif
  if( use_sonar )
    carmen_robot_add_sonar_handler();
  if( use_bumper )
    carmen_robot_add_bumper_handler();

  return 0;
}

int carmen_robot_run( void ) {
  if( carmen_robot_sensor_time_of_last_update >= 0 && carmen_get_time() - carmen_robot_sensor_time_of_last_update
                                                      > robot_sensor_timeout ) {
    carmen_warn("Sensor timed out. Stopping robot.\n");
    carmen_robot_stop_robot(CARMEN_ROBOT_ALL_STOP);
  } else if( following_vector )
    follow_vector();
  else if( following_trajectory )
    follow_trajectory_3d();

  carmen_publish_heartbeat("robot");

  return 1;
}
