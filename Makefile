
#XJSE_SYS = Linux, PSP
ifndef	XJSE_SYS
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
endif
ifeq ($(XJSE_SYS), PSP)
CC = psp-gcc
BUILD_PRX = 1
PSP_FW_VERSION = 371
#USE_KERNEL_LIBC = 1
#USE_KERNEL_LIBS = 1
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Cyeonna test buid
PSPSDK=$(shell psp-config --pspsdk-path)
endif

TARGET_DIR = ./$(XJSE_SYS).OBJ/
TARGET = $(TARGET_DIR)cyeonna
HDRS = xjse.h
SRCS_C = main.c xjse.c xjse_jsutil.c xjse_debug.c xjse_xhr.c xjse_expat.c xjse_fileio.c xjse_xpnet.c
SRCS_CPP = 
#OBJS_ = $(SRCS_C:.c=.o) $(SRCS_CPP:.cpp=.o) 
OBJS_C = $(patsubst %.c,$(TARGET_DIR)%.o,$(SRCS_C)) 
OBJS_CPP = $(patsubst %.cpp,$(TARGET_DIR)%.o,$(SRCS_CPP))
OBJS = $(OBJS_C) $(OBJS_CPP) 
ifeq ($(XJSE_SYS), PSP)
OBJS += libjs.o
endif
CFLAGS += -g -Wall -DXP_UNIX -DXTPF_$(XJSE_SYS) 
LDFLAGS += -g 
INCDIR = ./js ./expat ./pspinc
ifeq ($(XJSE_SYS), PSP)
INCDIR += /home/altair/local/pspdev/psp/sdk/include/
endif
INCS = $(patsubst %,-I%, $(INCDIR)) 
LIBDIR = $(TARGET_DIR)
#XXXX20081223LIBS = $(patsubst %,-L%, $(LIBDIR)) -ljs -lexpat -lxgg -lxtk 
LIBS = $(patsubst %,-L%, $(LIBDIR)) -lexpat -lxgg -lxtk 
LIBS += -lpng -lz -ljpeg
ifeq ($(XJSE_SYS), Linux)
LIBS += -ljs -lglut -lGLU
endif
ifeq ($(XJSE_SYS), PSP)
LIBS += -lintrafont -lpsppower -lpsprtc -lpspgum -lpspgu 
#LIBS += -lpspnet -lpspnet_inet -lpspnet_apctl -lpspnet_resolver 
LIBS += -lm 
endif

ifeq ($(XJSE_SYS), Linux)
$(TARGET): $(OBJS_C) $(OBJS_CPP) 
	@$(CC) $(LDFLAGS) -o $(TARGET) $^ $(LIBS) 
	@echo "  LD $< -> $@"

clean:
	@echo "  removing $(OBJS_C) $(OBJS_CPP) $(TARGET)"
	@rm -f $(OBJS_C) $(OBJS_CPP) $(TARGET)

all: $(TARGET_DIR)$(TARGET)
#	make -C xtk/
	@echo "$(TARGET) $(OBJS_C) $(OBJS_CPP) built."
endif

$(OBJS_C): $(TARGET_DIR)%.o: %.c
	@$(CC) $(CFLAGS) $(INCS) -c $< -o $@
	@echo "  CC $< -> $@"
$(OBJS_CPP): $(TARGET_DIR)%.o: %.cpp
	@$(CC) $(CFLAGS) $(INCS) -c $< -o $@
	@echo "  CC $< -> $@"
#.c.o:
#	@$(CC) $(CFLAGS) -c $(INCS) $< -o $(TARGET_DIR)$*.o 
#.cpp.o:
#	@$(CC) $(CFLAGS) -c $(INCS) $< -o $(TARGET_DIR)$*.o 

debug: $(TARGET)
	LD_LIBRARY_PATH=./Linux.OBJ:$LD_LIBRARY_PATH gdb $(TARGET)

ifeq ($(XJSE_SYS), PSP)
include $(PSPSDK)/lib/build.mak
endif

.PHONY: clean
