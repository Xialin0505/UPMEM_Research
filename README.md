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

If reports "cannot open shared object file: No such file or directory"
Manually export the library

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/my_library/

replace my library with the absolute path of /lib/ folder under one of the four benchmarks

# Run the use case

## Use case binary search
```
cd /binarysearch/
```
### lazy preemption (CPU baseline contained in lazy preemption)
```
./test.sh
```
### lazy preemption with advance mode
```
./test_advance.sh
```
### PIM baseline
```
./test_ref.sh
```
## Use case keccaf
```
cd /usecase_keccakf/
```
### lazy preemption
```
make run
```
### lazy preemption with advance mode
```
make advance
```
### CPU baseline
```
make cpu
```
### dpu baseline
```
cd /usecase_keccakf-base/
make run
```
## Use case checksum
```
cd /checksum/
```
### lazy preemption
```
./test.sh
```
### CPU baseline
```
./host_cpu/host
```
### lazy preemption with advance mode
```
./test_advance.sh
```
### PIM baseline
```
./test_ref.sh
```
## Use case Hashtable
```
cd /hashtable/
```
### lazy preemption
```
./test.sh
```
### CPU baseline
```
./test_cpu.sh
```
### lazy preemption with advance mode
```
./test_advance.sh
```
### PIM baseline
```
./test_ref.sh
```
