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
#include <carmen/carmen_stdio.h>
#include <carmen/readlog.h>

long int carmen_logfile_uncompressed_length(carmen_FILE *infile)
{
  unsigned char buffer[10000];
  long int log_bytes = 0;
  int nread;
  struct stat stat_buf;

  if(!infile->compressed) {
    /* compute total length of logfile */
    carmen_fseek(infile, 0L, SEEK_SET);
    log_bytes = 0;
    do {
      nread = carmen_fread(buffer, 1, 10000, infile);
      log_bytes += nread;
    } while(nread > 0);
    carmen_fseek(infile, 0L, SEEK_SET);
    return log_bytes;
  }
  else {
    /* report compressed size for compressed files */
    fstat(fileno(infile->fp), &stat_buf);
    return stat_buf.st_size;
  }
}

/**
 * Builds the index structure used for parsing a carmen log file.
 **/
carmen_logfile_index_p carmen_logfile_index_messages(carmen_FILE *infile)
{
  carmen_logfile_index_p index;
  int i, found_linebreak = 1, nread, total_bytes, max_messages, read_count = 0;
  int file_length = 0, file_position = 0;
  unsigned char buffer[10000];

  /* allocate and initialize an index */
  index = (carmen_logfile_index_p)calloc(1, sizeof(carmen_logfile_index_t));
  carmen_test_alloc(index);

  /* compute the total length of the uncompressed logfile. */
  fprintf(stderr, "\n\rIndexing messages (0%%)    ");
  file_length = carmen_logfile_uncompressed_length(infile);

  /* mark the start of all messages */
  index->num_messages = 0;
  max_messages = 10000;
  index->offset = (long int *)calloc(max_messages, sizeof(long int));
  carmen_test_alloc(index->offset);

  carmen_fseek(infile, 0L, SEEK_SET);

  total_bytes = 0;
  do {
    nread = carmen_fread(buffer, 1, 10000, infile);
    read_count++;
    if(read_count % 1000 == 0) {
      if(!infile->compressed)
	file_position = total_bytes + nread;
      else
	file_position = lseek(fileno(infile->fp), 0, SEEK_CUR);
      fprintf(stderr, "\rIndexing messages (%.0f%%)      ",
	      ((float)file_position) / file_length * 100.0);
    }

    if(nread > 0) {
      for(i = 0; i < nread; i++) {
        if(found_linebreak && buffer[i] != '\r') {
          found_linebreak = 0;
	  if(index->num_messages == max_messages) {
	    max_messages += 10000;
	    index->offset = (long int *)realloc(index->offset, max_messages *
						sizeof(long int));
	    carmen_test_alloc(index->offset);
	  }
	  index->offset[index->num_messages] = total_bytes + i;
	  index->num_messages++;
        }
        if(buffer[i] == '\n')
          found_linebreak = 1;
      }
      total_bytes += nread;
    }
  } while(nread > 0);
  fprintf(stderr, "\rIndexing messages (100%%) - %d messages found.      \n",
	  index->num_messages);
  carmen_fseek(infile, 0L, SEEK_SET);
  index->current_position = 0;
  return index;
}

void carmen_logfile_free_index(carmen_logfile_index_p* pindex) {
  if (pindex == NULL)
    return;

  if ( (*pindex) == NULL)
    return;

  if ( (*pindex)->offset != NULL) {
    free( (*pindex)->offset);
  }
  free(*pindex);
  (*pindex) = NULL;
}

int carmen_logfile_eof(carmen_logfile_index_p index)
{
  if(index->current_position > index->num_messages - 1)
    return 1;
  else
    return 0;
}

float carmen_logfile_percent_read(carmen_logfile_index_p index)
{
  return index->current_position / (float)index->num_messages;
}

int carmen_logfile_read_line(carmen_logfile_index_p index, carmen_FILE *infile,
			     int message_num, int max_line_length, char *line)
{
  int nread;

  /* are we moving sequentially through the logfile?  If not, fseek */
  if(message_num != index->current_position) {
    index->current_position = message_num;
    carmen_fseek(infile, index->offset[index->current_position], SEEK_SET);
  }

  /* check maximum line length */
  if(index->offset[index->current_position + 1] - index->offset[index->current_position] >= max_line_length)
    carmen_die("Error: %d exceed maximum line length (%d).\n", index->offset[index->current_position + 1] - index->offset[index->current_position], max_line_length);

  /* read the line of the logfile */
  nread = carmen_fread(line, 1, index->offset[index->current_position + 1] -
		       index->offset[index->current_position], infile);
  line[nread] = '\0';
  index->current_position++;
  return nread;
}

