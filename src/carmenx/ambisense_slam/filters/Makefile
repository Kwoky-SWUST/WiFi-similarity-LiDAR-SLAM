CARMEN_HOME          = ../../../exlib/carmen
include $(CARMEN_HOME)/src/Makefile.conf
SCIROCO_BASE_DIR = ../../../../
include $(SCIROCO_BASE_DIR)/src/carmenx/Makefile.carmenx

SUBDIRS        =
MODULE_NAME    = "AMBISENSE SLAM FILTERS"
MODULE_COMMENT = "Filters for the AmbiSense SLAM package"


CXXFLAGS += -g3
CFLAGS   += -g3 -UHAVE_LAPACK
LFLAGS   += -lstdc++ -lglobal -lGeographic -lipc -L$(SCIROCO_BASE_DIR)/lib  -lutils -lcarmen_utils_geometry -lnewmat -lambisense_slam_filters -lxstream -lbz2\
-L/opt/lampp/lib/ -lmysqlclient\
  -L$(SCIROCO_BASE_DIR)src/exlib/gzstream/ -lgzstream\
  -L$(SCIROCO_BASE_DIR)src/exlib/libdistance-0.2.2/ -ldistance
  
IFLAGS += -I$(QTDIR)/include -I../ -I/usr/include/mysql/ -I/opt/lampp/include/

IFLAGS += -I$(SCIROCO_BASE_DIR)/src/exlib -I$(SCIROCO_BASE_DIR)/src/exlib/xstream/include

SOURCES = particle_filter_2.cpp particle_swarm.cpp pf2_6d.cpp pf2_history_6d.cpp pf2_xytheta.cpp benchmark_particle_filters.cpp

PUBLIC_INCLUDES     = particle_filter_2.h particle_swarm.h
PUBLIC_LIBRARIES    = libambisense_slam_filters.a
PUBLIC_BINARIES     =
PUBLIC_LIBRARIES_SO = libambisense_slam_filters.so

TARGETS = libambisense_slam_filters.a
#benchmark_particle_filters fusion_Ap_FM fusion_Ap_FM_server computeCDF online_fusion locator_query query store store_second theoretical_accuracy offline_fusion_bluetooth_lr
#TARGETS = libambisense_slam_filters.a fusion_Ap_FM_server

libambisense_slam_filters.a:   pf2_6d.o

include $(CARMEN_HOME)/src/Makefile.rules
