# OA - Operating system for Arduino

Allows me to experiment with OS concepts I want to implement in assembler for my Z80 RC2014. Such as:

* Cooperative multitasking
* In-memory file system for memory management

## Code layout

### uart.c

Handles reading and writing bytes to the serial line.

### blk.c

Allocates a fixed chunk of memory and divides that up into blocks of a fixed size. The memory area starts with a block table that indicates if a block is in use and the next block in a chain of linked blocks.

### mini8fs.c

A small filesystem implemented on top of blk. File entries are either files or directories. Directory blocks contain lists of file entries. All memory allocations are made via the filesystem meaning all things are visible in the filesystem.

### coroutine.c

Handles switching between two coroutines. A coroutine is a function entry jump address and a stack. When starting a coroutine the stack pointer is switched out for the coroutine stack and the function is jumped to. When the coroutine yields the previous (kernel) stack pointer is switched back.

### queue.c

A queue is a reference to a producer function, a reference to a consumer function and a ring buffer. The producer function is periodically called and if the queue is not empty the consumer function is called (TODO: this concept will probably go away as coroutines will fill and empty ring buffers themselves).

### ringbuf.c

An implementation of a simple ring buffer with start and end pointers.

### shell.c

A basic shell for interpreting user commands.

### oa.c

The "main" OS entry point.

## Queue based multitasking

The idea is that the kernel is simply looping over a set of ring buffers, calling a "write byte" callback then calling a "read byte" callback. Every other part of the OS is implemented on top of that. There are no event loops allowed in any program, and if they need event loop functionality they must use available queues or timers for that. Programs are simply function handlers attached to queues.

The OS starts with three builtin queues:

* stdin - has bytes populated when a character is received from the UART
* stdout - bytes written here go out the UART
* finit - bytes written here are addresses of init functions to launch

Writing an address to finit causes the kernel to call that function pointer. It is expected that this function will initialize a "program" by loading other functions into memory and set up more queues in the system.

For example a shell program would attach a callback to the consumer of the stdin queue (overriding the default "echo" consumer) as well as set up a chunk of memory as a line buffer.

## Motivation

Operating systems are usually preemptive multitasking. They were born from a time when the usual mode 
