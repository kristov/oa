# OA - Operating system for Arduino

Allows me to experiment with OS concepts I want to implement in assembler for my Z80 RC2014. Such as:

* Queue based cooperative multitasking
* In-memory file system for memory management

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
