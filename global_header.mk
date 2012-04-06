#define thrift_template
#  $(1) : $(2)
#	$$(THRIFT) $(3) $(4) $(5) $(6) $(7) $(8) $$<
#endef



define thrift_template
XTARGET := $(shell perl -e '@val = split("\/","$(2)"); @val = split("\\.",@val[-1]); print "$(1)/"."gen-cpp/"."@val[0]"."_types.cpp\n"' )

ifneq ($$(XBUILT_SOURCES),) 
    XBUILT_SOURCES := $$(XBUILT_SOURCES) $$(XTARGET)
else
    XBUILT_SOURCES := $$(XTARGET)
endif
$$(XTARGET) : $(2)
	$$(THRIFT) -o $1 $3 $$<
endef

clean-common:
	rm -rf gen-*
