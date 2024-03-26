#!/bin/bash

cp ~/research/libaray/src/backends/build-dir/api/libdpu.so.0.0 ./lib 
cp ~/research/libaray/src/backends/build-dir/api/libdpujni.so.0.0 ./lib
cp ~/research/libaray/src/backends/build-dir/api/libdpu.so ./lib
cp /lib/libdpufsim.so ./lib/
# cp /lib/libdpuverbose.so.2023.2 ./lib/
# mv ./lib/libdpu.so.0.0 ./lib/libdpu.so.2023.2
# mv ./lib/libdpujni.so.0.0 ./lib/libdpujni.so.2023.2

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH;$PWD/lib"