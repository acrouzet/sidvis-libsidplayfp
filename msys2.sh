#!/usr/bin/bash
pacman -S mingw-w64-ucrt-x86_64-autotools
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-xa65
autoreconf -i
./configure --enable-hardsid
make
make install
cd ..
git clone https://github.com/acrouzet/sidvis-sidplayfp.git
cd sidvis-sidplayfp
autoreconf -i
./configure
make
make install
