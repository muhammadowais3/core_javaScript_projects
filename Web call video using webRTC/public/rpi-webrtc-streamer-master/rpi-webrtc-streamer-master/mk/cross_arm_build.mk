SYSROOT=/opt/rpi_rootfs
SYSROOTFLAG=--sysroot=$(SYSROOT)
#ARMIFY=-mfpu=neon-vfpv4 -mfloat-abi=hard -funsafe-math-optimizations
#BACKTRACE=-funwind-tables -rdynamic


CC=arm-linux-gnueabihf-gcc $(SYSROOTFLAG) $(BACKTRACE) $(ARMIFY)
CXX=arm-linux-gnueabihf-g++ $(SYSROOTFLAG) $(BACKGRACE) $(ARMIFY)
AR=arm-linux-gnueabihf-ar
LD=arm-linux-gnueabihf-ld
AS=arm-linux-gnueabihf-as
RANLIB=arm-linux-gnueabihf-ranlib

#
# WebRTC source tree and object directory 
# 
WEBRTC_ROOT=$(HOME)/Workspace/webrtc
WEBRTC_OUTPUT=arm_build
WEBRTC_LIBPATH=$(WEBRTC_ROOT)/src/$(WEBRTC_COUTPUT)

## WebRTC library build script
# GN Build
WEBRTC_NINJA_EXTACTOR=../mk/webrtc_library_gn.py $(WEBRTC_ROOT) $(WEBRTC_OUTPUT)

WEBRTC_CFLAGS=$(shell $(WEBRTC_NINJA_EXTACTOR) cflags)
WEBRTC_CCFLAGS=$(shell $(WEBRTC_NINJA_EXTACTOR) ccflags)
WEBRTC_DEFINES=$(shell $(WEBRTC_NINJA_EXTACTOR) defines)

WEBRTC_FLAGS_INCLUDES = -I$(WEBRTC_ROOT)/src 
WEBRTC_FLAGS_INCLUDES += $(shell $(WEBRTC_NINJA_EXTACTOR) includes)

WEBRTC_BUILD_LIBS=$(shell $(WEBRTC_NINJA_EXTACTOR) libs)
WEBRTC_SYSLIBS = $(shell $(WEBRTC_NINJA_EXTACTOR) syslibs)
WEBRTC_LDFLAGS = $(shell $(WEBRTC_NINJA_EXTACTOR) ldflags)

#$(info WEBRTC_CFLAGS is "$(WEBRTC_CFLAGS)")
#$(info WEBRTC_CCFLAGS is  "$(WEBRTC_CCFLAGS)")
#$(info WEBRTC_FLAGS_INCLUDES is "$(WEBRTC_FLAGS_INCLUDES)")
#$(info WEBRTC_SYSLIBS is "$(WEBRTC_SYSLIBS)")
#$(info WEBRTC_LDFLAGS is "$(WEBRTC_LDFLAGS)")
#$(info WEBRTC_BUILD_LIBS is "$(WEBRTC_BUILD_LIBS)")

SYSROOT=/opt/rpi_rootfs
SYSROOTFLAG=--sysroot=$(SYSROOT)



