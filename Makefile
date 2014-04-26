PREFIX?=/usr/local
s?=.
CONFIG_INSTALL_API=1
CONFIG_LOCAL=1

include $(s)/build/top
include $(s)/build/help

ifeq (${USE_CLANG_VIM_AUTOCOMPLETE},1)
CC=${HOME}/.vim/bin/cc_args.py clang
CXX=${HOME}/.vim/bin/cc_args.py clang
endif

include $(s)/build/config

# Build libraries
#include $(s)/src/Makefile

include $(s)/tools/Makefile

include $(s)/build/rules
include $(s)/build/bottom
