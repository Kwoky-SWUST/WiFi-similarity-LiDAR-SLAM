CARMEN_HOME          = ../../../exlib/carmen
include $(CARMEN_HOME)/src/Makefile.conf
SCIROCO_BASE_DIR = ../../../..
include $(SCIROCO_BASE_DIR)/src/carmenx/Makefile.carmenx

MODULE_NAME    = "SCIROCO STATISTICS UTILS"
MODULE_COMMENT = "Utilities for statistical computations"

CROCO_BASE_DIR = ../../../../../..

CXXFLAGS += -g3
CFLAGS   += -g3 -UHAVE_LAPACK

SOURCES = cusum.cpp

PUBLIC_INCLUDES     =
PUBLIC_LIBRARIES    = libsciroco_utils_statistics.a
PUBLIC_BINARIES     =
PUBLIC_LIBRARIES_SO = libsciroco_utils_statistics.so

TARGETS = libsciroco_utils_statistics.a

libsciroco_utils_statistics.a:       \
          cusum.o


include $(CARMEN_HOME)/src/Makefile.rules
