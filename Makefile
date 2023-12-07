#
# Makefile
#

obj_dir = obj
STAGING_DIR := ~/Tina-Linux/out/t113-au_10/staging_dir/target/usr
CROSS = ~/Tina-Linux/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-
#CROSS = arm-openwrt-linux-

HEADER_PATH = -I $(STAGING_DIR)/include
HEADER_PATH += -I ${shell pwd}/modbus

#-I /home/ubuntu/auzoa/sdl_cross/include \
#-I $(STAGING_DIR)/include/SDL2 \

INCLUDES += -I$(STAGING_DIR)/include

LDFLAGS ?= -lm -luapi

LIB_PATH = -L $(STAGING_DIR)/lib 

CC = $(CROSS)gcc

MODBUS_DIR_NAME ?= modbus
MODBUS_DIR ?= ${shell pwd}

LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= ${shell pwd}
CFLAGS ?= -O3 -g0 -I$(LVGL_DIR)/ -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wmissing-prototypes -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wno-discarded-qualifiers -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare \
-D_GNU_SOURCE=1 -D_REENTRANT \


LIBS = -lasound -luapi -lts -lSDL2 -lsamplerate -lSDL2_mixer \
 -lmpg123 -lmad -ldirect -ldirectfb -lfusion \
 -LVGL8_USE_SUNXIFB_G2D:kmod-sunxi-g2d

#LDFLAGS += -static

BIN = tt



#Collect the files to compile
#Collect the files to compile
SRCDIRS = $(shell find . -maxdepth 1 -type d)
MAINSRC = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))


include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk

#Do not compile the example
EXCSRCS += $(shell find -L $(LVGL_DIR)/$(LVGL_DIR_NAME)/examples -name \*.c)
CSRCS := $(filter-out $(EXCSRCS),$(CSRCS))



OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)

## MAINOBJ -> OBJFILES

#all: default
all: default

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@ $(HEADER_PATH)
	@echo "CC $<"
    
default: $(AOBJS) $(COBJS) $(MAINOBJ)
	$(CC) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS) $(LIB_PATH) $(LIBS)

clean: 
	rm -f $(BIN) $(AOBJS) $(COBJS) $(MAINOBJ)



