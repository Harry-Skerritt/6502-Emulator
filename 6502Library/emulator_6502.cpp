//
// Created by Harry Skerritt on 22/05/2025.
//

#include "emulator_6502.h"



// Outputs a formatted byte
void emulator_6502::outputByte(Byte value, const std::string& before_text, bool error) {
    if (!std::empty(before_text) && !error) {
        std::cout << before_text;
    } else if (!std::empty(before_text) && error) {
        std::cerr << before_text;
    }

    std::cout << "0x"
              << std::hex << std::uppercase
              << std::setw(2) << std::setfill('0')
              << static_cast<int>(value) << "\n";
}

// Outputs a formatted word
void emulator_6502::outputWord(Word value, const std::string& before_text, bool error) {
    if (!std::empty(before_text) && !error) {
        std::cout << before_text;
    } else if (!std::empty(before_text) && error) {
        std::cerr << before_text;
    }

    std::cout << "0x"
              << std::hex << std::uppercase
              << std::setw(4) << std::setfill('0')
              << static_cast<int>(value) << "\n";
}



// Memory

// Initializes memory to 0x0000
void emulator_6502::Memory::initMemory() {
    for (u32 i = 0; i < MAX_MEMORY; i++) {
        data[i] = 0x0000;
    }

    std::cout << "Memory initialized" << std::endl;
}

// Dumps the given memory section
void emulator_6502::Memory::dumpMemory(size_t start, size_t length) {
    std::cout << "Memory Dump ###" << std::endl;
    const size_t bytes_per_row = 32;

    for (size_t addr = start; addr < start + length; addr += bytes_per_row) {
        // Print address
        std::cout << std::hex << std::setw(4) << std::setfill('0') << addr << "  ";

        // Print hex bytes
        for (size_t i = 0; i < bytes_per_row; ++i) {
            if (addr + i < MAX_MEMORY)
                std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(data[addr + i]) << " ";
            else
                std::cout << "   ";
        }

        std::cout << " ";

        // Print ASCII representation
        for (size_t i = 0; i < bytes_per_row; ++i) {
            if (addr + i < MAX_MEMORY) {
                char c = static_cast<char>(data[addr + i]);
                std::cout << (std::isprint(c) ? c : '.');
            }
        }

        std::cout << '\n';
    }

    // Reset formatting
    std::cout << std::dec << std::setfill(' ');


    std::cout << "Memory Dump End ###" << std::endl;
}


// CPU

// Sets the 6502 into a reset state
void emulator_6502::CPU::reset(Memory& memory) {
    PC = 0xFFFC;
    SP = 0x0100;

    A = X = Y = 0;
    N = V = B = I = Z = C = D = 0;

    memory.initMemory();
}

// Gets and returns the value at PC, increments PC
emulator_6502::Byte emulator_6502::CPU::fetchByte(u32& clock_cycles, Memory& memory) {
    Byte data = memory[PC];
    PC++;

    clock_cycles--;

    return data;
}

// Gets and returns the value at the address, DOES NOT increment PC
emulator_6502::Byte emulator_6502::CPU::readByte(u32 &clock_cycles, Memory &memory, Byte& address) {
    Byte data = memory[address];
    clock_cycles--;
    return data;
}

// Gets and returns the value at PC, increments PC
emulator_6502::Word emulator_6502::CPU::fetchWord(u32& clock_cycles, Memory& memory) {
    // Note: Little Endian

    Word data = memory[PC];
    PC++;

    data |= (memory[PC] << 8);
    PC++;

    clock_cycles-=2;
    return data;
}

// Executes the specified cycle amount of cycles on the 6502
void emulator_6502::CPU::execute(u32 cycles, Memory& memory) {

    while (cycles > 0) {
        Byte instruction = fetchByte(cycles, memory);

        checkLDA(cycles, memory, instruction);
    }

}

// Sets the processor status flags for LDA instructions
void emulator_6502::CPU::setLDAFlags() {
    Z = (A == 0);
    N = (A & 0b1000000) > 0;
}


// Checks for LDA instructions (does not use clock cycle)
void emulator_6502::CPU::checkLDA(u32& cycles, Memory& memory, Byte& ins) {
    switch (ins) {

        case INS_LDA_IM: {
            A = fetchByte(cycles, memory);
            setLDAFlags();
        } break;

        case INS_LDA_ZP: {
            Byte zpa = fetchByte(cycles, memory);
            A = readByte(cycles, memory, zpa);
            setLDAFlags();
        } break;

        case INS_LDA_ZPX: {
            Byte zpa = fetchByte(cycles, memory);
            zpa += X;
            cycles--;
            A = readByte(cycles, memory, zpa);
            setLDAFlags();
        } break;

        default: {
            throw InvalidInstructionException(PC);
        }
    }
}




