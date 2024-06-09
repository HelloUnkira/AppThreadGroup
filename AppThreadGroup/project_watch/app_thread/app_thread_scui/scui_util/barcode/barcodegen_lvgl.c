/*
 * Copyright (c) 2022 Actions Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "barcodegen_lvgl.h"

/*---- Private tables of constants ----*/

#define BARCODE128C_BITS			(11)
#define BARCODE128C_BITS_STOP		(13)

// For control codec index.
enum barcode128c_idx {
	IDX_CODE_B = 100 ,  // Code B 100
	IDX_CODE_A = 101 ,  // Code A 101
	IDX_FNC1 = 102 ,  // FNC 1 102
	IDX_START_A = 103 ,  // Start Code A 103
	IDX_START_B = 104 ,  // Start Code B 104
	IDX_START_C = 105 ,  // Start Code C 105
	IDX_STOP = 106 ,  // Stop 106
	IDX_REV_STOP = 107 ,  // Reverse Stop 107
	IDX_END_STOP = 108 ,  // Stop pattern (7 bars/spaces) 108
};

// For generating bar codes.
static const uint16_t barcode128c_table[] = {
	0x06CC, // 00
	0x066C, // 01
	0x0666, // 02
	0x0498, // 03
	0x048C, // 04
	0x044C, // 05
	0x04C8, // 06
	0x04C4, // 07
	0x0464, // 08
	0x0648, // 09
	0x0644, // 10
	0x0624, // 11
	0x059C, // 12
	0x04DC, // 13
	0x04CE, // 14
	0x05CC, // 15
	0x04EC, // 16
	0x04E6, // 17
	0x0672, // 18
	0x065C, // 19
	0x064E, // 20
	0x06E4, // 21
	0x0674, // 22
	0x076E, // 23
	0x074C, // 24
	0x072C, // 25
	0x0726, // 26
	0x0764, // 27
	0x0734, // 28
	0x0732, // 29
	0x06D8, // 30
	0x06C6, // 31
	0x0636, // 32
	0x0518, // 33
	0x0458, // 34
	0x0446, // 35
	0x0588, // 36
	0x0468, // 37
	0x0462, // 38
	0x0688, // 39
	0x0628, // 40
	0x0622, // 41
	0x05B8, // 42
	0x058E, // 43
	0x046E, // 44
	0x05D8, // 45
	0x05C6, // 46
	0x0476, // 47
	0x0776, // 48
	0x068E, // 49
	0x062E, // 50
	0x06E8, // 51
	0x06E2, // 52
	0x06EE, // 53
	0x0758, // 54
	0x0746, // 55
	0x0716, // 56
	0x0768, // 57
	0x0762, // 58
	0x071A, // 59
	0x077A, // 60
	0x0642, // 61
	0x078A, // 62
	0x0530, // 63
	0x050C, // 64
	0x04B0, // 65
	0x0486, // 66
	0x042C, // 67
	0x0426, // 68
	0x0590, // 69
	0x0584, // 70
	0x04D0, // 71
	0x04C2, // 72
	0x0434, // 73
	0x0432, // 74
	0x0612, // 75
	0x0650, // 76
	0x07BA, // 77
	0x0614, // 78
	0x047A, // 79
	0x053C, // 80
	0x04BC, // 81
	0x049E, // 82
	0x05E4, // 83
	0x04F4, // 84
	0x04F2, // 85
	0x07A4, // 86
	0x0794, // 87
	0x0792, // 88
	0x06DE, // 89
	0x06F6, // 90
	0x07B6, // 91
	0x0578, // 92
	0x051E, // 93
	0x045E, // 94
	0x05E8, // 95
	0x05E2, // 96
	0x07A8, // 97
	0x07A2, // 98
	0x05DE, // 99
	0x05EE, // Code B 100
	0x075E, // Code A 101
	0x07AE, // FNC 1 102
	0x0684, // Start Code A 103
	0x0690, // Start Code B 104
	0x069C, // Start Code C 105
	0x063A, // Stop 106
	0x06B8, // Reverse Stop 107
	0x18EB, // Stop pattern (7 bars/spaces) 108
};

