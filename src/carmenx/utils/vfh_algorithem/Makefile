CARMEN_HOME          = ../../../exlib/carmen
include $(CARMEN_HOME)/src/Makefile.conf
SCIROCO_BASE_DIR = ../../../..
include $(SCIROCO_BASE_DIR)/src/carmenx/Makefile.carmenx

SUBDIRS = apps tests
MODULE_NAME    = "VFH UTILS FOR OBSTACLE AVODANCE"
MODULE_COMMENT = "General utilities (not part of standard CARMEN)"

CROCO_BASE_DIR = ../../../../../..

CXXFLAGS += -g3
CFLAGS   += -g3 -UHAVE_LAPACK
LFLAGS   +=
#-lparam_interface -lbase_interface -llocalize_interface -lglobal -lipc -lpthread -lstdc++
#            -L$(CROCO_BASE_DIR)/lib -lutils
#-L$(QTDIR)/lib -lqt-mt


IFLAGS += -I$(QTDIR)/include

SOURCES = vfh_algorithm.cpp vfh_algorithmconfig.cpp

PUBLIC_INCLUDES     = vfh_algorithm.h vfh_algorithmconfig.h
PUBLIC_LIBRARIES    = libcarmen_utils_vfh.a
PUBLIC_BINARIES     =
PUBLIC_LIBRARIES_SO = libcarmen_utils_vfh.so

TARGETS = libcarmen_utils_vfh.a


libcarmen_utils_vfh.a:           \
          vfh_algorithm.o \
          vfh_algorithmconfig.o 

include $(CARMEN_HOME)/src/Makefile.rules
