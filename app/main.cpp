
#include <emulator_6502.h>

// http://www.6502.org/users/obelisk/

using namespace emulator_6502;
int main() {

    CPU cpu;
    Memory memory;


/*
    memory.setMemory(0xEA);
    memory.data[0xFFFC] = 0x00;
    memory.data[0xFFFD] = 0x80; // 0x8000

    memory.data[0xFFFE] = 0x34;
    memory.data[0xFFFF] = 0x12; // 0x1234

    // Loads A with 42. Stores A in 0x6000 - Main Function
    memory.data[0x8000] = 0xA9;
    memory.data[0x8001] = 0x42;
    memory.data[0x8002] = 0x8D;
    memory.data[0x8003] = 0x00;
    memory.data[0x8004] = 0x60;
    memory.data[0x8005] = 0x00;

    // Loads X with 69. Stores X in 0x7000 - After Interrupt Return
    memory.data[0x8006] = 0xA2;
    memory.data[0x8007] = 0x69;
    memory.data[0x8008] = 0x8E;
    memory.data[0x8009] = 0x00;
    memory.data[0x800A] = 0x70;

    // Loads A with 87. Stores A in 0x9000 - Interrupt Code
    memory.data[0x1234] = 0xA9;
    memory.data[0x1235] = 0x87;
    memory.data[0x1236] = 0x8D;
    memory.data[0x1237] = 0x00;
    memory.data[0x1238] = 0x90;
    memory.data[0x1239] = 0x40;

    // Stack?
    // 0x01FD = a0
    // 0x01FE = 06
    // 0x01FF = 80

*/

    memory.promptMemoryLoad();

    cpu.reset(memory);
    cpu.execute(60, memory);

    memory.dumpMemoryToFile(0, memory.MAX_MEMORY);

    return 0;
}
