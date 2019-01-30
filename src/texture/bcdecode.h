#pragma once

#include "../io/io.h"
#include "texture.h"

typedef enum {
  BcnDecoderFormatRGBA = 1,
  BcnDecoderFormatBGRA = 2,
  BcnDecoderFormatARGB = 3,
  BcnDecoderFormatABGR = 4
} BcnDecoderFormat;

int BcnDecode(uint8_t *dst, int dst_size, const uint8_t *src, int src_size,
              int width, int height, int N, int dst_format, int flip);