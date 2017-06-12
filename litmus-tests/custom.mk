# This file is included at the end of the copied Makefile.  If you have some
# things you want to change about the Makefile, it is best to do it here.

NVCC_FLAGS     +=
NVCC_LINK      +=

# required compiler flags
# for example, include directories
#   CC_REQUIRED += -I<path>
# or defines
#   CC_REQUIRED += -DDEBUG_ENABLED=1
CC_REQUIRED    += -DWFT=float -DSCH_LIB

# required linker flags
# for example, link libraries
#   LD_REQUIRED += -L<library-path> -l<library-name>
# or rpath
#   LD_REQUIRED += -Wl,-rpath=<abs-path-to-library-dir>
LD_REQUIRED    +=