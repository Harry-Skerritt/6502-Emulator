# 6502 Emulator in C++

An emulator library for the 6502 CPU built in C++

Inspired by: [Dave Poo](https://www.youtube.com/watch?v=qJgsuQoy9bc&list=PLLwK93hM93Z13TRzPx9JqTIn33feefl37&pp=0gcJCV8EOCosWNin)
and [Ben Eater](https://www.youtube.com/playlist?list=PLowKtXNTBypFbtuVMUVXNR0z1mu7dp7eH)
on YouTube.

Based on: [This Documentation](http://www.6502.org/users/obelisk/)

## How to add the library to your project
### Method 1: CMake FetchContent 
The easiest way to include the library in your C++ project is by using CMake's FetchContent.\
Add the following to your `CMakeLists.txt`
```cmake
FetchContent_Declare(
  6502Library
  GIT_REPOSITORY https://github.com/Harry-Skerritt/6502-Emulator.git
  GIT_TAG main
)

FetchContent_MakeAvailable(6502Library)

target_link_libraries(MyProject PRIVATE 6502_Library)
```

### Method 2: Manually Adding the Library
Download the .zip source code from [the releases]()
this can then be dropped into a /external or /include folder within your project

You should then add these lines to your `CMakeLists.txt`, replacing `external` with your external libraries folder
```cmake
add_subdirectory(external/6502Library)
target_link_libraries(MyProject PRIVATE 6502_Library)
```

## How to setup the emulator
An instance of the CPU **and** Memory are required! \

### The first step is loading memory
To load memory you can use a few different functions

#### Hard coding memory
```c++
memory.initMemory(); // This will set the entire 65535 bytes of memory to 0x00
```

```c++
memory.setMemory(0xEA); // You can put any value as the argument and the entire 65535 bytes of memory will be set to that value
```

If using either of the above memory inits, the values in memory will need to be specified in a way similar to below
```c++
    memory.data[0x8000] = 0xA9;
    memory.data[0x8001] = 0x42;
    memory.data[0x8002] = 0x8D;
    memory.data[0x8003] = 0x00;
    memory.data[0x8004] = 0x60;
```


#### Loading from a file
```c++
memory.loadMemory("/path/to/file"); // You can specify the path to a .bin file which will be loaded into memory
```

```c++
memory.promptMemoryLoad(); // This will give you instructions to aid in memory setting within the console
```
***You only need ONE of these lines***


### The second step is to reset the CPU
The cpu needs to go through its reset sequence and move to the sector of memory which stores the program. 
This address is specified by the reset vector (0xFFFC and 0xFFFD) and the start address of your program should be stored at these two values using little endian.

To reset the cpu you can use:
``` c++
cpu.reset(memory);
```

### The final step is executing code
To execute code the function
```c++
cpu.execute(x, memory);
```
can be used.
The amount of clock cycles you wish to be performed should be passed in as x.

**This number can be less than the total for the program, but cannot be more unless the memory is initialised to 0xEA**


### An Example
The code below shows a basic program for settings up the emulator. \
The hardcoded program here takes the value 42, store it in the A register. Then stores the A register at address 0x6000

```c++
// main.cpp
#include <emulator_6502.h>
using namespace emulator_6502;

int main() {
    CPU cpu;
    Memory memory;

    memory.setMemory(0xEA); // Set the memory to NOP

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

    cpu.execute(6, memory); // <- Pass number of required clock cycles (6 here)

    return 0;
}