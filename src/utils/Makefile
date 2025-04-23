MODULE = utils
BASEPREFIX = ../..

####### CONF #######

CONF      = $(BASEPREFIX)/src/Makefile.conf
RULES     = $(BASEPREFIX)/src/Makefile.rules


include $(CONF)

CFLAGS += -DBOOST_NO_STDC_NAMESPACE -ftemplate-depth-50


####### INCLUDES #######

# OTHERS #
INCLUDES += -I$(BASEPREFIX)/src/carmenx -I$(BASEPREFIX)/src/extlib -I$(BASEPREFIX)/src/extlib/carmen/include -I$(BASEPREFIX)/src/extlib/ARFF/src -I$(BASEPREFIX)/src/carmenx/ambisense_slam


######  APPL1 #######
### the utils library

APPL1_DESCR    = "Utilities for all system components"
APPL1_LIBS     = 
APPL1_SYS_LIBS =

APPL1_FILES  = 	\
                gnuplot/gnuplot_i               \
                gnuplot/gnuplot \
                graphics/netpbm \
                io/get_key \
                math/random \
                math/running_mean_and_std_dev \
                quality/sciroco_exception \
                serial/serial_interface \
                sys/mem \
                sys/processor \
                time/get_time \
                time/time_tracker \
                file_system/fs 

####### RULES #######


build: so1 lib1


### top level rules
include $(RULES)
