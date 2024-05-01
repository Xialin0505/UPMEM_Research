/**
* app.c
* BS Host Application Source File
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dpu.h>
#include <dpu_log.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <time.h>
#include <x86intrin.h>

#if ENERGY
#include <dpu_probe.h>
#endif

#include "params.h"
#include "timer.h"
#include <dpu_program.h>

// Define the DPU Binary path as DPU_BINARY here
#define DPU_BINARY "./bin/bs_dpu"

DTYPE* global_input;
DTYPE* global_query;
DTYPE global_input_size;
uint64_t number_query;
dpu_results_t* results_retrieve[NR_DPUS];
int* abortInfo;
struct dpu_set_t dpu_set, dpu;
 
// Create input arrays
void create_test_file(DTYPE * input, DTYPE * querys, uint64_t  nr_elements, uint64_t nr_querys) {

	input[0] = 0;
	for (uint64_t i = 1; i < nr_elements; i++) {
		input[i] = input[i - 1] + 1;
	}

	uint64_t interval = nr_querys / 3;
	for (uint64_t i = 0; i < interval; i++) {
		querys[i] = 1;
	}
    for (uint64_t i = interval; i < interval * 2; i++) {
        querys[i] = nr_elements - 1;
    }

    for (uint64_t i = interval * 2; i < nr_querys; i++) {
        querys[i] = i;
    }

}

// Compute output in the host
int64_t binarySearch(DTYPE * input, DTYPE * querys, DTYPE input_size, uint64_t num_querys)
{
	uint64_t result = -1;
	DTYPE r;
	for(uint64_t q = 0; q < num_querys; q++)
	{
		DTYPE l = 0;
		r = input_size;
		while (l <= r) {
			DTYPE m = l + (r - l) / 2;

			// Check if x is present at mid
			if (input[m] == querys[q])
			result = m;

			// If x greater, ignore left half
			if (input[m] < querys[q])
			l = m + 1;

			// If x is smaller, ignore right half
			else
			r = m - 1;
		}
	}
	return result;
}

void binarySearchHost(uint32_t dpu_index)
{
	printf("restart dpu %d job\n", dpu_index);
	
	DTYPE r;
	uint64_t slice = number_query / NR_DPUS;
	uint64_t start = dpu_index * slice;
	uint64_t interval = slice / NR_TASKLETS;
	
	for (int i = 0; i < NR_TASKLETS; i++) {
		
		for (uint64_t q = 0; q < interval; q++) {
			
			uint64_t idx = start + i * interval + q;
			uint64_t result = -1;
			DTYPE l = 0;
			r = global_input_size;
			while (l <= r) {
				DTYPE m = l + (r - l) / 2;

				// Check if x is present at mid
				if (global_input[m] == global_query[idx])
				result = m;

				// If x greater, ignore left half
				if (global_input[m] < global_query[idx])
				l = m + 1;

				// If x is smaller, ignore right half
				else
				r = m - 1;
			}
			results_retrieve[dpu_index][i].found = result;
			// printf("dpu %d, tasklet %d, found %d\n", dpu_index, i, result);
		}
	}
}

void host_func_resume() {
	for (int dpu_index = 0; dpu_index < NR_DPUS; dpu_index++) {
		if (!abortInfo[dpu_index]) {
			continue;
		}

		for (int i = 0; i < NR_TASKLETS; i++) {
			DTYPE r;
			uint64_t slice = number_query / NR_DPUS;
			uint64_t start = dpu_index * slice;
			uint64_t interval = slice / NR_TASKLETS;


			uint64_t q = results_retrieve[dpu_index][i].idx;
			// printf("dpu %d, tasklet %d, q %d\n", dpu_index, i, q);

			for (; q < interval; q++) {
				
				uint64_t idx = start + i * interval + q;
				uint64_t result = -1;
				DTYPE l = 0;
				r = global_input_size;
				while (l <= r) {
					DTYPE m = l + (r - l) / 2;

					// Check if x is present at mid
					if (global_input[m] == global_query[idx])
					result = m;

					// If x greater, ignore left half
					if (global_input[m] < global_query[idx])
					l = m + 1;

					// If x is smaller, ignore right half
					else
					r = m - 1;
				}
				results_retrieve[dpu_index][i].found = result;
				// printf("dpu %d, tasklet %d, found %d\n", dpu_index, i, result);
			}
		}
	}
}


// Main of the Host Application
int main(int argc, char **argv) {

	struct Params p = input_params(argc, argv);
	uint32_t nr_of_dpus;
	uint64_t input_size = INPUT_SIZE;
	uint64_t num_querys = QUERY_SIZE;
	DTYPE result_host = -1;
	DTYPE result_dpu  = -1;

	// Create the timer
	Timer timer;

	// Allocate DPUs and load binary
	DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &dpu_set));
	DPU_ASSERT(dpu_load(dpu_set, DPU_BINARY, NULL));
	DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &nr_of_dpus));
	int rank_num;
	dpu_get_nr_ranks(dpu_set, &rank_num);
	abortInfo = malloc(rank_num * sizeof(int));

	#if ENERGY
	struct dpu_probe_t probe;
	DPU_ASSERT(dpu_probe_init("energy_probe", &probe));
	#endif

	// Query number adjustement for proper partitioning
	if(num_querys % (nr_of_dpus * NR_TASKLETS))
	num_querys = num_querys + (nr_of_dpus * NR_TASKLETS - num_querys % (nr_of_dpus * NR_TASKLETS));

	assert(num_querys % (nr_of_dpus * NR_TASKLETS) == 0 && "Input dimension");    // Allocate input and querys vectors

	DTYPE * input  = malloc((input_size) * sizeof(DTYPE));
	DTYPE * querys = malloc((num_querys) * sizeof(DTYPE));
	number_query = num_querys;

	// Create an input file with arbitrary data
	create_test_file(input, querys, input_size, num_querys);
	global_input = input;
	global_query = querys;
	global_input_size = input_size;


	// Compute host solution
	start(&timer, 0, 0);
	uint64_t startc = _rdtsc();
	result_host = binarySearch(input, querys, input_size - 1, num_querys);
	uint64_t endc = _rdtsc();
	stop(&timer, 0);

	double cycles = (double)(endc - startc);
	printf("CPU version CPU cycles : %g\n", cycles);

	int status = 0;
	// Create kernel arguments
	uint64_t slice_per_dpu          = num_querys / nr_of_dpus;
	dpu_arguments_t input_arguments = {input_size, slice_per_dpu, 0};

	for (unsigned int rep = 0; rep < p.n_warmup + p.n_reps; rep++) {
		// Perform input transfers
		uint64_t i = 0;

		//if (rep >= p.n_warmup)
		start(&timer, 1, rep - p.n_warmup);

		DPU_FOREACH(dpu_set, dpu, i)
		{
			DPU_ASSERT(dpu_prepare_xfer(dpu, &input_arguments));
		}

		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "DPU_INPUT_ARGUMENTS", 0, sizeof(input_arguments), DPU_XFER_DEFAULT));

		i = 0;

		DPU_FOREACH(dpu_set, dpu, i)
		{
			DPU_ASSERT(dpu_prepare_xfer(dpu, input));
		}

		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, input_size * sizeof(DTYPE), DPU_XFER_DEFAULT));

		i = 0;

		DPU_FOREACH(dpu_set, dpu, i)
		{
			DPU_ASSERT(dpu_prepare_xfer(dpu, querys + slice_per_dpu * i));
		}

		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, input_size * sizeof(DTYPE), slice_per_dpu * sizeof(DTYPE), DPU_XFER_DEFAULT));

		//if (rep >= p.n_warmup)
		stop(&timer, 1);

		// Run kernel on DPUs
		// if (rep >= p.n_warmup)
		// {
			start(&timer, 2, rep - p.n_warmup);
			#if ENERGY
			DPU_ASSERT(dpu_probe_start(&probe));
			#endif
		//}

		i = 0;
		DPU_FOREACH(dpu_set, dpu, i)
		{
			results_retrieve[i] = (dpu_results_t*)malloc(NR_TASKLETS * sizeof(dpu_results_t));
		}

		// DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
		DPU_ASSERT(dpu_launch_preempt(dpu_set, DPU_SYNCHRONOUS, abortInfo));

		// if (rep >= p.n_warmup)
		// {
			stop(&timer, 2);
			#if ENERGY
			DPU_ASSERT(dpu_probe_stop(&probe));
			#endif
		//}
		// Print logs if required
		#if PRINT
		unsigned int each_dpu = 0;
		printf("Display DPU Logs\n");
		DPU_FOREACH(dpu_set, dpu)
		{
			printf("DPU#%d:\n", each_dpu);
			DPU_ASSERT(dpulog_read_for_dpu(dpu.dpu, stdout));
			each_dpu++;
		}
		#endif

		// Retrieve results
		//if (rep >= p.n_warmup)
		start(&timer, 3, rep - p.n_warmup);
		i = 0;
		DPU_FOREACH(dpu_set, dpu, i)
		{
			//if (!abortInfo[i]) {
            	DPU_ASSERT(dpu_prepare_xfer(dpu, results_retrieve[i]));
        	//}
		}

		DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, "DPU_RESULTS", 0, NR_TASKLETS * sizeof(dpu_results_t), DPU_XFER_DEFAULT));

		startc = _rdtsc();
	    host_func_resume();
	    endc = _rdtsc();

	    cycles = (double)(endc - startc);
	    printf("Resume Function CPU cycles : %g\n", cycles);

		DPU_FOREACH(dpu_set, dpu, i)
		{
			for(unsigned int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++)
			{
				result_dpu = results_retrieve[i][each_tasklet].found;
				// printf("dpu %d, tasklet %d, result dpu %d, result host %d\n", i, each_tasklet, result_dpu, result_host);
				status = (result_dpu == result_host);
				if (status) {
					break;
				}
			}
			free(results_retrieve[i]);
		}
		//if(rep >= p.n_warmup)
		stop(&timer, 3);
	}
	// Print timing results
	printf("CPU Version Time (ms): ");
	print(&timer, 0, p.n_warmup);
	printf("CPU-DPU Time (ms): ");
	print(&timer, 1, p.n_warmup);
	printf("DPU Kernel Time (ms): ");
	print(&timer, 2, p.n_warmup);
	printf("DPU-CPU Time (ms): ");
	print(&timer, 3, p.n_warmup);
	printf("\nEnd to End Time (ms): %f\n",(timer.time[1] + timer.time[2] + timer.time[3]) / (1000 * p.n_warmup) );

	#if ENERGY
	double energy;
	DPU_ASSERT(dpu_probe_get(&probe, DPU_ENERGY, DPU_AVERAGE, &energy));
	printf("DPU Energy (J): %f\t", energy * num_iterations);
	#endif

	if (status) {
		printf("[" ANSI_COLOR_GREEN "OK" ANSI_COLOR_RESET "] results are equal\n");
	} else {
		printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "] results differ!\n");
	}

	free(input);
	free(abortInfo);
	DPU_ASSERT(dpu_free(dpu_set));

	return status ? 0 : 1;
}