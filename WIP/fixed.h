# ifndef FIXED_H
# define FIXED_H

// Main fixed point formats
typedef int16_t fixed16_10;
typedef int16_t fixed16_2;

// Conversion macros
# define fixed16_10toFloat(fixed) ((float)(fixed / 1024.0f))
# define fixed16_2toFloat(fixed) ((float)(fixed / 4.0f))
# define floatToFixed16_10(fpn) ((fixed16_10)(fpn * 1024.0f))
# define floatToFixed16_2(fpn) ((fixed16_2)(fpn * 4.0f))

# endif
