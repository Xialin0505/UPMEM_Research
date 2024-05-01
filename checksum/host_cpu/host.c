/*
 * Copyright (c) 2014-2019 - UPMEM
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file host.c
 * @brief Template for a Host Application Source File.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <x86intrin.h>
#include "timer.h"

#include "common.h"

static uint8_t test_file[BUFFER_SIZE];

uint8_t* global_data;
size_t length = BUFFER_SIZE;

/**
 * @brief creates a "test file"
 *
 * @return the theorical checksum value
 */
static uint32_t create_test_file()
{
    uint32_t checksum = 0;
    srand(0);
    for (unsigned int i = 0; i < BUFFER_SIZE; i++) {
        test_file[i] = (unsigned char)(rand());
        checksum += test_file[i];
    }
    //global_data = test_file;
    return checksum;
}

/**
 * @brief Main of the Host Application.
 */
int main()
{
   
	uint64_t start = _rdtsc();
    create_test_file();
	create_test_file();
	create_test_file();
	uint64_t end = _rdtsc();
	double cycles = (double)(end - start);
	printf("Total CPU cycles : %g\n", cycles);
    return 0;
}
