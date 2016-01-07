#!/bin/sh

rm -rf cp430_fw_updater/libxml2/
cd libxml2
echo "Entering directory libxml2"
make clean
./configure --prefix=$(pwd)/../cp430_fw_updater/libxml2/ --host=arm-none-linux-gnueabi --build=i686-pc-linux-gnu CC=arm-none-linux-gnueabi-gcc --without-python
make
make install
cd ..
echo "Exit directory libxml2"
