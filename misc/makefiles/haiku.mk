#-----------------------------
# Configurable flags and names
#-----------------------------
SOURCE_DIRS := src src/beos third_party/bearssl
BUILD_DIR	:= build/haiku

CFLAGS  := -fvisibility=hidden -fno-ident
LIBS 	:= -lGL -lnetwork -lbe -lgame -ltracker
include misc/makefiles/common_config.mk


LINK    := $(CXX)
include misc/makefiles/common_build.mk
