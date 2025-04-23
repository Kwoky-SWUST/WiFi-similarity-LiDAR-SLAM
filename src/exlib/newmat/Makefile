MODULE = newmat
BASEPREFIX = ../../..

####### CONF #######

CONF      = $(BASEPREFIX)/src/Makefile.conf
RULES     = $(BASEPREFIX)/src/Makefile.rules


include $(CONF)

CFLAGS += -DBOOST_NO_STDC_NAMESPACE


####### INCLUDES #######

# Inclues for libxml #
# INCLUDES += -I/usr/include/libxml2


######  APPL1 #######

APPL1_DESCR    = "newmat10D by Robert Davies"
APPL1_LIBS     =
APPL1_SYS_LIBS =

APPL1_FILES    = newmat1 newmat2 newmat3 newmat4 newmat5 newmat6 newmat7 newmat8 newmatex bandmat submat myexcept cholesky evalue fft hholder jacobi newfft sort svd newmatrm newmat9


####### RULES #######


build: so1 lib1


### top level rules
include $(RULES)
