
#include <emulator_6502.h>

// http://www.6502.org/users/obelisk/

using namespace emulator_6502;
int main() {

    CPU cpu;
    Memory memory;

    memory.setMemory(0xEA);

    // 0xFFFC = Reset Vector
    // Reset Address: 0x8000
    memory.data[0xFFFC] = 0x00;
    memory.data[0xFFFD] = 0x80;

    cpu.reset(memory);

    memory.data[0x8000] = 0xA9;
    memory.data[0x8001] = 0x42;
    memory.data[0x8002] = 0x8D;
    memory.data[0x8003] = 0x00;
    memory.data[0x8004] = 0x60;

    cpu.execute(4, memory);

    memory.dumpMemoryToFile(0, memory.MAX_MEMORY);







    return 0;
}
