
#include <emulator_6502.h>


using namespace emulator_6502;
int main() {

    CPU cpu;
    Memory memory;

    cpu.reset(memory);

    cpu.X_reg = 0x0f;

    // 0xFFFC = Reset Vector
    memory.data[0xFFFC] = 0x20; // JSR 0xB2
    memory.data[0xFFFD] = 0xB2; //
    memory.data[0xFFFE] = 0x00;
    memory.data[0x00B2] = 0xA9; // LDA 0x89
    memory.data[0x00B3] = 0x89; //
    memory.data[0x00B4] = 0x60; // RTS
    memory.data[0xFFFF] = 0xAA; // TAX

    //memory.dumpMemory();
    //memory.dumpMemory(0xFF00, 256);

    //memory.dumpMemoryToFile(0, memory.MAX_MEMORY);

    outputByte(cpu.Accumulator, "A Register Before: ");
    outputByte(cpu.X_reg, "X Register Before: ");


    cpu.execute(16, memory);

    outputByte(cpu.Accumulator, "A Register After: ");
    outputByte(cpu.Accumulator, "X Register After: ");







    return 0;
}
