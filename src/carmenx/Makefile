SHELL = /bin/bash

SCIROCO_BASE_DIR = ../..
include $(SCIROCO_BASE_DIR)/src/exlib/carmen/src/Makefile.vars

#packages that actually get compiled!
PACKAGES = IMU \
           utils/geometry \
           utils/raytracer \
           utils/statistics \
           utils/vfh_algorithem \
           utils/kernel_density_estimator \
	   ambisense_slam/filters \
	   ambisense_slam/sport_hub

MY_CARMEN_HOME          = ../exlib/carmen
MY_CARMEN_HOME_FROM_BIN = ../src/exlib/carmen

OWN_BINARIES     = ambisense_slam/filters

CARMEN_BINARIES  = central \
                   localize \
                   param_daemon \
                   vasco  robot robot_gui navigator navigatorgui proccontrol

BINARIES_TO_LINK = $(CARMEN_BINARIES) $(OWN_BINARIES)

CREATE_LINKS_TO_BINARIES = \
	cd $(MY_CARMEN_HOME)/bin; \
	CARMEN_BINS="*"; \
	echo $$CARMEN_BINS; \
	for b in $$CARMEN_BINS; do \
		echo $$b; \
		if [ -x $$b ]; then \
			ln -f -s $(MY_CARMEN_HOME_FROM_BIN)/bin/$$b ../../../../bin/$$b; \
			echo " -- Created link to $$b"; \
		fi; \
	done; \
	cd -;
#$(MY_CARMEN_HOME_FROM_BIN)/bin/
# 	@for b in $(BINARIES_TO_LINK); do


RECURSE = @ \
	@for i in $(PACKAGES) xxxx ; do \
		if [ -d $$i ] ; then \
			echo Making "$@" in $$i...; \
			if ! $(MAKE) -s -C $$i $@ ; then \
                                exit -1; \
                        fi; \
		fi \
	done


RECURSE_ALL = @ \
	@for i in $(ALL_PACKAGES) xxxx ; do \
		if [ -d $$i ] ; then \
			echo Making "$@" in $$i...; \
			if ! $(MAKE) -s -C $$i $@ ; then \
                                exit -1; \
                        fi; \
		fi \
	done

.PHONY: create_links_to_binaries

#export
all: connect2carmen create_links_to_binaries export phase1 phase2
	$(CREATE_LINKS_TO_BINARIES)

connect2carmen:
	@if [ -f $(MY_CARMEN_HOME)/src/Makefile.rules ]; then \
		ln -f -s $(MY_CARMEN_HOME)/src/Makefile.conf  Makefile.conf; \
		ln -f -s $(MY_CARMEN_HOME)/src/Makefile.rules Makefile.rules; \
		ln -f -s $(MY_CARMEN_HOME)/src/Makefile.vars  Makefile.vars; \
		echo Created links to CARMEN Makefile.\*; \
	else \
		echo Could not find CARMEN make files; \
		exit 1; \
	fi;

create_links_to_binaries:
	$(CREATE_LINKS_TO_BINARIES)

# build: connect2carmen
# 	$(RECURSE)
# 	$(CREATE_LINKS_TO_BINARIES)

clean:
	$(RECURSE)

distclean:
	$(RECURSE_ALL)

export:
	$(RECURSE_ALL)

phase1:
	$(RECURSE)

phase2:
	$(RECURSE)

phase3:
phase4:
