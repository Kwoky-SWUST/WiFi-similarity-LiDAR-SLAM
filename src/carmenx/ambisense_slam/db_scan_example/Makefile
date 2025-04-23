CARMEN_HOME          = ../../../exlib/carmen
include $(CARMEN_HOME)/src/Makefile.conf
SCIROCO_BASE_DIR = ../../../../
include $(SCIROCO_BASE_DIR)/src/carmenx/Makefile.carmenx

SUBDIRS        =
MODULE_NAME    = "DB scan example"
MODULE_COMMENT = "DB scan"


CXXFLAGS += -g3
CFLAGS   += -g3 -UHAVE_LAPACK
LFLAGS   += -lstdc++ -lglobal -lGeographic -lipc -L$(SCIROCO_BASE_DIR)/lib  -lutils -lcarmen_utils_geometry -lrfid_sensor_model -lambisense_slam_filters -lnewmat -lxstream -lbz2\
-L/opt/lampp/lib/ -lmysqlclient\
  -L$(SCIROCO_BASE_DIR)src/exlib/gzstream/ -lgzstream\
-L$(SCIROCO_BASE_DIR)/src/carmenx/ambisense_slam/laser_segmentation/ -lobstacle_extractor -larmadillo -lboost_system\
-L$(SCIROCO_BASE_DIR)/src/carmenx/ambisense_slam/db_scan/ -ldbscan
  
IFLAGS += -I$(QTDIR)/include -I../ -I/usr/include/mysql/ -I/opt/lampp/include/ -I$(SCIROCO_BASE_DIR)/src/carmenx/ambisense_slam/db_scan/

IFLAGS += -I$(SCIROCO_BASE_DIR)/src/exlib -I$(SCIROCO_BASE_DIR)/src/exlib/xstream/include

SOURCES = test_dbscan.cpp

PUBLIC_INCLUDES     = 
PUBLIC_LIBRARIES    = 
PUBLIC_BINARIES     =
PUBLIC_LIBRARIES_SO = 

TARGETS = test_dbscan


test_dbscan:   test_dbscan.o

include $(CARMEN_HOME)/src/Makefile.rules
