# ifndef FIXED_H
# define FIXED_H

# include <inttypes.h>

// Main fixed point formats
typedef int32_t fixed32_10;
typedef int16_t fixed16_13;
typedef int16_t fixed16_10;
typedef int16_t fixed16_6;
typedef int16_t fixed16_4;
typedef int16_t fixed16_2;

// Conversion macros
# define fixed32_10toFloat(fixed) ((float)((fixed) / 1024.0f))
# define fixed16_13toFloat(fixed) ((float)((fixed) / 8192.0f))
# define fixed16_10toFloat(fixed) ((float)((fixed) / 1024.0f))
# define fixed16_6toFloat(fixed) ((float)((fixed) / 64.0f))
# define fixed16_4toFloat(fixed) ((float)((fixed) / 16.0f))
# define fixed16_2toFloat(fixed) ((float)((fixed) / 4.0f))
# define floatToFixed32_10(fpn) ((fixed32_10)((fpn) * 1024.0f))
# define floatToFixed16_13(fpn) ((fixed16_13)((fpn) * 8192.0f))
# define floatToFixed16_10(fpn) ((fixed16_10)((fpn) * 1024.0f))
# define floatToFixed16_6(fpn) ((fixed16_6)((fpn) * 64.0f))
# define floatToFixed16_4(fpn) ((fixed16_4)((fpn) * 16.0f))
# define floatToFixed16_2(fpn) ((fixed16_2)((fpn) * 4.0f))

// Operators
# define add32_10(n1, n2) ((n1) + (n2))
# define add16_13(n1, n2) ((n1) + (n2))
# define add16_10(n1, n2) ((n1) + (n2))
# define add16_6(n1, n2) ((n1) + (n2))
# define add16_4(n1, n2) ((n1) + (n2))
# define add16_2(n1, n2) ((n1) + (n2))

# define sub32_10(n1, n2) ((n1) - (n2))
# define sub16_13(n1, n2) ((n1) - (n2))
# define sub16_10(n1, n2) ((n1) - (n2))
# define sub16_6(n1, n2) ((n1) - (n2))
# define sub16_4(n1, n2) ((n1) - (n2))
# define sub16_2(n1, n2) ((n1) - (n2))

# define mul32_10(n1, n2) ((int32_t)((((int64_t)(n1)) * (n2)) >> 10))
# define mul16_13(n1, n2) ((int16_t)((((int32_t)(n1)) * (n2)) >> 13))
# define mul16_10(n1, n2) ((int16_t)((((int32_t)(n1)) * (n2)) >> 10))
# define mul16_6(n1, n2) ((int16_t)(((int32_t)(n1)) * (n2)) >> 6)
# define mul16_4(n1, n2) ((int16_t)(((int32_t)(n1)) * (n2)) >> 4)
# define mul16_2(n1, n2) ((int16_t)(((int32_t)(n1)) * (n2)) >> 2)

# define div32_10(n1, n2) ((int32_t)((((int64_t)(n1) << 10) / (n2))))
# define div16_13(n1, n2) ((int16_t)((((int32_t)(n1)) << 13) / (n2)))
# define div16_10(n1, n2) ((int16_t)((((int32_t)(n1)) << 10) / (n2)))
# define div16_6(n1, n2) ((int16_t)((((int32_t)(n1)) << 6) / (n2)))
# define div16_4(n1, n2) ((int16_t)((((int32_t)(n1)) << 6) / (n2)))
# define div16_2(n1, n2) ((int16_t)((((int32_t)(n1)) << 2) / (n2)))

fixed16_6 fx_atan2(fixed16_10 x, fixed16_10 y);
fixed16_10 fx_invsqrt(fixed32_10 x);

# endif
