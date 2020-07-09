CC := gcc
CFLAGS := -Wall -Werror -ggdb

all: blk.t mini8fs.t

blk.t: blk.c blk.o fsimg.bin
	$(CC) $(CFLAGS) -I../ -o $@ blk.o $<

mini8fs.t: mini8fs.c blk.o mini8fs.o fsimg.bin
	$(CC) $(CFLAGS) -I../ -o $@ blk.o mini8fs.o $<

%.o: ../%.c ../%.h
	$(CC) $(CFLAGS) -DLINUX=1 -c -I../ -o $@ $<

fsimg.bin: fsimg.hex
	xxd -r -p fsimg.hex fsimg.bin

clean:
	rm -f blk.o mini8fs.o blk.t mini8fs.t fsimg.bin
