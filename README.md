## UPMEM Research

#### Build the Library
```
cd /libaray/src/backends/
mkdir build-dir
cd /libaray/src/backends/build-dir
cmake ..
make
```

It should build `libdpu.so.0.0` and `libdpujni.so.0.0`, which should be enough for our project.

#### Project code
Our project code are located in `./library` folder, and mainly the following files are modified
`library/backends/api/include/api/dpu.h`
`library/backends/api/src/api/dpu_runner.c`

The benchmarks are located in the remaining folders
`binarysearch`
`checksum`
`hashtable`
`usecase_keccakf`

#### How to run benchmarks
Navigate to either benchmark folder, then 
```
bash link.sh
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH;$PWD/lib"
bash test_advance.sh
bash test_ref.sh
bash test.sh
```
We notice that for when we remove the linking step for the reference, the reference will run much faster than without linking. The linking to our custom library cause a huge slow down of the entire program.

