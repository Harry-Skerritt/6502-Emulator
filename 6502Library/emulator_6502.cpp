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

// Initialises the dispatch table to handle opcodes
void emulator_6502::initDispatchTable() {
    // LDA
    dispatch_table[0xA9] = handle_LDA_IM;
    dispatch_table[0xA5] = handle_LDA_ZP;
    dispatch_table[0xB5] = handle_LDA_ZPX;
    dispatch_table[0xAD] = handle_LDA_ABS;
    dispatch_table[0xBD] = handle_LDA_ABSX;
    dispatch_table[0xB9] = handle_LDA_ABSY;
    dispatch_table[0xA1] = handle_LDA_INDX;
    dispatch_table[0xB1] = handle_LDA_INDY;

    // LDX
    dispatch_table[0xA2] = handle_LDX_IM;
    dispatch_table[0xA6] = handle_LDX_ZP;
    dispatch_table[0xB6] = handle_LDX_ZPY;
    dispatch_table[0xAE] = handle_LDX_ABS;
    dispatch_table[0xBE] = handle_LDX_ABSY;

    // LDY
    dispatch_table[0xA0] = handle_LDY_IM;
    dispatch_table[0xA4] = handle_LDY_ZP;
    dispatch_table[0xB4] = handle_LDY_ZPX;
    dispatch_table[0xAC] = handle_LDY_ABS;
    dispatch_table[0xBC] = handle_LDY_ABSX;

}


// Memory

// Initializes memory to 0x0000
void emulator_6502::Memory::initMemory() {
    for (u32 i = 0; i < MAX_MEMORY; i++) {
        data[i] = 0x0000;
    }

    std::cout << "Memory initialized" << std::endl;
}

// Dumps the given memory section to the console
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

// Dumps the given memory section to the a file
void emulator_6502::Memory::dumpMemoryToFile(size_t start, size_t length) {
    namespace fs = std::filesystem;

    // Ensure /dumps/ directory exists
    fs::path dump_dir = "dumps";
    if (!fs::exists(dump_dir)) {
        fs::create_directory(dump_dir);
    }

    // Generate timestamped filename: memorydump_MMDDYYYY_HHMMSS.txt
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&now_c);

    std::ostringstream file_stream;
    file_stream << "memorydump_"
                   << std::setw(2) << std::setfill('0') << tm.tm_mon + 1
                   << std::setw(2) << std::setfill('0') << tm.tm_mday
                   << tm.tm_year + 1900 << "_"
                   << std::setw(2) << std::setfill('0') << tm.tm_hour
                   << std::setw(2) << std::setfill('0') << tm.tm_min
                   << std::setw(2) << std::setfill('0') << tm.tm_sec
                   << ".txt";

    fs::path filePath = dump_dir / file_stream.str();

    std::ofstream file(filePath);
    if (!file) {
        std::cerr << "Failed to open file: " << filePath << "\n";
        return;
    }

    const size_t bytes_per_row = 16;

    for (size_t addr = start; addr < start + length; addr += bytes_per_row) {
        file << std::hex << std::setw(4) << std::setfill('0') << addr << "  ";

        for (size_t i = 0; i < bytes_per_row; ++i) {
            if (addr + i < MAX_MEMORY)
                file << std::setw(2) << std::setfill('0') << static_cast<int>(data[addr + i]) << " ";
            else
                file << "   ";
        }

        file << " ";

        for (size_t i = 0; i < bytes_per_row; ++i) {
            if (addr + i < MAX_MEMORY) {
                char c = static_cast<char>(data[addr + i]);
                file << (std::isprint(c) ? c : '.');
            }
        }

        file << '\n';
    }

    file.close();
    std::cout << "Memory dump saved to: " << filePath << '\n';
}





// CPU

// Sets the 6502 into a reset state
void emulator_6502::CPU::reset(Memory& memory) {
    initDispatchTable();

    PC = 0xFFFC;
    SP = 0x0100;

    Accumulator = X_reg = Y_reg = 0;
    flags = {}; // Resets flags to zero
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
        // Fetch
        Byte instruction = fetchByte(cycles, memory);

        // Decode
        InstructionHandler handler = dispatch_table[instruction];

        // Execute (or handle error)
        if (handler) {
            handler(*this, cycles, memory);
        } else {
            throw InvalidInstructionException(PC -1);
        }
    }

}

// *** Load Registers ***
// Sets the processor status flags for LDA instructions
void emulator_6502::CPU::setRegisterFlag(Byte& reg) {
    flags.Z = (reg == 0);
    flags.N = (reg & 0b1000000) > 0;
}


void emulator_6502::CPU::loadRegister(u32 &clock_cycles, Memory &memory, Byte &reg) {
    reg = fetchByte(clock_cycles, memory);
    setRegisterFlag(reg);
}

void emulator_6502::CPU::loadZPRegister(u32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte zp_addr = fetchByte(clock_cycles, memory);
    reg = readByte(clock_cycles, memory, zp_addr);
    setRegisterFlag(reg);
}

void emulator_6502::CPU::loadZPOffsetRegister(u32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Byte zp_addr = fetchByte(clock_cycles, memory);
    zp_addr += offset;
    clock_cycles--;

    reg = readByte(clock_cycles, memory, zp_addr);
    setRegisterFlag(reg);
}

void emulator_6502::CPU::loadAbsRegister(u32 &clock_cycles, Memory &memory, Byte &reg) {
    Word abs_addr = fetchWord(clock_cycles, memory);
    reg = readByte(clock_cycles, memory, abs_addr);
    setRegisterFlag(reg);
}

void emulator_6502::CPU::loadAbsOffsetRegister(u32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Word abs_addr = fetchWord(clock_cycles, memory);
    Word abs_offset = abs_addr + offset;
    reg = readByte(clock_cycles, memory, abs_offset);

    // Handle page jumping
    if (abs_addr - abs_offset >= 0xFF) {
        clock_cycles--;
    }
}

void emulator_6502::CPU::loadIndirectXRegister(u32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte zp_addr = fetchByte(clock_cycles, memory);
    zp_addr += X_reg;
    clock_cycles--;
    Word useful_addr = readWord(clock_cycles, memory, zp_addr);
    reg = readByte(clock_cycles, memory, useful_addr);
}

void emulator_6502::CPU::loadIndirectYRegister(u32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte zp_addr = fetchByte(clock_cycles, memory);
    Word useful_addr = readWord(clock_cycles, memory, zp_addr);
    Word useful_addr_y = useful_addr + Y_reg;
    reg = readByte(clock_cycles, memory, useful_addr_y);
    if (useful_addr - useful_addr_y >= 0xFF) {
        clock_cycles--;
    }
}






