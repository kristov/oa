DEVICE = /dev/ttyUSB0

OBJECTS =
OBJECTS += ringbuf.o
OBJECTS += queue.o
OBJECTS += uart.o
OBJECTS += mini8fs.o
OBJECTS += shell.o
OBJECTS += oa.o

#MAPARGS = -Wl,-Map=myprog.map -Wl,--cref

all: oa.hex

%.o: %.c
	avr-gcc -Wall -O1 -DF_CPU=16000000UL -mmcu=atmega328p -c -I. -o $@ $<

oa.bin: $(OBJECTS)
	avr-gcc -mmcu=atmega328p $(OBJECTS) $(MAPARGS) -o oa.bin
	avr-size --format=avr --mcu=atmega328p oa.bin

oa.hex: oa.bin
	avr-objcopy -O ihex -R .eeprom oa.bin oa.hex

install:
	avrdude -F -V -c arduino -p ATMEGA328P -P $(DEVICE) -b 57600 -U flash:w:oa.hex

clean:
	rm -f oa.hex oa.bin $(OBJECTS)

deps:
	sudo apt-get install binutils-avr gcc-avr avr-libc avrdude

# screen /dev/ttyUSB0 57600
