DPU_DIR := dpu
HOST_DIR := host
BUILDDIR ?= build
NR_TASKLETS ?= 16
NR_DPUS ?= 5
# HOST_UPMEM_SOURCE = $(filter-out ../../libaray/src/backends/api/src/api/dpu_python_wrappers.c \
# 								../../libaray/src/backends/api/src/api/dpu_jni.c, \
# 					$(wildcard ../../libaray/src/backends/api/src/api/*.c, \
# 								../../libaray/src/backends/api/src/*.c))
# HOST_UPMEM_SOURCE = ../../libaray/src/backends/api/src/api/dpu_runner.c \
					../../libaray/src/backends/api/src/api/dpu_thread_job.c \
					../../libaray/src/backends/api/src/api/dpu_polling.c

# LIB_INCLUDE = 	-I../../libaray/src/backends/api/include/api \
# 				-I ../../libaray/src/backends/api/src/api/include \
# 				-I ../../libaray/src/backends/api/src/include \
# 				-I../../libaray/src/backends/api/src/api \
# 				-I../../libaray/src/backends/commons/include \
# 				-I../../libaray/src/backends/commons/src/properties \
# 				-I../../libaray/src/backends/verbose/src \
# 				-I../../libaray/src/backends/build-dir/gen/include

# LIB_INCLUDE = 	-I../../libaray/src/backends/api/include/api \
# 				-I../../libaray/src/backends/api/include/lowlevel \
# 				-I../../libaray/src/backends/api/src/include \
# 				-I../../libaray/src/backends/verbose/src \
# 				-I../../libaray/src/backends/commons/include \
# 				-I../../libaray/src/backends/commons/src \
# 				-I../../libaray/src/backends/commons/src/properties \
# 				-I../../libaray/src/backends/ufi/include/ufi \
# 				-I../../libaray/src/backends/ufi \
# 				-I../../libaray/src/backends/build-dir/gen/include \
# 				-I../../libaray/src/backends/api/src/api/include \
# 				-I../../libaray/src/backends/ufi/include


LIB_INCLUDE = -I../../libaray/src/backends/api/include/api
LDFLAGS=-lpthread -L./lib -ldpu -ldpujni -ldpuverbose -ldl

define conf_filename
	${BUILDDIR}/.NR_DPUS_$(1)_NR_TASKLETS_$(2).conf
endef
CONF := $(call conf_filename,${NR_DPUS},${NR_TASKLETS})

HOST_PYTHON_SCRIPT := ${HOST_DIR}/host.py
HOST_TARGET := ${BUILDDIR}/checksum_host
DPU_TARGET := ${BUILDDIR}/checksum_dpu

COMMON_INCLUDES := common
HOST_SOURCES := $(wildcard ${HOST_DIR}/*.c)
DPU_SOURCES := $(wildcard ${DPU_DIR}/*.c)

.PHONY: all clean test

__dirs := $(shell mkdir -p ${BUILDDIR})

COMMON_FLAGS := -Wall -Wextra -g -I${COMMON_INCLUDES}
HOST_FLAGS := ${COMMON_FLAGS} -std=gnu11 -O3 `dpu-pkg-config --cflags --libs dpu` -DNR_TASKLETS=${NR_TASKLETS} -DNR_DPUS=${NR_DPUS}
DPU_FLAGS := ${COMMON_FLAGS} -O2 -DNR_TASKLETS=${NR_TASKLETS}

all: ${HOST_TARGET} ${DPU_TARGET}

${CONF}:
	$(RM) $(call conf_filename,*,*)
	touch ${CONF}

${HOST_TARGET}: ${HOST_SOURCES} ${COMMON_INCLUDES} ${CONF}
	$(CC) -o $@ ${HOST_SOURCES} ${HOST_UPMEM_SOURCE} ${LIB_INCLUDE} ${LDFLAGS} ${HOST_FLAGS} -v

${DPU_TARGET}: ${DPU_SOURCES} ${COMMON_INCLUDES} ${CONF}
	dpu-upmem-dpurte-clang ${DPU_FLAGS} -o $@ ${DPU_SOURCES}

clean:
	$(RM) -r $(BUILDDIR)

test_c: ${HOST_TARGET} ${DPU_TARGET}
	./${HOST_TARGET}

test_python: ${DPU_TARGET}
	python3 ${HOST_PYTHON_SCRIPT} ${NR_DPUS} ${NR_TASKLETS}

test: test_c test_python

