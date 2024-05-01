#ifndef __COMMON_H__
#define __COMMON_H__

#define XSTR(x) STR(x)
#define STR(x) #x

/* DPU variable that will be read of write by the host */
#define DPU_BUFFER dpu_mram_buffer
#define DPU_CACHES dpu_wram_caches
#define DPU_RESULTS dpu_wram_results

/* Size of the buffer on which the checksum will be performed */
#define BUFFER_SIZE (8 << 20)

/* Use blocks of 256 bytes */
#define BLOCK_SIZE (256)

/* Structure used by both the host and the dpu to communicate information */

#include <stdint.h>

#endif /* __COMMON_H__ */