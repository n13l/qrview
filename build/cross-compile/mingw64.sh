TARGET=x86_64-w64-mingw32
CC=x86_64-w64-mingw32-gcc
CPP=x86_64-w64-mingw32-cpp
CXX=x86_64-w64-mingw32-g++
CFLAGS='-mthreads -mms-bitfields -L/usr/x86_64-w64-mingw32/sys-root/mingw/lib'
CXXFLAGS='-mthreads -mms-bitfields'
LDFLAGS='-static-libgcc'
#export PATH=/cygdrive/c/masm32/bin:$PATH
PATH=/usr/x86_64-w64-mingw32/sys-root/mingw/bin:/usr/x86_64-w64-mingw32/bin:/usr/bin:/usr/lib/gcc/x86_64-w64-mingw32/4.8.2:$PATH
PKG_CONFIG_PATH=/usr/x86_64-w64-mingw32/sys-root/mingw/lib/pkgconfig
export CC CPP CXX CFLAGS CXXFLAGS PATH PKG_CONFIG_PATH LDFLAGS TARGET
