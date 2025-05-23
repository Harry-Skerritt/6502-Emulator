# 6502 Emulator in C++

An emulator library for the 6502 CPU built in C++;

Inspired by: [DavePoo](https://www.youtube.com/watch?v=qJgsuQoy9bc&list=PLLwK93hM93Z13TRzPx9JqTIn33feefl37&pp=0gcJCV8EOCosWNin)
and [Ben Eater](https://www.youtube.com/playlist?list=PLowKtXNTBypFbtuVMUVXNR0z1mu7dp7eH)
on YouTube.

Based on: [This Documentation](http://www.6502.org/users/obelisk/)

## How to setup the emulator
An instance of the CPU and Memory are required! \
The code below shows a basic program for settings up the emulator. \
The most important thing is to set the address in the reset vector. 

The hardcoded program here takes the value 42, store it in the A register. Then stores the A register at address 0x6000

```c++
// main.cpp
#include <emulator_6502.h>
using namespace emulator_6502;

int main() {
    CPU cpu;
    Memory memory;

    memory.setMemory(0xEA); // Set the memory to some value

    // Set the reset vector values to an address
    memory.data[0xFFFC] = 0x00;
    memory.data[0xFFFD] = 0x80;

    cpu.reset(memory); // <- Resets the CPU and starts the program

    // Set other memory addresses here
    memory.data[0x8000] = 0xA9;
    memory.data[0x8001] = 0x42;
    memory.data[0x8002] = 0x8D;
    memory.data[0x8003] = 0x00;
    memory.data[0x8004] = 0x60;

    cpu.execute(6, memory); // <- Pass number of required clock cycles

    return 0;
}