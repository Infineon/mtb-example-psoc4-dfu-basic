################################################################################
# \file Makefile
# \version 1.0
#
# \brief
# Makefile for configuring the Cortex-M0 bootloader project
#
################################################################################
# \copyright
# $ Copyright 2020-2022 Cypress Semiconductor Apache2 $
################################################################################

include ../common.mk

################################################################################
# Basic Configuration
################################################################################

# Name of application (used to derive name of final linked file).
#
# If APPNAME is edited, ensure to update or regenerate launch
# configurations for your IDE.
APPNAME=bootloader

# If set to "true" or "1", display full command-lines when building.
VERBOSE=

# Build location.
CY_BUILD_LOCATION=./build/$(TRANSPORT_OPT)
CY_IGNORE=./build

################################################################################
# Advanced Configuration
################################################################################

# Enable optional code that is ordinarily disabled by default.
#
# Available components depend on the specific targeted hardware and firmware
# in use. In general, if you have
#
#    COMPONENTS=foo bar
#
# ... then code in directories named COMPONENT_foo and COMPONENT_bar will be
# added to the build
#
COMPONENTS=DFU_USER

# Like COMPONENTS, but disable optional code that was enabled by default.
DISABLE_COMPONENTS+=

# Core
CORE=CM0p
CORE_NAME=CM0p_0

# By default the build system automatically looks in the Makefile's directory
# tree for source code and builds it. The SOURCES variable can be used to
# manually add source code to the build process from a location not searched
# by default, or otherwise not found by the build system.
SOURCES=

# Like SOURCES, but for include directories. Value should be paths to
# directories (without a leading -I).
INCLUDES=

# Add additional defines to the build process (without a leading -D).
# App ID of Bootloader 
DEFINES=CY_APP_ID=0

# Select softfp or hardfp floating point. Default is softfp.
VFP_SELECT=

# Additional / custom C compiler flags.
#
# NOTE: Includes and defines should use the INCLUDES and DEFINES variable
# above.
CFLAGS=

# Additional / custom C++ compiler flags.
#
# NOTE: Includes and defines should use the INCLUDES and DEFINES variable
# above.
CXXFLAGS=

# Additional / custom assembler flags.
#
# NOTE: Includes and defines should use the INCLUDES and DEFINES variable
# above.
ASFLAGS=

# Additional / custom linker flags.
LDFLAGS=

# Additional / custom libraries to link in to the application.
LDLIBS=

# Path to the linker script to use (if empty, use the default linker script).
LINKER_SCRIPT=$(MTB_TOOLS__TARGET_DIR)/TOOLCHAIN_$(TOOLCHAIN)/dfu_cm0p_app0.$(MTB_RECIPE__SUFFIX_LS)

# Custom pre-build commands to run.
PREBUILD=

# Custom post-build commands to run.
POSTBUILD=


################################################################################
# Include additional configurations, source, headers and Macros.
################################################################################
ifneq ($(filter PSOC4000T,$(DEVICE_COMPONENTS)),) #CY8CKIT-040T and CY8CPROTO-040T supports only I2C as the transport
 ifeq ($(TRANSPORT_OPT), I2C)
  $(info Selected I2C transport for DFU.)
  DEFINES+=TRANSPORT_I2C
  COMPONENTS+=DFU_I2C

 else ifeq ($(TRANSPORT_OPT), UART)
  $(info Selected UART transport for DFU.)
  DEFINES+=TRANSPORT_UART
  COMPONENTS+=DFU_UART
 else
  $(error Unknown DFU transport selected !.)
 endif

else
 $(info Selected I2C transport for DFU.)
 TRANSPORT_OPT=I2C
 DEFINES+=TRANSPORT_I2C
 COMPONENTS+=DFU_I2C
endif


# Transport opt should be in lowercase to pick the correct files for the build. 
TRANSPORT_OPT_LOWER_CASE=$(shell echo $(TRANSPORT_OPT) | tr A-Z a-z)

################################################################################
# Paths
################################################################################

# Relative path to the project directory (default is the Makefile's directory).
#
# This controls where automatic source code discovery looks for code.
CY_APP_PATH=

# Relative path to the shared repo location.
#
# All .mtb files have the format, <URI>#<COMMIT>#<LOCATION>. If the <LOCATION> field 
# begins with $$ASSET_REPO$$, then the repo is deposited in the path specified by 
# the CY_GETLIBS_SHARED_PATH variable. The default location is one directory level 
# above the current app directory.
# This is used with CY_GETLIBS_SHARED_NAME variable, which specifies the directory name.
CY_GETLIBS_SHARED_PATH=../..

# Directory name of the shared repo location.
#
CY_GETLIBS_SHARED_NAME=mtb_shared

# Absolute path to the compiler's "bin" directory.
#
# The default depends on the selected TOOLCHAIN (GCC_ARM uses the ModusToolbox
# IDE provided compiler by default).
CY_COMPILER_PATH=

include $(CY_TOOLS_DIR)/make/start.mk
