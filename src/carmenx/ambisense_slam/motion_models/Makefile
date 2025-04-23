CARMEN_HOME          = ../../../exlib/carmen
include $(CARMEN_HOME)/src/Makefile.conf
SCIROCO_BASE_DIR = ../../../..
include $(SCIROCO_BASE_DIR)/src/carmenx/Makefile.carmenx

MODULE_NAME    = "AMBISENSE SLAM MOTION MODELS"
MODULE_COMMENT = "Motion models used in the AmbiSense SLAM package"

CXXFLAGS += -g3
CFLAGS   += -g3
LFLAGS   += -lstdc++ \
  -lglobal -llocalize_interface -lbase_interface -lsimulator_interface -lparam_interface -lplayback_interface \
  -llaser_interface -lrfid_interface \
  -lrobot_interface -lipc -L$(SCIROCO_BASE_DIR)/lib -lmodels -lutils -lrfid \
  -L$(SCIROCO_BASE_DIR)/src/extlib/ann/lib -lANN \
  -L$(SCIROCO_BASE_DIR)/src/extlib/libsvm


IFLAGS += -I$(QTDIR)/include -I../ -I$(SCIROCO_BASE_DIR)/src/exlib -I$(SCIROCO_BASE_DIR)/src/exlib/xstream/include
IFLAGS += -I$(XSTREAM_INCLUDE_DIR)
IFLAGS += -I$(SCIROCO_BASE_DIR)/src/extlib/libsvm

SOURCES = diffusion_motion_model.cpp crossrange_downrange_rotation_motion_model.cpp diffusion_motion_model.cpp motion_model.cpp motion_model_parameters.cpp noiseless_motion_model.cpp rotation_translation_rotation_motion_model.cpp rigid_body_transformation_motion_model.cpp

PUBLIC_INCLUDES     = 
PUBLIC_LIBRARIES    = libambisense_slam_motion_models.a
PUBLIC_BINARIES     =
PUBLIC_LIBRARIES_SO = libambisense_slam_sensor_models.so

TARGETS = libambisense_slam_motion_models.a

libambisense_slam_motion_models.a: \
          crossrange_downrange_rotation_motion_model.o \
          diffusion_motion_model.o \
          motion_model.o \
          motion_model_parameters.o \
          noiseless_motion_model.o \
          rigid_body_transformation_motion_model.o \
          rotation_translation_rotation_motion_model.o


include $(CARMEN_HOME)/src/Makefile.rules
