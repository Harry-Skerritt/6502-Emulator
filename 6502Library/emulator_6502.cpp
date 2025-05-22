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

    Accumulator = X_reg = Y_reg = 0;
    N = V = B = I = Z = C = D = 0;

    memory.initMemory();
}

// Gets and returns the Byte value at PC, increments PC
emulator_6502::Byte emulator_6502::CPU::fetchByte(u32& clock_cycles, Memory& memory) {
    Byte data = memory[PC];
    PC++;

    clock_cycles--;

    return data;
}

// Gets and returns the Byte value at the address, DOES NOT increment PC (Takes byte as address)
/*
 emulator_6502::Byte emulator_6502::CPU::readByte(u32 &clock_cycles, Memory &memory, Byte address) {
    Byte data = memory[address];
    clock_cycles--;
    return data;
}
*/

// Gets and returns the Byte value at the address, DOES NOT increment PC (Takes word as address)
emulator_6502::Byte emulator_6502::CPU::readByte(u32 &clock_cycles, Memory &memory, Word address) {
    Byte data = memory[address];
    clock_cycles--;
    return data;
}


// Gets and returns the Word value at PC, increments PC
emulator_6502::Word emulator_6502::CPU::fetchWord(u32& clock_cycles, Memory& memory) {
    // Note: Little Endian

    Word data = memory[PC];
    PC++;

    data |= (memory[PC] << 8);
    PC++;

    clock_cycles-=2;
    return data;
}

// Gets and returns the Word value at PC, DOES NOT increment PC
emulator_6502::Word emulator_6502::CPU::readWord(u32& clock_cycles, Memory& memory, Word address) {
    Byte low_byte = readByte(clock_cycles, memory, address);
    Byte high_byte = readByte(clock_cycles, memory, (address + 1));
    return low_byte | (high_byte << 8);
}



// Executes the specified cycle amount of cycles on the 6502
void emulator_6502::CPU::execute(u32 cycles, Memory& memory) {

    while (cycles > 0) {
        Byte instruction = fetchByte(cycles, memory);

        // Load Registers
        checkLDA(cycles, memory, instruction);
        checkLDX(cycles, memory, instruction);
        checkLDY(cycles, memory, instruction);
    }

}

// *** Load Registers ***
// Sets the processor status flags for LDA instructions
void emulator_6502::CPU::setRegisterFlag(Byte& reg) {
    Z = (reg == 0);
    N = (reg & 0b1000000) > 0;
}

// Checks for LDA instructions (does not use clock cycle)
void emulator_6502::CPU::checkLDA(u32& cycles, Memory& memory, Byte& ins) {
    switch (ins) {

        case INS_LDA_IM: {
            Accumulator = fetchByte(cycles, memory);
            setRegisterFlag(Accumulator);
        } break;

        case INS_LDA_ZP: {
            Byte zpa = fetchByte(cycles, memory);
            Accumulator = readByte(cycles, memory, zpa);
            setRegisterFlag(Accumulator);
        } break;

        case INS_LDA_ZPX: {
            Byte zpa = fetchByte(cycles, memory);
            zpa += X_reg;
            cycles--;
            Accumulator = readByte(cycles, memory, zpa);
            setRegisterFlag(Accumulator);
        } break;

        case INS_LDA_ABS: {
            Word abs_address = fetchWord(cycles, memory);
            Accumulator = readByte(cycles, memory, abs_address);
            setRegisterFlag(Accumulator);
        } break;

        case INS_LDA_ABSX: {

            Word abs_address = fetchWord(cycles, memory);
            Word absx_address = readByte(cycles, memory, abs_address);
            Accumulator = readByte(cycles, memory, absx_address);
            if (absx_address - abs_address >= 0xFF) {
                cycles--;
            }
            setRegisterFlag(Accumulator);
        } break;

        case INS_LDA_ABSY: {
            Word abs_address = fetchWord(cycles, memory);
            Word absy_address = readByte(cycles, memory, abs_address);
            Accumulator = readByte(cycles, memory, absy_address);
            if (absy_address - abs_address >= 0xFF) {
                cycles--;
            }
            setRegisterFlag(Accumulator);
        } break;

        case INS_LDA_INDX: {
            Byte zp = fetchByte(cycles, memory);
            Word use_address = readWord(cycles, memory, zp);
            Word use_x_address = use_address + X_reg;
            Accumulator = readByte(cycles, memory, use_x_address);
            if (use_address - use_x_address >= 0xFF) {
                cycles--;
            }
            setRegisterFlag(Accumulator);
        } break;

        case INS_LDA_INDY: {
            Byte zp = fetchByte(cycles, memory);
            Word use_address = readWord(cycles, memory, zp);
            Word use_y_address = use_address + Y_reg;
            Accumulator = readByte(cycles, memory, use_y_address);
            if (use_address - use_y_address >= 0xFF) {
                cycles--;
            }
            setRegisterFlag(Accumulator);
        } break;


        default: {
            throw InvalidInstructionException(PC);
        }
    }
}

