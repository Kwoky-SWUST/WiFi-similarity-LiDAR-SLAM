CARMEN_HOME          = ../../../exlib/carmen
include $(CARMEN_HOME)/src/Makefile.conf
SCIROCO_BASE_DIR = ../../../..
include $(SCIROCO_BASE_DIR)/src/carmenx/Makefile.carmenx

SUBDIRS = apps tests
MODULE_NAME    = "RAYTRACER"
MODULE_COMMENT = "Raytracing on CARMEN grid maps"

CROCO_BASE_DIR = ../../../../../..

CXXFLAGS += -g3
CFLAGS   += -g3 -UHAVE_LAPACK
LFLAGS   += -lparam_interface -lmap_interface -lglobal -lipc -lpthread -lstdc++
#-lparam_interface -lbase_interface -llocalize_interface -lglobal -lipc -lpthread -lstdc++
#            -L$(CROCO_BASE_DIR)/lib -lutils
#-L$(QTDIR)/lib -lqt-mt


IFLAGS += -I$(QTDIR)/include

SOURCES = grid_raytracer.cpp test_raytracer.cpp

PUBLIC_INCLUDES     = grid_raytracer.h
PUBLIC_LIBRARIES    = libcarmen_utils_raytracer.a
PUBLIC_BINARIES     =
PUBLIC_LIBRARIES_SO = libcarmen_utils_raytracer.so

TARGETS = libcarmen_utils_raytracer.a test_raytracer


libcarmen_utils_raytracer.a: grid_raytracer.o

test_raytracer: test_raytracer.o libcarmen_utils_raytracer.a


include $(CARMEN_HOME)/src/Makefile.rules
