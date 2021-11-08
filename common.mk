CROSS_COMPILE = aarch64-linux-gnu-
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
CC      = $(CROSS_COMPILE)gcc
CPP     = $(CC) -E
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm

STRIP       = $(CROSS_COMPILE)strip
OBJCOPY     = $(CROSS_COMPILE)objcopy
OBJDUMP     = $(CROSS_COMPILE)objdump

CFLAGS := -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only

LINK_SCRIPT := $(BUILD_ROOT)/kernel.lds
LDFLAGS :=  -T $(LINK_SCRIPT)

ifeq ($(DEBUG), true)
  VERSION = debug
  CFLAGS += -g
  CFLAGS += -DDEBUG
else
  VERSION = release
endif

S_SRCS = $(wildcard *.S)
S_OBJS = $(S_SRCS:.S=_S.o)
S_DEPS = $(S_SRCS:.S=_S.d)

C_SRCS = $(wildcard *.c)
C_OBJS = $(C_SRCS:.c=_c.o)
C_DEPS = $(C_SRCS:.c=_c.d)

LINK_OBJ_DIR = $(BUILD_ROOT)/.link_obj
DEP_DIR = $(BUILD_ROOT)/.dep

$(shell mkdir -p $(LINK_OBJ_DIR))
$(shell mkdir -p $(DEP_DIR))

S_OBJS := $(addprefix $(LINK_OBJ_DIR)/,$(S_OBJS))
S_DEPS := $(addprefix $(DEP_DIR)/,$(S_DEPS))

C_OBJS := $(addprefix $(LINK_OBJ_DIR)/,$(C_OBJS))
C_DEPS := $(addprefix $(DEP_DIR)/,$(C_DEPS))

LINK_OBJ = $(wildcard $(LINK_OBJ_DIR)/*.o)
LINK_OBJ += $(S_OBJS)
LINK_OBJ += $(C_OBJS)

ELF := $(addprefix $(BUILD_ROOT)/, $(ELF))
IMG := $(addprefix $(BUILD_ROOT)/, $(IMG))

all: $(C_DEPS) $(S_DEPS) $(S_OBJS) $(C_OBJS) $(IMG) $(ELF)

ifneq ("$(wildcard $(C_DEPS))", "")
  include $(C_DEPS)
endif

ifneq ("$(wildcard $(S_DEPS))", "")
  include $(S_DEPS)
endif

$(IMG): $(ELF)
	$(OBJCOPY) $^ -O binary $@

$(ELF): $(LINK_OBJ) $(LINK_SCRIPT)
	@echo "------------------------------- build $(VERSION) version  ------------------------------------"
	$(LD) -o $@ $^ $(LDFLAGS)

$(LINK_OBJ_DIR)/%_S.o: %.S
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) -o $@ -c $(filter %.S, $^)

$(LINK_OBJ_DIR)/%_c.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) -o $@ -c $(filter %.c, $^)

$(DEP_DIR)/%_c.d: %.c
	echo -n $(LINK_OBJ_DIR)/ > $@
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) -MM $^ >> $@
	sed -i 's/\.o/_c\.o/g' $@

$(DEP_DIR)/%_S.d: %.S
	echo -n $(LINK_OBJ_DIR)/ > $@
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) -MM $^ >> $@
	sed -i 's/\.o/_S\.o/g' $@