// Checks for LDX instructions (does not use clock cycle)
void emulator_6502::CPU::checkLDX(u32& cycles, Memory& memory, Byte& ins) {
    switch (ins) {

        case INS_LDX_IM: {
            X_reg = fetchByte(cycles, memory);
            setRegisterFlag(X_reg);
        } break;

        case INS_LDX_ZP: {
            Byte zpa = fetchByte(cycles, memory);
            X_reg = readByte(cycles, memory, zpa);
            setRegisterFlag(X_reg);
        } break;

        case INS_LDX_ZPY: {
            Byte zpa = fetchByte(cycles, memory);
            zpa += Y_reg;
            cycles--;
            X_reg = readByte(cycles, memory, zpa);
            setRegisterFlag(X_reg);
        } break;

        case INS_LDX_ABS: {
            Word abs_address = fetchWord(cycles, memory);
            X_reg = readByte(cycles, memory, abs_address);
            setRegisterFlag(X_reg);
        } break;

        case INS_LDX_ABSY: {
            Word abs_address = fetchWord(cycles, memory);
            Word absy_address = readByte(cycles, memory, abs_address);
            X_reg = readByte(cycles, memory, absy_address);
            if (absy_address - abs_address >= 0xFF) {
                cycles--;
            }
            setRegisterFlag(X_reg);
        } break;

        default: {
            throw InvalidInstructionException(PC);
        }
    }
}

// Checks for LDY instructions (does not use clock cycle)
void emulator_6502::CPU::checkLDY(u32& cycles, Memory& memory, Byte& ins) {
    switch (ins) {

        case INS_LDY_IM: {
            Y_reg = fetchByte(cycles, memory);
            setRegisterFlag(Y_reg);
        } break;

        case INS_LDY_ZP: {
            Byte zpa = fetchByte(cycles, memory);
            Y_reg = readByte(cycles, memory, zpa);
            setRegisterFlag(Y_reg);
        } break;

        case INS_LDY_ZPX: {
            Byte zpa = fetchByte(cycles, memory);
            zpa += X_reg;
            cycles--;
            Y_reg = readByte(cycles, memory, zpa);
            setRegisterFlag(Y_reg);
        } break;

        case INS_LDY_ABS: {
            Word abs_address = fetchWord(cycles, memory);
            Y_reg = readByte(cycles, memory, abs_address);
            setRegisterFlag(Y_reg);
        } break;

        case INS_LDY_ABSX: {
            Word abs_address = fetchWord(cycles, memory);
            Word absX_address = readByte(cycles, memory, abs_address);
            Y_reg = readByte(cycles, memory, absX_address);
            if (absX_address - abs_address >= 0xFF) {
                cycles--;
            }
            setRegisterFlag(Y_reg);
        } break;


        default: {
            throw InvalidInstructionException(PC);
        }
    }
}


// *** Store Registers ***
// Checks for STA instructions (does not use a clock cycle)
void emulator_6502::CPU::checkSTA(u32& cycles, Memory& memory, Byte& ins) {
    switch (ins) {

    }
}



