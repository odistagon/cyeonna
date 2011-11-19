
#XJSE_SYS = Linux, PSP
ifndef  XJSE_SYS
	XJSE_SYS = Linux
endif
ifeq ($(XJSE_SYS), PSP)
    CFLAGS += -DXP_PSP
endif
#ifeq ($(strip $(foo)), )

ifeq ($(XJSE_SYS), Linux)
CC = gcc
AR = ar
endif
ifeq ($(XJSE_SYS), PSP)
CC = psp-gcc
AR = psp-ar
endif

TARGET_DIR = ../$(XJSE_SYS).OBJ/
ifeq ($(XJSE_SYS), Linux)
TARGET = $(TARGET_DIR)libexpat.so
endif
ifeq ($(XJSE_SYS), PSP)
TARGET = $(TARGET_DIR)libexpat.a
endif
HDRS = ascii.h asciitab.h expat_config.h expat_external.h expat.h iasciitab.h internal.h latin1tab.h nametab.h utf8tab.h xmlrole.h xmltok.h xmltok_impl.h
SRCS = xmlparse.c xmlrole.c xmltok.c xmltok_ns.c xmltok_impl.c
OBJS = $(patsubst %.c,$(TARGET_DIR)%.o,$(SRCS))
CFLAGS += -g -Wall -DHAVE_EXPAT_CONFIG_H
LDFLAGS += -g -shared 
INCDIR = .
ifeq ($(XJSE_SYS), PSP)
INCDIR += ./compat_psp
endif
INCS = $(patsubst %,-I%, $(INCDIR)) 
LIBS =

$(TARGET): $(OBJS)
ifeq ($(XJSE_SYS), Linux)
	$(CC) $(LDFLAGS) -o $(TARGET) $^ $(LIBS)
endif
ifeq ($(XJSE_SYS), PSP)
	$(AR) rcs $(TARGET) $^ 
endif
	@echo "  LD $< -> $@"

$(OBJS): $(TARGET_DIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@
	@echo "  CC $< -> $@"
#.c.o:
#	$(CC) -c $< $(CFLAGS) $(INCS) 

all: $(TARGET)
	@echo "$(TARGET) $(OBJS) built."

clean:
	@echo "  removing $(OBJS) $(TARGET)"
	@rm -f $(OBJS) $(TARGET)

.PHONY: clean

