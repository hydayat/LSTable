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