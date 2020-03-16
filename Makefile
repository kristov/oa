DEVICE = /dev/ttyUSB0

OBJECTS =
OBJECTS += ringbuf.o
OBJECTS += uart.o
OBJECTS += mini8fs.o
OBJECTS += oa.o

all: oa.hex

%.o: %.c
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -I. -o $@ $<

oa.bin: $(OBJECTS)
	avr-gcc -mmcu=atmega328p $(OBJECTS) -o oa.bin

oa.hex: oa.bin
	avr-objcopy -O ihex -R .eeprom oa.bin oa.hex

install:
	avrdude -F -V -c arduino -p ATMEGA328P -P $(DEVICE) -b 57600 -U flash:w:oa.hex

clean:
	rm -f oa.hex oa.bin $(OBJECTS)

deps:
	sudo apt-get install binutils-avr gcc-avr avr-libc avrdude

# screen /dev/ttyUSB0 57600