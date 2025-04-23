CARMEN_HOME          = ../../../exlib/carmen
include $(CARMEN_HOME)/src/Makefile.conf
SCIROCO_BASE_DIR = ../../../..
include $(SCIROCO_BASE_DIR)/src/carmenx/Makefile.carmenx

SUBDIRS        =
MODULE_NAME    = "Wifi SLAM"
MODULE_COMMENT = "Wifi SLAM"


CXXFLAGS += -g3 -Ofast -std=c++11
CFLAGS   += -g3 -UHAVE_LAPACK 
LFLAGS   += -lstdc++ -lglobal -lGeographic -lipc -L$(SCIROCO_BASE_DIR)/lib  -lutils -lcarmen_utils_geometry -lnewmat -lxstream -lbz2\
-L/opt/lampp/lib/ -lmysqlclient\
-L$(SCIROCO_BASE_DIR)/src/exlib/gzstream/ -lgzstream -lcsparse -lcxsparse\
-L/usr/lib/x86_64-linux-gnu/ -lboost_system\
-L/usr/local/lib/ -lg2o_parser -lg2o_solver_eigen -lg2o_types_sim3 -lg2o_cli -lg2o_incremental -lg2o_simulator -lg2o_solver_pcg -lg2o_types_data -lg2o_types_slam2d_addons -lg2o_core -lg2o_interactive -lg2o_solver_cholmod -lg2o_solver_slam2d_linear -lg2o_types_icp -lg2o_types_slam2d -lg2o_csparse_extension -lg2o_interface -lg2o_solver_csparse -lg2o_solver_structure_only -lg2o_types_sba -lg2o_types_slam3d_addons -lg2o_ext_freeglut_minimal -lg2o_opengl_helper -lg2o_solver_dense -lg2o_stuff -lg2o_types_sclam2d -lg2o_types_slam3d -lcholmod -lopencv_imgcodecs -lopencv_dnn -lopencv_imgproc -lopencv_core
# -lopencv_imgcodecs -lopencv_dnn -lopencv_imgproc -lopencv_core是加进来的
IFLAGS += -I$(QTDIR)/include -I../ -I/opt/lampp/include/ -I/usr/include/mysql/ -I/usr/include/eigen3 

IFLAGS += -I$(SCIROCO_BASE_DIR)/src/exlib -I$(SCIROCO_BASE_DIR)/src/exlib/xstream/include  -I/usr/local/include/ -I/usr/include/suitesparse/ -I/usr/local/include/opencv/ 
# -I/usr/local/include/opencv/ 是加进来的

SOURCES = efficient_wifi_slam.cpp

PUBLIC_INCLUDES     = 
PUBLIC_LIBRARIES    = 
PUBLIC_BINARIES     =
PUBLIC_LIBRARIES_SO = 


TARGETS = efficient_wifi_slam

efficient_wifi_slam:efficient_wifi_slam.o
include $(CARMEN_HOME)/src/Makefile.rules