/*---- Low-level Bar Code encoding functions ----*/

static bool barcodegen_lvgl_is_num(uint8_t buf[], size_t len)
{
	char ch;

	while (len-- > 0) {
		ch = (char)*buf++;
		if (ch < '0' || ch > '9')
			return false;
	}
	return true;
}

bool barcodegen_lvgl_encode(uint8_t buf[], size_t buf_len, uint16_t code_buf[], size_t *code_num)
{
	uint32_t i, j, idx;
	uint32_t chksum = 0;
	size_t even_len = (buf_len / 2) * 2;
	size_t odd_len = (buf_len % 2);

	// encode start
	code_buf[0] = barcode128c_table[IDX_START_C];
	chksum += IDX_START_C;

	// encode even numbers
	for(i = 0, j = 1; i < even_len; i += 2, j++) {
		idx = (buf[i] - '0') * 10 + (buf[i + 1] - '0');
		code_buf[j] = barcode128c_table[idx];
		chksum += j * idx;
	}

	// encode last odd number
	if (odd_len > 0) {
		code_buf[j] = barcode128c_table[IDX_CODE_A];
		chksum += j * IDX_CODE_A;
		j ++;

		idx = (buf[i] - '0' + 16); // CODE A
		code_buf[j] = barcode128c_table[idx];
		chksum += j * idx;
		j ++;
	}

	// encode checksum
	chksum %= 103;
	code_buf[j++] = barcode128c_table[chksum];

	// encode stop
	code_buf[j++] = barcode128c_table[IDX_END_STOP];

	// save code num
	*code_num = j;

	return true;
}

bool barcodegen_lvgl_merge(uint16_t code_buf[], size_t code_num, uint8_t barcode[], size_t *bits)
{
	size_t i, j;
	uint16_t val;
	uint8_t dat_bit, cpy_bit = 8;

	for (i = 0, j = 0; i < code_num; i ++) {
		// init data
		val = code_buf[i];
		dat_bit = (i == (code_num - 1)) ? BARCODE128C_BITS_STOP : BARCODE128C_BITS;
		if (cpy_bit != 8) {
			cpy_bit = 8 - cpy_bit;
		}

		// merge bits
		if (cpy_bit == 8) {
			barcode[j] = 0;
		}
		barcode[j++] |= (uint8_t)(val >> (dat_bit - cpy_bit));
		cpy_bit = dat_bit - cpy_bit;
		if (cpy_bit > 8) {
			barcode[j++] = (uint8_t)(val >> (cpy_bit - 8));
			cpy_bit = cpy_bit - 8;
		}
		barcode[j] = (uint8_t)((val << (sizeof(val) * 8 - cpy_bit)) >> 8);
		if (cpy_bit == 8) {
			j ++;
		}
	}	

	// save code bits
	*bits = (code_num - 1) * BARCODE128C_BITS + BARCODE128C_BITS_STOP;

	return true;
}


/*---- High-level Bar Code encoding functions ----*/

// Public function - see documentation comment in header file.
bool barcodegen_lvgl_encodeBinary(uint8_t dataAndTemp[], size_t dataLen, uint8_t barcode[],
	enum barcodegen_lvgl_Type type, size_t *bitsLen)
{
	uint32_t code_num;

	// check type
	if ((type != barcodegen_lvgl_128C) || (!bitsLen)) {
		return false;
	}

	// check numeric
	if (!barcodegen_lvgl_is_num(dataAndTemp, dataLen)) {
		return false;
	}

	// encode barcode 128c
	if (!barcodegen_lvgl_encode(dataAndTemp, dataLen, (uint16_t*)barcode, &code_num)) {
		return false;
	}

	// merge barcode 128c
	if (!barcodegen_lvgl_merge((uint16_t*)barcode, code_num, barcode, bitsLen)) {
		return false;
	}

	return true;
}

// Public function - see documentation comment in header file.
bool barcodegen_lvgl_getModule(const uint8_t barcode[], int x) {
	uint8_t val = barcode[x >> 3];
	uint8_t boff = 7 - (x & 7);

	return ((val >> boff) & 1);
}

