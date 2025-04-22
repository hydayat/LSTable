/*
 * @Author: Yimin Li 2111289@tongji.edu.com
 * @Date: 2025-04-22 12:27:25
 * @LastEditTime: 2025-04-22 14:27:19
 * @Description:
 *
 * Copyright (c) 2025 by Tongji University, All Rights Reserved.
 */
#pragma once
#include <stdint.h>

/**
 * @description: generate the lookup table of PC16
 * @return {*}
 */
void pc16_table_gen();

/**
 * @description: encrypt in using LSTable-(8,R) and store the result in out
 * @param {unsigned int*} in: plaintext
 * @param {unsigned int*} out: ciphertext
 * @return {*}
 */
void pc16_encrypt(uint8_t *in, uint8_t *out);