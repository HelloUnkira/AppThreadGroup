/*
 * Copyright (c) 2022 Actions Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


/*备注:
 *    这里将所有的barcodegen修改成barcodegen_lvgl
 */

#ifdef __cplusplus
extern "C" {
#endif


/* 
 * This library creates Bar Code symbols
 */

/*---- Enum and struct types----*/

/* 
 * The type in a Bar Code symbol.
 */
enum barcodegen_lvgl_Type {
	// Must be declared in ascending order of error protection
	// so that an internal barcodegen_lvgl function works properly
	barcodegen_lvgl_128C = 0 ,  // Bar Code128C
};

/*---- Macro constants and functions ----*/

// The max number of bytes needed to store data
#define barcodegen_lvgl_DATA_LEN_MAX  20

// The max number of bytes needed to store Bar Code
#define barcodegen_lvgl_BUFFER_LEN_MAX  ((barcodegen_lvgl_DATA_LEN_MAX / 2 + 5) * 2)

/*---- Functions (high level) to generate Bar Codes ----*/

/* 
 * Encodes the given binary data to a Bar Code, returning true if encoding succeeded.
 */
bool barcodegen_lvgl_encodeBinary(uint8_t dataAndTemp[], size_t dataLen, uint8_t barcode[],
	enum barcodegen_lvgl_Type type, size_t *bitsLen);

/* 
 * Returns the color of the module (pixel) at the given coordinates, which is false
 * for white or true for black. The top left corner has the coordinates (x=0, y=0).
 * If the given coordinates are out of bounds, then false (white) is returned.
 */
bool barcodegen_lvgl_getModule(const uint8_t barcode[], int x);


#ifdef __cplusplus
}
#endif
