
#XJSE_SYS = Linux, PSP
ifndef  XJSE_SYS
	XJSE_SYS = Linux
endif
ifeq ($(XJSE_SYS), Linux)
	CFLAGS += -DENABLEGLUT
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
TARGET = $(TARGET_DIR)libxgg.so
endif
ifeq ($(XJSE_SYS), PSP)
TARGET = $(TARGET_DIR)libxgg.a
endif
HDRS = xgg.h 
SRCS = xgg_debug.c xgg_dom.c xgg_system.c xgg_options.c xgg_view.c xgg_basicelement.c xgg_button.c xgg_div.c xgg_img.c xgg_label.c xgg_progressbar.c xgg_plugin.c xgg_pluginhelper.c xgg.c 
OBJS = $(patsubst %.c,$(TARGET_DIR)%.o,$(SRCS))
CFLAGS += -g -Wall -DXP_UNIX -DXTPF_$(XJSE_SYS)
LDFLAGS += -g 
ifeq ($(XJSE_SYS), Linux)
LDFLAGS += -shared 
endif
INCDIR = ../ ../js ../expat ../xtk ../pspinc
ifeq ($(XJSE_SYS), PSP)
INCDIR += $(PSPDEVDIR)/psp/sdk/include/
endif
INCS = $(patsubst %,-I%, $(INCDIR)) 
LIBS =
ifeq ($(XJSE_SYS), Linux)
#LDFLAGS += `pkg-config --libs gtk+-x11-2.0` \
#	`pkg-config --libs gmodule-2.0` \
#	`pkg-config --libs glib-2.0`
#INCS += ../ `pkg-config --cflags-only-I gtk+-x11-2.0` \
#	`pkg-config --cflags-only-I gmodule-2.0` \
#	`pkg-config --cflags-only-I glib-2.0`
else
endif

$(TARGET): $(OBJS)
ifeq ($(XJSE_SYS), Linux)
	@$(CC) $(LDFLAGS) -o $(TARGET) $^ $(LIBS)
	@echo "  LD $< -> $@"
endif
ifeq ($(XJSE_SYS), PSP)
	@$(AR) rcs $(TARGET) $^
	@echo "  AR $< -> $@"
endif

$(OBJS): $(TARGET_DIR)%.o: %.c
	@$(CC) $(CFLAGS) $(INCS) -c $< -o $@
	@echo "  CC $< -> $@"
#.c.o:
#	$(CC) -c $< $(CFLAGS) $(INCS) 

all: $(TARGET)
	@echo "$(TARGET) $(OBJS) built."

clean:
	@echo "  removing $(OBJS) $(TARGET)"
	@rm -f $(OBJS) $(TARGET)

.PHONY: clean

