# GNU Make project makefile autogenerated by Premake
ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifndef CC
  CC = gcc
endif

ifndef CXX
  CXX = g++
endif

ifndef AR
  AR = ar
endif

ifeq ($(config),debug)
  OBJDIR     = ../obj/Debug/comparison
  TARGETDIR  = ../bin/Debug
  TARGET     = $(TARGETDIR)/comparison
  DEFINES   += -DDEBUG -D_DEBUG -DPLATFORM_CHECKED -DUNIX_BUILD -DMFUNC_TEST -DVFUNC_TEST -DCLASS_PARAM_IMPLICIT_CAST_TEST -DOOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA=1 -DOOLUA_USERDATA_OPTIMISATION=1
  INCLUDES  += -I. -I../include -Iinclude/lua
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -g -Wall -W -Wall -ansi -pedantic -std=c++98
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -L../bin/Debug
  LIBS      += -loolua_d -llua
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += ../bin/Debug/liboolua_d.a
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),release)
  OBJDIR     = ../obj/Release/comparison
  TARGETDIR  = ../bin/Release
  TARGET     = $(TARGETDIR)/comparison
  DEFINES   += -DNDEBUG -DRELEASE -DPLATFORM_CHECKED -DUNIX_BUILD -DMFUNC_TEST -DVFUNC_TEST -DCLASS_PARAM_IMPLICIT_CAST_TEST -DOOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA=1 -DOOLUA_USERDATA_OPTIMISATION=1
  INCLUDES  += -I. -I../include -Iinclude/lua
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -O2 -Wall -W -Wall -ansi -pedantic -std=c++98
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -s -L../bin/Release
  LIBS      += -loolua -llua
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += ../bin/Release/liboolua.a
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

OBJECTS := \
	$(OBJDIR)/hierarchy_expose.o \
	$(OBJDIR)/swig_profile_wrap.o \
	$(OBJDIR)/set_n_get_expose.o \
	$(OBJDIR)/compare.o \

RESOURCES := \

SHELLTYPE := msdos
ifeq (,$(ComSpec)$(COMSPEC))
  SHELLTYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(SHELL)))
  SHELLTYPE := posix
endif

.PHONY: clean prebuild prelink

all: $(TARGETDIR) $(OBJDIR) prebuild prelink $(TARGET)
	@:

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking comparison
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning comparison
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
$(GCH): $(PCH)
	@echo $(notdir $<)
	-$(SILENT) cp $< $(OBJDIR)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
endif

$(OBJDIR)/hierarchy_expose.o: hierarchy_expose.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/swig_profile_wrap.o: swig_profile_wrap.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/set_n_get_expose.o: set_n_get_expose.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/compare.o: compare.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
