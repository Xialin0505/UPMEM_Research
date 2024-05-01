DPU_DIR := dpu
HOST_DIR := host
HOSTREF_DIR := hostref
HOSTADVANCE_DIR := hostadvance
BUILDDIR ?= bin
NR_TASKLETS ?= 16
NR_DPUS ?= 5
PROBLEM_SIZE ?= 2

LIB_INCLUDE = -I../../library/src/backends/api/include/api
LDFLAGS=-lpthread -L./lib -ldpu -ldpujni -ldpuverbose -ldl

define conf_filename
	${BUILDDIR}/.NR_DPUS_$(1)_NR_TASKLETS_$(2).conf
endef
CONF := $(call conf_filename,${NR_DPUS},${NR_TASKLETS})

COMMON_INCLUDES := support
HOST_TARGET := ${BUILDDIR}/bs_host
HOSTREF_TARGET := ${BUILDDIR}/bs_hostref
HOSTADVANCE_TARGET := ${BUILDDIR}/bs_hostadvance
DPU_TARGET := ${BUILDDIR}/bs_dpu

HOST_SOURCES := $(wildcard ${HOST_DIR}/*.c)
HOSTREF_SOURCES := $(wildcard ${HOSTREF_DIR}/*.c)
HOSTADVANCE_SOURCES = $(wildcard ${HOSTADVANCE_DIR}/*.c)
DPU_SOURCES := $(wildcard ${DPU_DIR}/*.c)

.PHONY: all clean test

__dirs := $(shell mkdir -p ${BUILDDIR})

COMMON_FLAGS := -Wall -Wextra  -g -I${COMMON_INCLUDES}
HOST_FLAGS := ${COMMON_FLAGS} -std=c11 -O3 `dpu-pkg-config --cflags --libs dpu` -DNR_TASKLETS=${NR_TASKLETS} -DNR_DPUS=${NR_DPUS} -DPROBLEM_SIZE=${PROBLEM_SIZE}
DPU_FLAGS := ${COMMON_FLAGS} -O2 -DNR_TASKLETS=${NR_TASKLETS}

all: ${HOST_TARGET} ${DPU_TARGET} ${HOSTREF_TARGET} ${HOSTADVANCE_TARGET}

${CONF}:
	$(RM) $(call conf_filename,*,*)
	touch ${CONF}

${HOST_TARGET}: ${HOST_SOURCES} ${COMMON_INCLUDES} ${CONF}
	$(CC) -o $@ ${HOST_SOURCES} ${LIB_INCLUDE} ${LDFLAGS} ${HOST_FLAGS}

${HOSTREF_TARGET}: ${HOSTREF_SOURCES} ${COMMON_INCLUDES} ${CONF}
	$(CC) -o $@ ${HOSTREF_SOURCES} ${LIB_INCLUDE} ${HOST_FLAGS}

${HOSTADVANCE_TARGET}: ${HOSTADVANCE_SOURCES} ${COMMON_INCLUDES} ${CONF}
	$(CC) -o $@ ${HOSTADVANCE_SOURCES} ${LIB_INCLUDE} ${LDFLAGS} ${HOST_FLAGS}

${DPU_TARGET}: ${DPU_SOURCES} ${COMMON_INCLUDES} ${CONF}
	dpu-upmem-dpurte-clang ${DPU_FLAGS} -o $@ ${DPU_SOURCES}

clean:
	$(RM) -r $(BUILDDIR)

test: all
	./${HOST_TARGET} -i 262144