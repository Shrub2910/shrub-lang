#ifndef OPERAND_CONVERSION_H
#define OPERAND_CONVERSION_H

#include <stdint.h>

// Conversion from unsigned bytes to other type

#define TO_SIGNED_WORD(lo, hi) \
  ((int16_t) ((uint16_t) (hi) << 8) | ((uint16_t) (lo)))

// Conversion from other type to bytes

#define FROM_SIGNED_WORD(val) \
  ((uint8_t) ((int16_t)(val) & 0xFF)), \
  ((uint8_t) (((int16_t)(val) >> 8) & 0xFF))

#endif
