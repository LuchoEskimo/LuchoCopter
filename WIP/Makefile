NAME = main
WIP = mpu9150
LIB = ../lib/* $(WIP).* fixed.*
#LIB = motors.c usart.c eon.c i2c.c
PROGRAMMER = usbasp-clone
#PROGRAMMER = arduino
F_CPU=16000000UL
MMCU_GCC = atmega328p
MMCU_AVRDUDE = m328p
OPTIMISATIONS = -Os -Wall
OPTIONS_GCC = -std=gnu99 -mmcu=$(MMCU_GCC) $(OPTIMISATIONS) -DF_CPU=$(F_CPU)

ifeq ($(PROGRAMMER), usbasp-clone)
	PORT = /dev/USBasp
else
	PORT = /dev/ttyACM0
endif

all: $(NAME).hex

$(NAME).hex: $(NAME).o
	avr-objcopy -O ihex $(NAME).o $(NAME).hex

$(NAME).o: $(NAME).c $(WIP).h $(WIP).c fixed.*
	avr-gcc $(OPTIONS_GCC) $(NAME).c $(LIB) -o $(NAME).o

.PHONY : clean
clean:
	rm -f *.o *.hex *~
	rm -f *.swp

.PHONY : upload
upload: $(NAME).hex
	avrdude -c $(PROGRAMMER) -p $(MMCU_AVRDUDE) -P $(PORT) -U flash:w:$(NAME).hex:i
	make clean
