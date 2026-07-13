ifeq ($(strip $(NXDK_DIR)),)
$(error Please set NXDK_DIR in your environment. For example:)
$(warning export NXDK_DIR=/opt/nxdk)
$(error Undefined NXDK_DIR (root directory))
endif

.SUFFIXES:


#---------------------------------------------------------------------------------
# Configurable options
#---------------------------------------------------------------------------------
# Name of the final output
TARGET		= ClassiCube-xbox
# Application name/title
XBE_TITLE 	= ClassiCube
# List of directories containing source code
SOURCE_DIRS = src src/xbox third_party/bearssl
# Shader objects
SHADER_OBJS = build/xbox/vs_coloured.inl build/xbox/vs_textured.inl build/xbox/vs_offset.inl build/xbox/ps_coloured.inl build/xbox/ps_textured.inl
# Directory where object files are placed
BUILD_DIR   = build/xbox


#---------------------------------------------------------------------------------
# Compilable files
#---------------------------------------------------------------------------------
S_FILES = $(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.S))
C_FILES = $(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.c))
OBJS 	= $(addprefix $(BUILD_DIR)/, $(notdir $(C_FILES:%.c=%.o) $(S_FILES:%.S=%.o)))

# Dependency tracking
DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD_DIR)/$*.d
DEPFILES := $(OBJS:%.o=%.d)


#---------------------------------------------------------------------------------
# Code generation
#---------------------------------------------------------------------------------
CFLAGS = -O2 -fno-math-errno -Wno-builtin-macro-redefined \
	-I$(NXDK_DIR)/lib/net/lwip/src/include \
	-I$(NXDK_DIR)/lib/net/nforceif/include \
	-I$(NXDK_DIR)/lib/net/nvnetdrv \
	-I$(NXDK_DIR)/lib/usb/libusbohci/inc \
	-I$(NXDK_DIR)/lib/usb/libusbohci_xbox/ \
	-DUSBH_USE_EXTERNAL_CONFIG=\"usbh_config_xbox.h\"

LDFLAGS = -stack:196608 \
	$(NXDK_DIR)/lib/libnxdk.lib \
	$(NXDK_DIR)/lib/libnxdk_hal.lib \
	$(NXDK_DIR)/lib/libnxdk_net.lib \
	$(NXDK_DIR)/lib/libpbkit.lib \
	$(NXDK_DIR)/lib/libpdclib.lib \
	$(NXDK_DIR)/lib/libxboxrt.lib \
	$(NXDK_DIR)/lib/libwinapi.lib \
	$(NXDK_DIR)/lib/nxdk_usb.lib \
	$(NXDK_DIR)/lib/xboxkrnl/libxboxkrnl.lib


#---------------------------------------------------------------------------------
# Tools
#---------------------------------------------------------------------------------
NXDK_CC = $(NXDK_DIR)/bin/nxdk-cc
NXDK_LD = $(NXDK_DIR)/bin/nxdk-link

CGC = $(NXDK_DIR)/tools/cg/linux/cgc
# NOTE: Linux only. Would need changing for other platforms

CXBE         = $(NXDK_DIR)/tools/cxbe/cxbe
VP20COMPILER = $(NXDK_DIR)/tools/vp20compiler/vp20compiler
FP20COMPILER = $(NXDK_DIR)/tools/fp20compiler/fp20compiler
EXTRACT_XISO = $(NXDK_DIR)/tools/extract-xiso/build/extract-xiso


#---------------------------------------------------------------------------------
# Main targets
#---------------------------------------------------------------------------------
default: $(BUILD_DIR) $(TARGET).iso

clean:
	rm -f $(TARGET).iso $(TARGET).xbe $(TARGET).exe $(OBJS) $(SHADER_OBJS) $(DEPFILES)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


#---------------------------------------------------------------------------------
# Executable generation
#---------------------------------------------------------------------------------
src/xbox/Graphics_Xbox.c : $(SHADER_OBJS)

$(TARGET).iso: $(TARGET).xbe
	mkdir -p $(BUILD_DIR)/cd
	cp $(TARGET).xbe $(BUILD_DIR)/cd/default.xbe
	$(EXTRACT_XISO) -c $(BUILD_DIR)/cd $(XISO_FLAGS) $@

$(TARGET).xbe: $(TARGET).exe
	$(CXBE) -OUT:$@ -TITLE:$(XBE_TITLE) $<

$(TARGET).exe: $(OBJS)
	$(NXDK_LD) $(LDFLAGS) -out:$@ $^


#---------------------------------------------------------------------------------
# Object generation
#---------------------------------------------------------------------------------
$(BUILD_DIR)/%.o: src/%.c
	$(NXDK_CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: src/xbox/%.c
	$(NXDK_CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: src/xbox/%.S
	$(NXDK_CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: third_party/bearssl/%.c
	$(NXDK_CC) $(CFLAGS) -c $< -o $@


#---------------------------------------------------------------------------------
# shader object generation
#---------------------------------------------------------------------------------
.PRECIOUS: misc/xbox/vs_%.asm misc/xbox/ps_%.asm
# prevent the .asm files from being deleted

$(BUILD_DIR)/vs_%.inl: misc/xbox/vs_%.asm
	$(VP20COMPILER) $< > $@

misc/xbox/vs_%.asm: misc/xbox/vs_%.cg
	$(CGC) -profile vp20 $< -o $@

$(BUILD_DIR)/ps_%.inl: misc/xbox/ps_%.asm
	$(FP20COMPILER) $< > $@

misc/xbox/ps_%.asm: misc/xbox/ps_%.cg
	$(CGC) -profile fp20 $< -o $@


#---------------------------------------------------------------------------------
# Dependency tracking
#---------------------------------------------------------------------------------
$(DEPFILES):

include $(wildcard $(DEPFILES))
