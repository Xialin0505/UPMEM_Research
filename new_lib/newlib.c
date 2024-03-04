 #include <limits.h>
 #include <stdbool.h>
 #include <stddef.h>
 #include <stdint.h>
 #include <stdio.h>
 #include <stdlib.h>
 
#include <dpu.h>

  
 dpu_error_t
 my_dpu_prepare_xfer(struct dpu_set_t dpu_set, void *buffer) {
    printf("my lib\n");
    return DPU_OK;
}