int carmen_logfile_read_next_line(carmen_logfile_index_p index, carmen_FILE *infile,
				  int max_line_length, char *line)
{
  return carmen_logfile_read_line(index, infile,
				  index->current_position,
				  max_line_length, line);
}


int first_wordlength(char *str)
{
  //  return strchr(str, ' ') - str;
  char* x = carmen_next_word(str);
  return (int) (x-str);

}

void copy_host_string(char **host, char **string)
{
  int l;

  while(*string[0] == ' ')
    *string += 1;                           /* advance past spaces */
  l = first_wordlength(*string) + 1;
  if(*host != NULL)
    free(*host);
  *host = (char *)calloc(1, l);
  carmen_test_alloc(*host);
  strncpy(*host, *string, l - 1);
  (*host)[l] = '\0';
  *string += l;
}

#define CLF_READ_DOUBLE(str) strtod(*(str), (str))
#define CLF_READ_INT(str) (int)strtol(*(str), (str), 10)
#define CLF_READ_CHAR(str) (char) ( ( (*str)++)[0] )

char *carmen_string_to_base_odometry_message(char *string,
					     carmen_base_odometry_message
					     *odometry)
{
  char *current_pos = string;

  if (strncmp(current_pos, "ODOM", 4) == 0)
    current_pos = carmen_next_word(current_pos);

  odometry->x = CLF_READ_DOUBLE(&current_pos);
  odometry->y = CLF_READ_DOUBLE(&current_pos);
  odometry->theta = CLF_READ_DOUBLE(&current_pos);
  odometry->tv = CLF_READ_DOUBLE(&current_pos);
  odometry->rv = CLF_READ_DOUBLE(&current_pos);
  odometry->acceleration = CLF_READ_DOUBLE(&current_pos);
  odometry->timestamp = CLF_READ_DOUBLE(&current_pos);
  copy_host_string(&odometry->host, &current_pos);
  return current_pos;
}

char *carmen_string_to_arm_state_message(char *string,
					 carmen_arm_state_message *arm)
{
  int i;
  char *current_pos = string;

  if (strncmp(current_pos, "ARM", 3) == 0)
    current_pos = carmen_next_word(current_pos);

  arm->flags = CLF_READ_INT(&current_pos);
  arm->num_joints = CLF_READ_INT(&current_pos);
  arm->joint_angles = (double *) realloc(arm->joint_angles, arm->num_joints*sizeof(double));
  carmen_test_alloc(arm->joint_angles);
  for (i = 0; i < arm->num_joints; i++)
    arm->joint_angles[i] = CLF_READ_DOUBLE(&current_pos);
  arm->num_currents = CLF_READ_INT(&current_pos);
  arm->joint_currents = (double *) realloc(arm->joint_currents, arm->num_currents*sizeof(double));
  carmen_test_alloc(arm->joint_currents);
  for (i = 0; i < arm->num_currents; i++)
    arm->joint_currents[i] = CLF_READ_DOUBLE(&current_pos);
  arm->num_vels = CLF_READ_INT(&current_pos);
  arm->joint_angular_vels = (double *) realloc(arm->joint_angular_vels,
					       arm->num_vels*sizeof(double));
  carmen_test_alloc(arm->joint_angular_vels);
  for (i = 0; i < arm->num_vels; i++)
    arm->joint_angular_vels[i] = CLF_READ_DOUBLE(&current_pos);
  arm->gripper_closed = CLF_READ_INT(&current_pos);

  arm->timestamp = CLF_READ_DOUBLE(&current_pos);
  copy_host_string(&arm->host, &current_pos);
  return current_pos;
}

char *carmen_string_to_simulator_truepos_message(char *string,
						 carmen_simulator_truepos_message *truepos)
{
  char *current_pos = string;

  if (strncmp(current_pos, "TRUEPOS", 7) == 0)
    current_pos = carmen_next_word(current_pos);


  truepos->truepose.x = CLF_READ_DOUBLE(&current_pos);
  truepos->truepose.y = CLF_READ_DOUBLE(&current_pos);
  truepos->truepose.theta = CLF_READ_DOUBLE(&current_pos);
  truepos->odometrypose.x = CLF_READ_DOUBLE(&current_pos);
  truepos->odometrypose.y = CLF_READ_DOUBLE(&current_pos);
  truepos->odometrypose.theta = CLF_READ_DOUBLE(&current_pos);
  truepos->timestamp = CLF_READ_DOUBLE(&current_pos);
  copy_host_string(&truepos->host, &current_pos);
  return current_pos;
}

