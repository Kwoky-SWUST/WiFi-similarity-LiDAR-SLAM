CARMEN_HOME          = ../../../exlib/carmen
include $(CARMEN_HOME)/src/Makefile.conf
SCIROCO_BASE_DIR = ../../../..
include $(SCIROCO_BASE_DIR)/src/carmenx/Makefile.carmenx

SUBDIRS = apps tests
MODULE_NAME    = "GEOMETRY UTILS"
MODULE_COMMENT = "General utilities (not part of standard CARMEN)"

CROCO_BASE_DIR = ../../../../../..

CXXFLAGS += -g3
CFLAGS   += -g3 -UHAVE_LAPACK
LFLAGS   +=
#-lparam_interface -lbase_interface -llocalize_interface -lglobal -lipc -lpthread -lstdc++
#            -L$(CROCO_BASE_DIR)/lib -lutils
#-L$(QTDIR)/lib -lqt-mt


IFLAGS += -I$(QTDIR)/include -I$(SCIROCO_BASE_DIR)/src/exlib

SOURCES = point_utils.cpp pose.cpp coordinate_transformation.cpp intersections.cpp regular_grid_geometry_3d.cpp regular_grid_geometry_6d.cpp

PUBLIC_INCLUDES     = point_extension.h vector2d.h vector3d.h regular_grid.hpp
PUBLIC_LIBRARIES    = libcarmen_utils_geometry.a
PUBLIC_BINARIES     =
PUBLIC_LIBRARIES_SO = libcarmen_utils_geometry.so

TARGETS = libcarmen_utils_geometry.a


libcarmen_utils_geometry.a:           \
          coordinate_transformation.o \
          point_utils.o               \
          pose.o                      \
          intersections.o             \
          regular_grid_geometry_3d.o  \
          regular_grid_geometry_6d.o

include $(CARMEN_HOME)/src/Makefile.rules
