
# Component Makefile

# This Makefile can be left empty. By default, it will take the sources in the
# src/ directory, compile them and link them into lib(subdirectory_name).a
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.

# add app path
COMPONENT_ADD_INCLUDEDIRS := ./include
COMPONENT_SRCDIRS := ./src

CFLAGS += -Wno-implicit-fallthrough
# COMPONENT_ADD_LDFRAGMENTS += component_file.lf