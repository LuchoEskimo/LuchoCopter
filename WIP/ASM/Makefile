NAME = beeep
PROGRAMMER = usbasp-clone
#PROGRAMMER = arduino
MMCU_AVRDUDE = m328p

ifeq ($(PROGRAMMER), usbasp-clone)
	PORT = /dev/USBasp
else
	PORT = /dev/ttyACM0
endif

all: $(NAME).s.hex

$(NAME).s.hex: $(NAME).s
	avra $(NAME).s -l $(NAME).lst

.PHONY : list
list: 	all
	avra $(NAME).s -l $(NAME).lst

.PHONY : map
map: 	all
	avra $(NAME).s -m $(NAME).map

.PHONY : clean
clean:
	rm -f *.o *.hex *~ $(NAME).s.* $(NAME).lst $(NAME).map
	rm -f *.swp

.PHONY : upload
upload: $(NAME).s.hex
	avrdude -c $(PROGRAMMER) -p $(MMCU_AVRDUDE) -P $(PORT) -U flash:w:$(NAME).s.hex:i
	make clean