double carmen_laser_guess_fov(int num_beams)
{
  if (num_beams == 181)
    return M_PI;                  /* 180 degrees */
  else if (num_beams == 180)
    return M_PI / 180.0 * 179.0;  /* 179 degrees (last beam ignored)*/
  else if (num_beams == 361)
    return M_PI;                  /* 180 degrees */
  else if (num_beams == 360)
    return M_PI / 180.0 * 179.5 ; /* 179.5 degrees (last beam ignored)*/
  else if (num_beams == 401)
    return M_PI / 180.0 * 100.0 ; /* 100.0 degrees */
  else if (num_beams == 400)
    return M_PI / 100.0 * 99.75 ; /* 99.75 degrees (last beam ignored)*/
  else
    return M_PI;                  /* assume 180 degrees */
}

double carmen_laser_guess_angle_increment(int num_beams)
{
  if (num_beams == 181 || num_beams == 180)
    return M_PI / 180.0; /* 1 degree = M_PI/180 */
  else if (num_beams == 361 || num_beams == 360)
    return M_PI / 360.0;  /* 0.5 degrees = M_PI/360 */
  else if (num_beams == 401 || num_beams == 400)
    return M_PI / 720.0;  /* 0.25 degrees = M_PI/720 */
  else
    return carmen_laser_guess_fov(num_beams) /
      ((double) (num_beams-1));
}

char *carmen_string_to_laser_laser_message_orig(char *string,
						carmen_laser_laser_message
						*laser)
{
  char *current_pos = string;
  int i, num_readings;

  if (strncmp(current_pos, "LASER", 5) == 0)
    current_pos = carmen_next_word(current_pos);


  num_readings = CLF_READ_INT(&current_pos);
  if(laser->num_readings != num_readings) {
    laser->num_readings = num_readings;
    laser->range = (float *)realloc(laser->range, laser->num_readings *
				    sizeof(float));
    carmen_test_alloc(laser->range);
  }
  for(i = 0; i < laser->num_readings; i++)
    laser->range[i] = CLF_READ_DOUBLE(&current_pos);
  laser->timestamp = CLF_READ_DOUBLE(&current_pos);
  copy_host_string(&laser->host, &current_pos);

  /* fill in remission with nothing */
  laser->num_remissions = 0;
  laser->remission = NULL;

  /* guess at fields */
  laser->config.laser_type = SICK_LMS;
  laser->config.fov = carmen_laser_guess_fov(laser->num_readings);
  laser->config.start_angle = -M_PI / 2.0;
  laser->config.angular_resolution =
    carmen_laser_guess_angle_increment(laser->num_readings);
  laser->config.maximum_range = 80.0;
  laser->config.accuracy = 0.01;
  laser->config.remission_mode = 0;

  return current_pos;
}

char *carmen_string_to_robot_laser_message_orig(char *string,
						carmen_robot_laser_message
						*laser)
{
  char *current_pos = string;
  int i, num_readings;

  if (strncmp(current_pos, "FLASER", 6) == 0 ||
      strncmp(current_pos, "RLASER", 6) == 0)
    current_pos = carmen_next_word(current_pos);

  num_readings = CLF_READ_INT(&current_pos);
  if(laser->num_readings != num_readings) {
    laser->num_readings = num_readings;
    laser->range = (float *)realloc(laser->range, laser->num_readings *
				    sizeof(float));
    carmen_test_alloc(laser->range);
    laser->tooclose = (char *)realloc(laser->tooclose, laser->num_readings);
    carmen_test_alloc(laser->tooclose);
  }
  for(i = 0; i < laser->num_readings; i++)
    laser->range[i] = CLF_READ_DOUBLE(&current_pos);

  laser->laser_pose.x = CLF_READ_DOUBLE(&current_pos);
  laser->laser_pose.y = CLF_READ_DOUBLE(&current_pos);
  laser->laser_pose.theta = CLF_READ_DOUBLE(&current_pos);
  laser->robot_pose.x = CLF_READ_DOUBLE(&current_pos);
  laser->robot_pose.y = CLF_READ_DOUBLE(&current_pos);
  laser->robot_pose.theta = CLF_READ_DOUBLE(&current_pos);
  laser->timestamp = CLF_READ_DOUBLE(&current_pos);
  copy_host_string(&laser->host, &current_pos);

  /* fill in remission with nothing */
  laser->num_remissions = 0;
  laser->remission = NULL;

  /* guess at fields */
  laser->config.laser_type = SICK_LMS;
  laser->config.fov = carmen_laser_guess_fov(laser->num_readings);
  laser->config.start_angle = -M_PI / 2.0;
  laser->config.angular_resolution =
    carmen_laser_guess_angle_increment(laser->num_readings);
  laser->config.maximum_range = 80.0;
  laser->config.accuracy = 0.01;
  laser->config.remission_mode = 0;

  return current_pos;
}

