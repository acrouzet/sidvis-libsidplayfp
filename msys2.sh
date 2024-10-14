#!/usr/bin/bash
pacman -S mingw-w64-ucrt-x86_64-autotools
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-xa65
autoreconf -i
./configure --enable-hardsid
make
make install
git clone https://github.com/acrouzet/sidvis-sidplayfp.git
cd sidplayfp
autoreconf -i
./configure
make
make install
