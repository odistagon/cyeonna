TARGET = libjs
OBJS = main.o jsapi.o jsdbgapi.o jsinterp.o jsobj.o jsstr.o jsarena.o jsdhash.o jslock.o jsopcode.o jsutil.o jsarray.o jsdtoa.o jslog2.o jsparse.o jsxdrapi.o jsatom.o jsemit.o jslong.o jsprf.o jsxml.o jsexn.o jsmath.o jsregexp.o jsbool.o jsfun.o jsscan.o prmjtime.o jscntxt.o jsgc.o jsnum.o jsscope.o jsdate.o jshash.o jsscript.o
# (ocluded js.o, jsmathtemp.o)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp
PSP_FW_VERSION = 371

#USE_PSPSDK_LIBC = 1

INCDIR = 
CFLAGS = -g -O2 -G0 -Wall -fno-strict-aliasing -fno-builtin-printf
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LDFLAGS = -mno-crt0 -nostartfiles 
LIBDIR =
LIBS += -lpsprtc -lm 

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