char *carmen_string_to_laser_laser_message(char *string,
					   carmen_laser_laser_message *laser)
{
  char *current_pos = string;
  int i, num_readings, num_remissions;

  if (strncmp(current_pos, "RAWLASER", 6) == 0)
    current_pos = carmen_next_word(current_pos);

  laser->config.laser_type = CLF_READ_INT(&current_pos);
  laser->config.start_angle = CLF_READ_DOUBLE(&current_pos);
  laser->config.fov = CLF_READ_DOUBLE(&current_pos);
  laser->config.angular_resolution = CLF_READ_DOUBLE(&current_pos);
  laser->config.maximum_range = CLF_READ_DOUBLE(&current_pos);
  laser->config.accuracy = CLF_READ_DOUBLE(&current_pos);
  laser->config.remission_mode = CLF_READ_INT(&current_pos);

  num_readings = CLF_READ_INT(&current_pos);
  if(laser->num_readings != num_readings) {
    laser->num_readings = num_readings;
    laser->range = (float *)realloc(laser->range, laser->num_readings *
				    sizeof(float));
    carmen_test_alloc(laser->range);
  }
  for(i = 0; i < laser->num_readings; i++)
    laser->range[i] = CLF_READ_DOUBLE(&current_pos);

  num_remissions = CLF_READ_INT(&current_pos);
  if(laser->num_remissions != num_remissions) {
    laser->num_remissions = num_remissions;
    laser->remission = (float *)realloc(laser->remission,
					laser->num_remissions * sizeof(float));
    carmen_test_alloc(laser->remission);
  }
  for(i = 0; i < laser->num_remissions; i++)
    laser->remission[i] = CLF_READ_DOUBLE(&current_pos);

  laser->timestamp = CLF_READ_DOUBLE(&current_pos);
  copy_host_string(&laser->host, &current_pos);

  return current_pos;
}

char *carmen_string_to_robot_laser_message(char *string,
					   carmen_robot_laser_message *laser)
{
  char *current_pos = string;
  int i, num_readings, num_remissions;

  if (strncmp(current_pos, "ROBOTLASER", 10) == 0)
    current_pos = carmen_next_word(current_pos);

  laser->config.laser_type = CLF_READ_INT(&current_pos);
  laser->config.start_angle = CLF_READ_DOUBLE(&current_pos);
  laser->config.fov = CLF_READ_DOUBLE(&current_pos);
  laser->config.angular_resolution = CLF_READ_DOUBLE(&current_pos);
  laser->config.maximum_range = CLF_READ_DOUBLE(&current_pos);
  laser->config.accuracy = CLF_READ_DOUBLE(&current_pos);
  laser->config.remission_mode = CLF_READ_INT(&current_pos);

  num_readings = CLF_READ_INT(&current_pos);
  if(laser->num_readings != num_readings) {
    laser->num_readings = num_readings;
    laser->range = (float *)realloc(laser->range, laser->num_readings *
				    sizeof(float));
    carmen_test_alloc(laser->range);
  }
  for(i = 0; i < laser->num_readings; i++)
    laser->range[i] = CLF_READ_DOUBLE(&current_pos);

  num_remissions = CLF_READ_INT(&current_pos);
  if(laser->num_remissions != num_remissions) {
    laser->num_remissions = num_remissions;
    laser->remission = (float *)realloc(laser->remission,
					laser->num_remissions * sizeof(float));
    carmen_test_alloc(laser->remission);
  }
  for(i = 0; i < laser->num_remissions; i++)
    laser->remission[i] = CLF_READ_DOUBLE(&current_pos);

  laser->laser_pose.x = CLF_READ_DOUBLE(&current_pos);
  laser->laser_pose.y = CLF_READ_DOUBLE(&current_pos);
  laser->laser_pose.theta = CLF_READ_DOUBLE(&current_pos);
  laser->robot_pose.x = CLF_READ_DOUBLE(&current_pos);
  laser->robot_pose.y = CLF_READ_DOUBLE(&current_pos);
  laser->robot_pose.theta = CLF_READ_DOUBLE(&current_pos);

  laser->tv = CLF_READ_DOUBLE(&current_pos);
  laser->rv = CLF_READ_DOUBLE(&current_pos);
  laser->forward_safety_dist = CLF_READ_DOUBLE(&current_pos);
  laser->side_safety_dist = CLF_READ_DOUBLE(&current_pos);
  laser->turn_axis = CLF_READ_DOUBLE(&current_pos);

  laser->timestamp = CLF_READ_DOUBLE(&current_pos);
  copy_host_string(&laser->host, &current_pos);

  return current_pos;
}



