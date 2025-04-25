# We use the FIRMWARE_MODE variable to determine various settings, and so check
# here that it is an allowable value
ifneq ($(FIRMWARE_MODE),APPLICATION)
  ifneq ($(FIRMWARE_MODE),BOOTLOADER)
    $(error FIRMWARE_MODE must be defined and set to either APPLICATION or BOOTLOADER)
  endif
endif

##############################################################################
# Project, sources and paths
#

BOARDDIR = $(COMMON)/boards
COMMON_CPPSRC = $(wildcard $(COMMON)/can-proto/driver/*.cpp) 
COMMON_CSRC =
COMMON_INC = $(COMMON)/can-proto $(COMMON)/config

ifeq ($(FIRMWARE_MODE),APPLICATION)
# Define linker script file here
LDSCRIPT= $(COMMON)/config/ld/STM32L432xC_application.ld
endif
ifeq ($(FIRMWARE_MODE),BOOTLOADER)
# Define linker script file here
LDSCRIPT= $(COMMON)/config/ld/STM32L432xC_bootloader.ld
endif

#Add chibios files
include $(COMMON)/chibios/chibios.mk
COMMON_CSRC += $(CHIBI_CSRC)
COMMON_CPPSRC += $(CHIBI_CPPSRC)
COMMON_INC += $(CHIBI_INCDIR)

#Add protocol files
PROTO_DIR = $(COMMON)/can-proto
include $(PROTO_DIR)/proto.mk
COMMON_CPPSRC += $(PROTO_CPPSRC)
COMMON_INC += $(PROTO_INCDIR)

#Default compile options
include $(COMMON)/defaults.mk

#
# Project, sources and paths
##############################################################################

##############################################################################
# General Project Information
#

# Extract version and repository information from git
GIT_REVISION:= $(shell git log -1 | head -1 | cut -d ' ' -f 2 | cut -c-7 )
GIT_PATH := $(shell git rev-parse --show-prefix)
GIT_MODIFIED := $(shell ( ! git diff --cached -s --exit-code || ! git diff --exit-code -s) && echo "(modified)")
# Shorthand 'modified' tag
GIT_MOD := $(shell ( ! git diff --cached -s --exit-code || ! git diff --exit-code -s) && echo "-m")

# Auto-generate summary information for firmware.  If BUILD_LABEL
# is provided, use that; otherwise, use git revision
FIRMWARE_REVISION := $(if $(BUILD_LABEL),$(BUILD_LABEL),$(GIT_PATH)@$(GIT_REVISION)$(GIT_MOD))

FIRMWARE_TAG := $(DESCRIPTION) $(GIT_MODIFIED)
FIRMWARE_DATE := $(shell date)
FIRMWARE_USERNAME := $(shell whoami | sed 's,\\\\,/,')

# List all default C defines here, like -D_DEBUG=1
COMMON_DDEFS = -D_FIRMWARE_REVISION="$(FIRMWARE_REVISION)"
COMMON_DDEFS += -D_FIRMWARE_TAG="$(FIRMWARE_TAG)"
COMMON_DDEFS += -D_FIRMWARE_DATE="$(FIRMWARE_DATE)"
COMMON_DDEFS += -D_FIRMWARE_USERNAME="$(FIRMWARE_USERNAME)"
COMMON_DDEFS += -D_FIRMWARE_TYPE="$(FIRMWARE_TYPE)"
COMMON_DDEFS += -D_ELECTRICAL_TYPE="$(ELECTRICAL_TYPE)"
COMMON_DDEFS += -D_BOARD_TYPE="$(BOARD_TYPE)"
# For string manipulation:
COMMON_DDEFS += -D_FIRMWARE_MODE="$(FIRMWARE_MODE)"
# For preprocessor ifdefs:
COMMON_DDEFS += -D_FIRMWARE_MODE_$(FIRMWARE_MODE)

#
# General Project Information
##############################################################################
