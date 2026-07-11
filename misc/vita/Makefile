ifeq ($(strip $(VITASDK)),)
$(warning "Please set VITASDK variables in your environment. For example:")
$(warning   export VITASDK=/usr/local/vitasdk)
$(error   Failed to find VitaSDK installation)
endif

.SUFFIXES:


#---------------------------------------------------------------------------------
# Configurable options
#---------------------------------------------------------------------------------
# Name of the final output
TARGET 			= ClassiCube-vita
# List of directories containing source code
SOURCE_DIRS		= src src/psvita third_party/bearssl
# Directory where object files are placed
BUILD_DIR		= build/vita

SFO_TITLE 	= ClassiCube
SFO_TITLEID = CUBE00200

PARAM_SFO    := $(BUILD_DIR)/param.sfo
EBOOT_BIN    := eboot.bin


#---------------------------------------------------------------------------------
# Compilable files
#---------------------------------------------------------------------------------
C_FILES = $(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.c))
S_FILES = $(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.S))
OBJS 	= $(addprefix $(BUILD_DIR)/, $(notdir $(C_FILES:%.c=%.o) $(S_FILES:%.S=%.o)))

# Dependency tracking
DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD_DIR)/$*.d
DEPFILES := $(OBJS:%.o=%.d)


#---------------------------------------------------------------------------------
# Code generation
#---------------------------------------------------------------------------------
CFLAGS 	= -O2 -DPLAT_VITA -fno-math-errno

LDFLAGS = -Wl,-q
LIBS 	= -lm -lSceDisplay_stub -lSceCtrl_stub -lSceTouch_stub -lSceGxm_stub -lSceCommonDialog_stub -lSceAppUtil_stub


#---------------------------------------------------------------------------------
# Compiler tools
#---------------------------------------------------------------------------------
PREFIX	:= $(VITASDK)/bin/arm-vita-eabi-
VITA_AS	:= $(PREFIX)as
VITA_CC	:= $(PREFIX)gcc
VITA_CXX:= $(PREFIX)g++
VITA_STRIP := $(PREFIX)strip

PACK_VPK 	:= $(VITASDK)/bin/vita-pack-vpk
MAKE_FSELF	:= $(VITASDK)/bin/vita-make-fself
MKSOFEX		:= $(VITASDK)/bin/vita-mksfoex
ELF_CREATE	:= $(VITASDK)/bin/vita-elf-create


#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
default: $(BUILD_DIR) $(TARGET).vpk

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm $(TARGET).velf $(TARGET).elf $(TARGET).vpk param.sfo eboot.bin $(OBJS)


#---------------------------------------------------------------------------------
# executable generation
#---------------------------------------------------------------------------------
$(TARGET).vpk: $(EBOOT_BIN) $(PARAM_SFO)
	$(PACK_VPK) -s $(PARAM_SFO) -b $(EBOOT_BIN) -a misc/vita/ICON0.png=sce_sys/ICON0.png $@

$(EBOOT_BIN): $(TARGET).velf
	$(MAKE_FSELF) $< $@

$(PARAM_SFO):
	$(MKSOFEX) -s TITLE_ID="$(SFO_TITLEID)" "$(SFO_TITLE)" $@

$(TARGET).velf: $(TARGET).elf
	$(VITA_STRIP) -g $<
	$(ELF_CREATE) $< $@

$(TARGET).elf: $(OBJS)
	$(VITA_CC) $(LDFLAGS) $^ $(LIBS) -o $@


#---------------------------------------------------------------------------------
# object generation
#---------------------------------------------------------------------------------
$(BUILD_DIR)/%.o : src/%.c
	$(VITA_CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o : src/psvita/%.c
	$(VITA_CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@
	
$(BUILD_DIR)/%.o : third_party/bearssl/%.c
	$(VITA_CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o : src/psvita/%.S
	$(VITA_CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@


#---------------------------------------------------------------------------------
# Dependency tracking
#---------------------------------------------------------------------------------
$(DEPFILES):

include $(wildcard $(DEPFILES))
