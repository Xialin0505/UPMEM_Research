CFLAGS+=-Wall
NR_TASKLETS ?= 16
NR_DPUS ?= 1
DPU_FLAGS := -O2 -DNR_TASKLETS=${NR_TASKLETS}
COMMON_FLAGS := -Wall -g
HOST_FLAGS := ${COMMON_FLAGS} -std=c11 -O3 `dpu-pkg-config --cflags --libs dpu` -DNR_TASKLETS=${NR_TASKLETS} -DNR_DPUS=${NR_DPUS}

all: newlib.so

newlib.o: newlib.c
	${CC} ${HOST_FLAGS} -Wall -fPIC -DPIC -c newlib.c -L../lib

newlib.so: newlib.o
	ld -shared -o newlib.so newlib.o -ldl

clean:
	rm *.o *.so