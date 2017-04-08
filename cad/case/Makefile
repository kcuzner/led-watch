# LED Wristwatch
# Kevin Cuzner
#
# Case STL Makefile

SCAD=openscad
BASEFILE=case.scad

TARGETS=$(shell sed '/^module [a-z0-9_-]*\(\).*make..\?me.*$$/!d;s/module //;s/().*/.stl/' $(BASEFILE))

all: ${TARGETS}

.SECONDARY: $(shell echo "${TARGETS}" | sed 's/\.stl/.scad/g')

include $(wildcard *.deps)

%.scad: Makefile
	printf 'use <$(BASEFILE)>\n$*();' > $@

%.stl: %.scad
	openscad -m make -o $@ -d $@.deps $<

