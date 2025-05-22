
#include <emulator_6502.h>


using namespace emulator_6502;
int main() {

    CPU cpu;
    Memory memory;

    cpu.reset(memory);

    cpu.X = 0x0f;

    memory.data[0xFFFC] = 0xB5;
    memory.data[0xFFFD] = 0x80;
    memory.data[0x008F] = 0x96;

    memory.dumpMemory();
    memory.dumpMemory(0xFF00, 256);

    outputByte(cpu.A, "A Register Before: ");


    cpu.execute(4, memory);

    outputByte(cpu.A, "A Register After: ");





    return 0;
}
