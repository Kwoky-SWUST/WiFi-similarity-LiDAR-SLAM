CARMEN_HOME          = ../../../exlib/carmen
include $(CARMEN_HOME)/src/Makefile.conf
SCIROCO_BASE_DIR = ../../../..
include $(SCIROCO_BASE_DIR)/src/carmenx/Makefile.carmenx

SUBDIRS = apps tests
MODULE_NAME    = "KERNEL DENSITY ESTIMATION"
MODULE_COMMENT = "Kernel Density Estimation in 1D or 2D (not part of standard CARMEN)"

CROCO_BASE_DIR = ../../../../../..

CXXFLAGS += -g3 -std=c++11
CFLAGS   += -g3 -UHAVE_LAPACK
LFLAGS   +=

SOURCES = kde.cpp

PUBLIC_INCLUDES     = kde.hpp
PUBLIC_LIBRARIES    = libcarmen_utils_KDE.a
PUBLIC_BINARIES     =
PUBLIC_LIBRARIES_SO = libcarmen_utils_KDE.so

TARGETS = libcarmen_utils_KDE.a


libcarmen_utils_KDE.a:kde.o


include $(CARMEN_HOME)/src/Makefile.rules
