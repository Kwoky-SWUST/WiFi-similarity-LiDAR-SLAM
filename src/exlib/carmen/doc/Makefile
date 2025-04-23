include ../src/Makefile.conf

MODULE_NAME = DOCUMENTATION
MODULE_COMMENT = The CARMEN Documentation

SOURCES =
PUBLIC_INCLUDES =
PUBLIC_LIBRARIES =
PUBLIC_BINARIES =

ifndef NO_DOXYGEN
TARGETS =   ambisense_docs ambisense.doxygen copy_files
#carmen.doxygen
endif

TO_HTML = tree.html tree-bin.html tree-src.html carmen-logo-tiny.png doxygen.css

copy_to_html =   $(SILENT) \
	for i in $(TO_HTML) ; do \
		echo "    ---- Copying $$i to html/" ; \
		$(CP) -f $$i html/; \
	done


rm_html =   $(SILENT) \
	if [ -d html ] ; then \
		echo "    ---- Removing html/" ; \
		$(RM) -rf html/; \
        fi

list_ambisense = cat carmen.dox.part1 > ambisense.dox; \
	find ../src/ambisense_slam/ -name "*.h" -printf " %p" >> ambisense.dox; \
	cat carmen.dox.part2 >> ambisense.dox


all: $(TARGETS)

carmen.doxygen:     carmen.dox
ambisense.doxygen:  ambisense.dox

copy_files:
	$(copy_to_html)


clean:
	$(rm_html)

ambisense_docs:
	$(list_ambisense)

%.doxygen:
	$(ECHO) "    ---- Compiling $< (Doxygen)"
	$(SILENT) $(DOXYGEN) $<

