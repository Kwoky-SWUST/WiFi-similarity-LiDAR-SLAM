include carmen/src/Makefile.vars

all: build

build: 
	$(GZSTREAM_MAKE)
	make -C newmat
	$(make_xstream)
	$(make_GeographicLib)
	$(make_libdistance)
	$(CARMEN_CONFIGURE)
	$(CARMEN_MAKE)


clean:
	$(CARMEN_MAKE_CLEAN)
	make clean -C newmat

distclean:
	$(CARMEN_MAKE_DISTCLEAN)
	make distclean -C newmat

opencv:
	$(OPENCV_CONFIGURE)
	$(OPENCV_MAKE)

patch:
	$(PATCH_CARMEN)
	

XSTREAM_TARBALL = xstream-0.0.3.tar.bz2
XSTREAM_DIR     = xstream-0.0.3
PATCH_XSTREAM_0_0_3 = echo "Patching XSTREAM's md5.cpp..."; \
                      patch -b --verbose $(XSTREAM_DIR)/src/md5.cpp patches/patch_xstream_md5_cpp;  \
                      echo "Patch applied."

GZSTREAM_MAKE=cd gzstream; make; cd -

make_xstream=cd xstream;./configure; cd -;make -C xstream/src

make_GeographicLib=cd GeographicLib-1.44/BUILD;../configure;make;make install

make_libdistance=cd libdistance-0.2.2;make; cd -

CARMEN_CONFIGURE = if [ ! -f carmen/src/Makefile.vars ]; then \
		cd carmen/src; \
		./configure --nojava; \
		cd -; \
	fi;

CARMEN_MAKE = if [ -f carmen/src/Makefile.vars ]; then \
		cd carmen/src; \
		make; \
		cd -; \
	fi;

CARMEN_MAKE_CLEAN = if [ -f carmen/src/Makefile.vars ]; then \
		cd carmen/src; \
		make clean; \
		cd -; \
		rm carmen/src/Makefile.vars; \
	fi;

CARMEN_MAKE_DISTCLEAN = if [ -f carmen/src/Makefile.vars ]; then \
		cd carmen/src; \
		make distclean; \
		cd -; \
		rm carmen/src/Makefile.vars; \
	fi;
