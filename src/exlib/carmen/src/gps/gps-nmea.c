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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <carmen/carmen.h>
#include <carmen/gps_nmea_interface.h>

#include "gps.h"
#include "gps-io.h"
 
int
gps_parse_gga( char * line, int num_chars )
{
  char * ptr;
  int    i;
  for (i=1; i<num_chars-1; i++) {
    if (line[i]=='$' || line[i]=='*')
      return(FALSE);
  }
  if (num_chars>0 && carmen_extern_gpgga_ptr!=NULL) {

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->utc = atof(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->latitude = atof(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->lat_orient = ptr[0];

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->longitude = atof(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->long_orient = ptr[0];

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->gps_quality = atoi(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->num_satellites = atoi(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->hdop = atof(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->sea_level = atof(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->altitude = atof(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->geo_sea_level = atof(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->geo_sep = atof(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gpgga_ptr->data_age = atoi(ptr);

    return(TRUE);
  }
  return(FALSE);
}

int
gps_parse_rmc( char * line, int num_chars )
{
  char * ptr;
  int    i;
  for (i=1; i<num_chars-1; i++) {
    if (line[i]=='$' || line[i]=='*')
      return(FALSE);
  }
  if (num_chars>0 && carmen_extern_gpgga_ptr!=NULL) {

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gprmc_ptr->utc = atof(ptr);
 
    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gprmc_ptr->validity = (ptr[0] == 'A')?1:0;

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gprmc_ptr->latitude = atof(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gprmc_ptr->lat_orient = ptr[0];

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gprmc_ptr->longitude = atof(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gprmc_ptr->long_orient = ptr[0];

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gprmc_ptr->speed = carmen_knots_to_meters_per_second( atof(ptr) );

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gprmc_ptr->true_course = carmen_degrees_to_radians( atoi(ptr) );

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gprmc_ptr->date = atoi(ptr);

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gprmc_ptr->variation = carmen_degrees_to_radians( atoi(ptr) );

    ptr = strsep( &line, ",");
    if (ptr==NULL) return(FALSE);
    carmen_extern_gprmc_ptr->var_dir =ptr[0];

    return(TRUE);
  }
  return(FALSE);
}

int
carmen_gps_parse_data( char * line, int num_chars )
{
#ifdef GPGGA_PARSE_DEBUG
  int  i;
#endif
#ifdef PARSE_DEBUG
  int  j;
#endif
  if (num_chars>=6) {
    if (!strncmp( "$GPGGA", line, 6 ) ) {
#ifdef PARSE_DEBUG
      fprintf( stderr, "(GPGGA)" );
#endif
#ifdef GPGGA_PARSE_DEBUG
      fprintf( stderr, "(" );
      for (i=0;i<num_chars;i++)
	fprintf( stderr, "%c", line[i] );
      fprintf( stderr, ")" );
#endif
      return(gps_parse_gga( line, num_chars ));
    } else if (!strncmp( "$GPGLL", line, 6 ) ) {
#ifdef PARSE_DEBUG
      fprintf( stderr, "(GPGLL)" );
#endif
    } else if (!strncmp( "$GPRMC", line, 6 ) ) {
      return(gps_parse_rmc( line, num_chars ));
#ifdef PARSE_DEBUG
      fprintf( stderr, "(GPRMC)" );
#endif
      
    } else if (!strncmp( "$GPGSV", line, 6 ) ) {
#ifdef PARSE_DEBUG
      fprintf( stderr, "(GPGSV)" );
#endif
    } else if (!strncmp( "$GPGSA", line, 6 ) ) {
#ifdef PARSE_DEBUG
      fprintf( stderr, "(GPGSA)" );
#endif 
} else {
#ifdef PARSE_DEBUG
      fprintf( stderr, "[" );
      j=1;
      while (j<num_chars  && line[j]!=',' && line[j]!='*') {
	fprintf( stderr, "%c", line[j] );
	j++;
      }
      fprintf( stderr, "]" );
#endif
    }
  }
  return(FALSE);
}

