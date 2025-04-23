CARMEN_HOME          = ../../../exlib/carmen
include $(CARMEN_HOME)/src/Makefile.conf
SCIROCO_BASE_DIR = ../../../../
include $(SCIROCO_BASE_DIR)/src/carmenx/Makefile.carmenx

SUBDIRS        =
MODULE_NAME    = "Uwb_Positioning"
MODULE_COMMENT = "Uwb_Positioning generation"


CXXFLAGS += -g3
CFLAGS   += -g3 
LFLAGS   += -lstdc++ -lglobal -lGeographic -lipc -L$(SCIROCO_BASE_DIR)/lib  -lutils -lcarmen_utils_geometry -lrfid_sensor_model -lambisense_slam_filters -lnewmat -lxstream -lbz2\
-L/opt/lampp/lib/ -lmysqlclient\
  -L$(SCIROCO_BASE_DIR)src/exlib/gzstream/ -lgzstream\
-L$(SCIROCO_BASE_DIR)/src/carmenx/ambisense_slam/laser_segmentation/ -lobstacle_extractor -larmadillo -lboost_system
  
IFLAGS += -I$(QTDIR)/include -I../ -I/usr/include/mysql/ -I/opt/lampp/include/ -I$(SCIROCO_BASE_DIR)/src/carmenx/ambisense_slam/laser_segmentation/

IFLAGS += -I$(SCIROCO_BASE_DIR)/src/exlib -I$(SCIROCO_BASE_DIR)/src/exlib/xstream/include


SOURCES = hal.cpp hal_fprint.cpp lmh.cpp lmh_uartrx.cpp hal_uart.cpp dwm_api.cpp


PUBLIC_INCLUDES     = hal.h hal_fprint.h lmh.h lmh_uartrx.h hal_uart.h dwm_api.h
PUBLIC_LIBRARIES    = libdwm1001.a
PUBLIC_BINARIES     = 
PUBLIC_LIBRARIES_SO = libdwm1001.so

TARGETS = libdwm1001.a

libdwm1001.a:hal_fprint.o lmh_uartrx.o hal_uart.o hal.o lmh.o dwm_api.o
include $(CARMEN_HOME)/src/Makefile.rules
