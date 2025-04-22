/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2025-04-22 11:37:36
 * @LastEditTime: 2025-04-22 14:54:18
 * @Description:
 *
 * Copyright (c) 2025 by Tongji University, All Rights Reserved.
 */
#pragma once
#include <stdint.h>

/**
 * @brief test the execution time of table generation algorithm
 *
 * @param repeat : number of repetitions
 * @param table_gen : table generation algorithm for test
 * @return double : execution time in seconds
 */
double test_tablegen_time(int repeat, void (*table_gen)());

/**
 * @brief test functionality of encryption and decryption
 *
 * @param block_len : length of a block
 * @param enc : encryption algorithm
 * @param dec : decryption algorithm
 * @return int : 0 for fail and 1 for success
 */
int test_func(int block_len, void (*enc)(uint8_t *, uint8_t *), void (*dec)(uint8_t *, uint8_t *));

/**
 * @brief test the throughput of encryption algorithm
 *
 * @param block_len : length of a block
 * @param repeat : number of repetitions
 * @param enc : encryption algorithm
 * @return double : throughput (MB/s)
 */
double test_enc_throughput(int block_len, int repeat, void (*enc)(uint8_t *, uint8_t *));