CC := gcc
CFLAGS := -Wall -Werror -ggdb

all: mini8fs.t

mini8fs.t: mini8fs.c mini8fs.o fsimg.bin
	$(CC) $(CFLAGS) -I../ -o $@ mini8fs.o $<

%.o: ../%.c ../%.h
	$(CC) $(CFLAGS) -DLINUX=1 -c -I../ -o $@ $<

fsimg.bin: fsimg.hex
	xxd -r -p fsimg.hex fsimg.bin

clean:
	rm -f mini8fs.o mini8fs.t fsimg.bin