char *carmen_string_to_gps_gpgga_message(char *string,
				       carmen_gps_gpgga_message *gps_msg)
{
  char *current_pos = string;

  if (strncmp(current_pos, "NMEAGGA", 7) == 0)
    current_pos = carmen_next_word(current_pos);

  gps_msg->nr               = CLF_READ_INT(&current_pos);
  gps_msg->utc              = CLF_READ_DOUBLE(&current_pos);
  gps_msg->latitude         = CLF_READ_DOUBLE(&current_pos);
  current_pos = carmen_next_word(current_pos);
  gps_msg->lat_orient       = CLF_READ_CHAR(&current_pos);
  gps_msg->longitude        = CLF_READ_DOUBLE(&current_pos);
  current_pos = carmen_next_word(current_pos);
  gps_msg->long_orient      = CLF_READ_CHAR(&current_pos);
  gps_msg->gps_quality      = CLF_READ_INT(&current_pos);
  gps_msg->num_satellites   = CLF_READ_INT(&current_pos);
  gps_msg->hdop             = CLF_READ_DOUBLE(&current_pos);
  gps_msg->sea_level        = CLF_READ_DOUBLE(&current_pos);
  gps_msg->altitude         = CLF_READ_DOUBLE(&current_pos);
  gps_msg->geo_sea_level    = CLF_READ_DOUBLE(&current_pos);
  gps_msg->geo_sep          = CLF_READ_DOUBLE(&current_pos);
  gps_msg->data_age         = CLF_READ_INT(&current_pos);
  gps_msg->timestamp        = CLF_READ_DOUBLE(&current_pos);
  copy_host_string(&gps_msg->host, &current_pos);

  return current_pos;
}

char *carmen_string_to_gps_gprmc_message(char *string,
					 carmen_gps_gprmc_message *gps_msg)
{
  char *current_pos = string;

  if (strncmp(current_pos, "NMEARMC", 7) == 0)
    current_pos = carmen_next_word(current_pos);

  gps_msg->nr               = CLF_READ_INT(&current_pos);
  gps_msg->validity         = CLF_READ_INT(&current_pos);
  gps_msg->utc              = CLF_READ_DOUBLE(&current_pos);
  gps_msg->latitude         = CLF_READ_DOUBLE(&current_pos);
  gps_msg->lat_orient       = CLF_READ_CHAR(&current_pos);
  gps_msg->longitude        = CLF_READ_DOUBLE(&current_pos);
  current_pos = carmen_next_word(current_pos);
  gps_msg->long_orient      = CLF_READ_CHAR(&current_pos);
  gps_msg->speed            = CLF_READ_INT(&current_pos);
  gps_msg->true_course      = CLF_READ_INT(&current_pos);
  gps_msg->variation        = CLF_READ_DOUBLE(&current_pos);
  current_pos = carmen_next_word(current_pos);
  gps_msg->var_dir          = CLF_READ_CHAR(&current_pos);
  gps_msg->date             = CLF_READ_INT(&current_pos);
  gps_msg->timestamp        = CLF_READ_DOUBLE(&current_pos);
  copy_host_string(&gps_msg->host, &current_pos);

  return current_pos;
}
