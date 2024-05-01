DPU_DIR := dpu
HOST_DIR := host
HOSTREF_DIR := host_ref
HOSTADVANCE_DIR := host_advance
CPU_DIR := host_cpu
BUILDDIR ?= build
NR_TASKLETS ?= 8
NR_DPUS ?= 5

LIB_INCLUDE = -I../../library/src/backends/api/include/api
LDFLAGS=-lpthread -L./lib -ldpu -ldpujni -ldpuverbose -ldl

define conf_filename
	${BUILDDIR}/.NR_DPUS_$(1)_NR_TASKLETS_$(2).conf
endef
CONF := $(call conf_filename,${NR_DPUS},${NR_TASKLETS})

HOST_PYTHON_SCRIPT := ${HOST_DIR}/host.py
HOST_TARGET := ${BUILDDIR}/hashtable_host
HOSTREF_TARGET := ${BUILDDIR}/hashtable_hostref
HOSTADVANCE_TARGET := ${BUILDDIR}/hashtable_host_advance
CPU_TARGET := ${BUILDDIR}/hashtable_cpu
DPU_TARGET := ${BUILDDIR}/hashtable_dpu

COMMON_INCLUDES := common
HOST_SOURCES := $(wildcard ${HOST_DIR}/*.c)
HOSTREF_SOURCES := $(wildcard ${HOSTREF_DIR}/*.c)
HOSTADVANCE_SOURCES = $(wildcard ${HOSTADVANCE_DIR}/*.c)
CPU_SOURCES = $(wildcard ${CPU_DIR}/*.c)
DPU_SOURCES := $(wildcard ${DPU_DIR}/*.c)

.PHONY: all clean test

__dirs := $(shell mkdir -p ${BUILDDIR})

COMMON_FLAGS := -Wall -Wextra -g -I${COMMON_INCLUDES}
HOST_FLAGS := ${COMMON_FLAGS} -std=gnu11 -O3 `dpu-pkg-config --cflags --libs dpu` -DNR_TASKLETS=${NR_TASKLETS} -DNR_DPUS=${NR_DPUS}
DPU_FLAGS := ${COMMON_FLAGS} -O2 -DNR_TASKLETS=${NR_TASKLETS}

all: ${HOST_TARGET} ${DPU_TARGET} ${HOSTREF_TARGET} ${HOSTADVANCE_TARGET} ${CPU_TARGET}

${CONF}:
	$(RM) $(call conf_filename,*,*)
	touch ${CONF}

${HOST_TARGET}: ${HOST_SOURCES} ${COMMON_INCLUDES} ${CONF}
	$(CC) -o $@ ${HOST_SOURCES} ${LIB_INCLUDE} ${LDFLAGS} ${HOST_FLAGS}

${HOSTREF_TARGET}: ${HOSTREF_SOURCES} ${COMMON_INCLUDES} ${CONF}
	$(CC) -o $@ ${HOSTREF_SOURCES} ${LIB_INCLUDE} ${HOST_FLAGS}

${HOSTADVANCE_TARGET}: ${HOSTADVANCE_SOURCES} ${COMMON_INCLUDES} ${CONF}
	$(CC) -o $@ ${HOSTADVANCE_SOURCES} ${LIB_INCLUDE} ${LDFLAGS} ${HOST_FLAGS}

${CPU_TARGET}: ${CPU_SOURCES} ${COMMON_INCLUDES} ${CONF}
	$(CC) -o $@ ${CPU_SOURCES} ${LIB_INCLUDE} ${LDFLAGS} ${HOST_FLAGS}

${DPU_TARGET}: ${DPU_SOURCES} ${COMMON_INCLUDES} ${CONF}
	dpu-upmem-dpurte-clang ${DPU_FLAGS} -o $@ ${DPU_SOURCES}

clean:
	$(RM) -r $(BUILDDIR)

test_c: ${HOST_TARGET} ${DPU_TARGET}
	./${HOST_TARGET}

# test_python: ${DPU_TARGET}
# 	python3 ${HOST_PYTHON_SCRIPT} ${NR_DPUS} ${NR_TASKLETS}

test: test_c #test_python