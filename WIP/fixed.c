# include "fixed.h"

# define ATAN2_LIMIT 10
static const fixed16_6 ATANS[ATAN2_LIMIT] = {2880, 1700, 898, 456, 229, 115, 57, 29, 14, 7};

/*
 * Function fx_atan2
 * Desc     Returns the arc tangent of b / a, 
 *          considering the sign of both b and a.
 *          Thus, the returned value is in (-180, 180]
 * Input    fixed16_10 b and a
 * Output   The angle in fixed16_6
 */
fixed16_6 fx_atan2(fixed16_10 b, fixed16_10 a) {
    // If the angle is in the 2nd and 3rd quadrant,
    // return atan2(-b, -a) - pi
    if( a < 0 ) {
        fixed16_6 res = - 11520 + fx_atan2(-b, -a);
        if( res <= -11520 )
            return res + 23040;
        else if( res > 11520 )
            return res - 23040;
        else
            return res;
    }

    // Avoid extra calculations:
    //  - atan2(0,0) = 0
    //  - atan2(x, 0) = ±90
    //  - atan2(0, x) = 90 ± 90
    else if( (a == b) && (a == 0) )
        return 0;
    else if( a == 0 ) {
        if( b > 0 )
            return 5760;
        else
            return -5760;
    } else if( b == 0 ) {
        if( a > 0 )
            return 0;
        else
            return 11520;
    }

    // The positions of the current points, and the current approximation
    fixed16_13 x = 8192;
    fixed16_13 y = 0;
    fixed16_6 z = 0;
    fixed16_13 tmp;

    // The tangent of the desired angle
    fixed16_10 tan = div16_10(b, a);

    for( int8_t i = 0; i < ATAN2_LIMIT; ++i ) {
        // Compare in fixed16_10 format, losing 3LSBs (~0.067 in the worst case)
        // but keeping MSBs.
        // If cur_tan > tan <=> y / x > tan <=> y > x * tan (x always positive)
        // then substract the angle, else add it
        tmp = x;
        if( (y >> 3) > (mul16_10( (x >> 3), tan)) ) {
            x = x + (y >> i);
            y = y - (tmp >> i);
            z = z - ATANS[i];
        } else {
            x = x - (y >> i);
            y = y + (tmp >> i);
            z = z + ATANS[i];
        }
    }

    return z;
}
