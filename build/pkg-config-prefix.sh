#!/bin/bash
find ./pkgconfig/ -type f -exec sed -i '1 s/^.*$/prefix\=\/usr\/i686-w64-mingw32\/build/g' {} \; -print
