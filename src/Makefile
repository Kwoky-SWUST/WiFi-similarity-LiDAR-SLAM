SHELL = /bin/bash

NAME    = Indoor-Positioning

CONF    = Makefile.conf
RULES   = Makefile.rules

BASIC_MODULES = \
	utils                                     \
	extlib                                    \
	carmenx


MODULES = $(BASIC_MODULES)



# dont even compile the eclipse stuff
#COMPILE_FLAGS = -DCOMPILE_PURE_REACTIVE

export COMPILE_FLAGS

SILENT = @
ECHO   = $(SILENT)echo

FONTSPECIAL = echo; tput setaf 2 ; tput bold
ECHOMODULE = $(FONTSPECIAL) ; echo " --- Module:"
FONTNORMAL = tput sgr0


RECURSE = $(SILENT) \
 	$(CHECKLINUXTHREADS) \
	for i in $(MODULES) ; do \
		if [ -d $$i ] ; then \
			if ( cd $$i ; $(ECHOMODULE) $$i ; $(FONTNORMAL) ; \
				 ${MAKE} --no-print-directory $@ ; ) then \
				echo -n ; \
			else \
				exit 1 ; \
			fi ; \
		fi ; \
	done

PRINT_LOGO = @echo -e "\n"; \
	echo "This is $(NAME)"


all:
	$(PRINT_LOGO)
	$(ECHO)
	$(ECHO)
	$(ECHO) "       A L L"
	$(ECHO)
	$(MAKE_VISION)
	$(RECURSE)

rebuild: clean build

build:
	$(ECHO)
	$(ECHO)
	$(ECHO) "       B U I L D"
	$(ECHO)
	$(RECURSE)


export:
	$(ECHO)
	$(ECHO)
	$(ECHO) "       E X P O R T"
	$(ECHO)
	$(RECURSE)


remove_doc:
	$(ECHO)
	$(ECHO) "       R E M O V I N G   H T M L   D O C U M E N T A T I O N"
	$(SILENT) rm -rf ../doc/html


clean:
	$(ECHO)
	$(ECHO)
	$(ECHO) "       C L E A N"
	$(ECHO)
	$(RECURSE)

removelibs:
	$(ECHO)
	$(ECHO)
	$(ECHO) "       R E M O V E   L I B R A R I E S"
	$(ECHO)
	$(RECURSE)

distclean: remove_doc
	$(ECHO)
	$(ECHO)
	$(ECHO) "       D I S T C L E A N"
	$(ECHO)
	$(RECURSE)


depend:
	$(ECHO)
	$(ECHO)
	$(ECHO) "       D E P E N D"
	$(ECHO)
	$(RECURSE)


doc:
	$(SILENT) make -C ../doc


status:
	$(SILENT) cvs status | grep -e 'File\+\|?'


count-lines:
	grep -cR ".*" * | grep -e "\.\(h\|cpp\)\>" | sed s/.*://
