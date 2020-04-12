/*
 * Image.h
 *
 *  Created on: 11 апр. 2020 г.
 *      Author: Garick
 */

#ifndef IMAGE_H_
#define IMAGE_H_
#include "stm32f30x.h"

//((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3)
extern const uint8_t image_bmp[32454];
extern const uint32_t image_bmp_size;

#endif /* IMAGE_H_ */
