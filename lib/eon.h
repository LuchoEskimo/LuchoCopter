# ifndef EON_H
# define EON_H

# include <avr/io.h>
# include <avr/interrupt.h>
# include <stdint.h>
# include <util/atomic.h>

void eon_init(void);
uint32_t eon_millis(void);

# endif
