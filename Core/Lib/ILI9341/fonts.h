/*
 * fonts.h
 *
 *  Created on: Dec 17, 2023
 *      Author: zh
 */

#ifndef LIB_ILI9341_FONTS_H_
#define LIB_ILI9341_FONTS_H_

#include <stdint.h>

typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;


extern FontDef Font_7x10;
extern FontDef Font_11x18;
extern FontDef Font_16x26;

#endif // LIB_ILI9341_FONTS_H_
