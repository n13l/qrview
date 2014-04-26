TARGET=i686-w64-mingw32
CC=i686-w64-mingw32-gcc
CPP=i686-w64-mingw32-cpp
CXX=i686-w64-mingw32-g++
CFLAGS='-mthreads -mms-bitfields -L/usr/i686-w64-mingw32/sys-root/mingw/lib'
CXXFLAGS='-mthreads -mms-bitfields'
LDFLAGS='-static-libgcc'
#export PATH=/cygdrive/c/masm32/bin:$PATH
PATH=/usr/i686-w64-mingw32/sys-root/mingw/bin:/usr/i686-w64-mingw32/bin:/usr/bin:/usr/lib/gcc/i686-w64-mingw32/4.8.2:$PATH
PKG_CONFIG_PATH=/usr/i686-w64-mingw32/sys-root/mingw/lib/pkgconfig
export CC CPP CXX CFLAGS CXXFLAGS PATH PKG_CONFIG_PATH LDFLAGS TARGET
