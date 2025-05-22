
#include <emulator_6502.h>


using namespace emulator_6502;
int main() {

    CPU cpu;
    Memory memory;

    cpu.reset(memory);

    cpu.X_reg = 0x0f;

    memory.data[0xFFFC] = 0x20;
    memory.data[0xFFFD] = 0xB2;
    memory.data[0x00B2] = 0xA9;
    memory.data[0x00B3] = 0x89;

    //memory.dumpMemory();
    //memory.dumpMemory(0xFF00, 256);

    //memory.dumpMemoryToFile(0, memory.MAX_MEMORY);

    outputByte(cpu.Accumulator, "A Register Before: ");


    cpu.execute(8, memory);

    outputByte(cpu.Accumulator, "A Register After: ");







    return 0;
}
