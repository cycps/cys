#Cypress Simulation Environment Makefile

BUILDDIR := build
DEPDIR := .d
OBJDIR := .o
ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
CPPFLAGS := "$(CPPFLAGS) -I $(ROOT_DIR)/common"
CXXFLAGS := "$(CXXFLAGS) -std=c++1y -stdlib=libc++"

$(shell mkdir -p $(BUILDDIR) >/dev/null)
$(shell mkdir -p $(DEPDIR) >/dev/null)
$(shell mkdir -p $(OBJDIR) >/dev/null)


COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

%.o : %.c $(DEPDIR)/%.d
        $(COMPILE.c) $(OUTPUT_OPTION) $<
        $(POSTCOMPILE)

%.o : %.cc $(DEPDIR)/%.d
        $(COMPILE.cc) $(OUTPUT_OPTION) $<
        $(POSTCOMPILE)

%.o : %.cxx $(DEPDIR)/%.d
        $(COMPILE.cc) $(OUTPUT_OPTION) $<
        $(POSTCOMPILE)

$(DEPDIR)/%.d: ;

.PHONY: all
.all: libcys.a libcyc.a

libcys: $(shell find sim --name "*.cxx")

include $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))
