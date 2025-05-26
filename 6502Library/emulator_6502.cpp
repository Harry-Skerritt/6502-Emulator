//
// Created by Harry Skerritt on 22/05/2025.
//

#include "emulator_6502.h"

using namespace emulator_6502;

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
    // Load Registers
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

    // STA
    dispatch_table[0x85] = handle_STA_ZP;
    dispatch_table[0x95] = handle_STA_ZPX;
    dispatch_table[0x8D] = handle_STA_ABS;
    dispatch_table[0x9D] = handle_STA_ABSX;
    dispatch_table[0x99] = handle_STA_ABSY;
    dispatch_table[0x81] = handle_STA_INDX;
    dispatch_table[0x91] = handle_STA_INDY;

    // STX
    dispatch_table[0x86] = handle_STX_ZP;
    dispatch_table[0x96] = handle_STX_ZPY;
    dispatch_table[0x8E] = handle_STX_ABS;

    // STY
    dispatch_table[0x84] = handle_STY_ZP;
    dispatch_table[0x94] = handle_STY_ZPX;
    dispatch_table[0x8C] = handle_STY_ABS;


    // Register Transfers
    dispatch_table[0xAA] = handle_TAX;
    dispatch_table[0xA8] = handle_TAY;
    dispatch_table[0x8A] = handle_TXA;
    dispatch_table[0x98] = handle_TYA;


    // Stack Operations
    dispatch_table[0xBA] = handle_TSX;
    dispatch_table[0x9A] = handle_TXS;
    dispatch_table[0x48] = handle_PHA;
    dispatch_table[0x08] = handle_PHP;
    dispatch_table[0x68] = handle_PLA;
    dispatch_table[0x28] = handle_PLP;


    // Logical
    // AND
    dispatch_table[0x29] = handle_AND_IM;
    dispatch_table[0x25] = handle_AND_ZP;
    dispatch_table[0x35] = handle_AND_ZPX;
    dispatch_table[0x2D] = handle_AND_ABS;
    dispatch_table[0x3D] = handle_AND_ABSX;
    dispatch_table[0x39] = handle_AND_ABSY;
    dispatch_table[0x21] = handle_AND_INDX;
    dispatch_table[0x31] = handle_AND_INDY;

    // EOR
    dispatch_table[0x49] = handle_EOR_IM;
    dispatch_table[0x45] = handle_EOR_ZP;
    dispatch_table[0x55] = handle_EOR_ZPX;
    dispatch_table[0x4D] = handle_EOR_ABS;
    dispatch_table[0x5D] = handle_EOR_ABSX;
    dispatch_table[0x59] = handle_EOR_ABSY;
    dispatch_table[0x41] = handle_EOR_INDX;
    dispatch_table[0x51] = handle_EOR_INDY;

    // IOR
    dispatch_table[0x09] = handle_IOR_IM;
    dispatch_table[0x05] = handle_IOR_ZP;
    dispatch_table[0x15] = handle_IOR_ZPX;
    dispatch_table[0x0D] = handle_IOR_ABS;
    dispatch_table[0x1D] = handle_IOR_ABSX;
    dispatch_table[0x19] = handle_IOR_ABSY;
    dispatch_table[0x01] = handle_IOR_INDX;
    dispatch_table[0x11] = handle_IOR_INDY;

    // Bit Test
    dispatch_table[0x24] = handle_BIT_ZP;
    dispatch_table[0x2C] = handle_BIT_ABS;


    // Arithmetic
    // Addition
    dispatch_table[0x69] = handle_ADC_IM;
    dispatch_table[0x65] = handle_ADC_ZP;
    dispatch_table[0x75] = handle_ADC_ZPX;
    dispatch_table[0x6D] = handle_ADC_ABS;
    dispatch_table[0x7D] = handle_ADC_ABSX;
    dispatch_table[0x79] = handle_ADC_ABSY;
    dispatch_table[0x61] = handle_ADC_INDX;
    dispatch_table[0x71] = handle_ADC_INDY;

    // Subtraction
    dispatch_table[0xE9] = handle_SBC_IM;
    dispatch_table[0xE5] = handle_SBC_ZP;
    dispatch_table[0xF5] = handle_SBC_ZPX;
    dispatch_table[0xED] = handle_SBC_ABS;
    dispatch_table[0xFD] = handle_SBC_ABSX;
    dispatch_table[0xF9] = handle_SBC_ABSY;
    dispatch_table[0xE1] = handle_SBC_INDX;
    dispatch_table[0xF1] = handle_SBC_INDY;

    // Comparisons
    dispatch_table[0xC9] = handle_CMP_IM;
    dispatch_table[0xC5] = handle_CMP_ZP;
    dispatch_table[0xD5] = handle_CMP_ZPX;
    dispatch_table[0xCD] = handle_CMP_ABS;
    dispatch_table[0xDD] = handle_CMP_ABSX;
    dispatch_table[0xD9] = handle_CMP_ABSY;
    dispatch_table[0xC1] = handle_CMP_INDX;
    dispatch_table[0xD1] = handle_CMP_INDY;

    dispatch_table[0xE0] = handle_CPX_IM;
    dispatch_table[0xE4] = handle_CPX_ZP;
    dispatch_table[0xEC] = handle_CPX_ABS;

    dispatch_table[0xC0] = handle_CPY_IM;
    dispatch_table[0xC4] = handle_CPY_ZP;
    dispatch_table[0xCC] = handle_CPY_ABS;


    // Increments and Decrements
    dispatch_table[0xE6] = handle_INC_ZP;
    dispatch_table[0xF6] = handle_INC_ZPX;
    dispatch_table[0xEE] = handle_INC_ABS;
    dispatch_table[0xFE] = handle_INC_ABSX;
    dispatch_table[0xE8] = handle_INX;
    dispatch_table[0xC8] = handle_INY;
    dispatch_table[0xC6] = handle_DEC_ZP;
    dispatch_table[0xD6] = handle_DEC_ZPX;
    dispatch_table[0xCE] = handle_DEC_ABS;
    dispatch_table[0xDE] = handle_DEC_ABSX;
    dispatch_table[0xCA] = handle_DEX;
    dispatch_table[0x88] = handle_DEY;


    // Shifts
    // Arithmetic Shift Left
    dispatch_table[0x0A] = handle_ASL;
    dispatch_table[0x06] = handle_ASL_ZP;
    dispatch_table[0x16] = handle_ASL_ZPX;
    dispatch_table[0x0E] = handle_ASL_ABS;
    dispatch_table[0x1E] = handle_ASL_ABSX;

    // Logical Shift Right
    dispatch_table[0x4A] = handle_LSR;
    dispatch_table[0x46] = handle_LSR_ZP;
    dispatch_table[0x56] = handle_LSR_ZPX;
    dispatch_table[0x4E] = handle_LSR_ABS;
    dispatch_table[0x5E] = handle_LSR_ABSX;

    // Rotate Left
    dispatch_table[0x2A] = handle_ROL;
    dispatch_table[0x26] = handle_ROL_ZP;
    dispatch_table[0x36] = handle_ROL_ZPX;
    dispatch_table[0x2E] = handle_ROL_ABS;
    dispatch_table[0x3E] = handle_ROL_ABSX;

    // Rotate Right
    dispatch_table[0x6A] = handle_ROR;
    dispatch_table[0x66] = handle_ROR_ZP;
    dispatch_table[0x76] = handle_ROR_ZPX;
    dispatch_table[0x6E] = handle_ROR_ABS;
    dispatch_table[0x7E] = handle_ROR_ABSX;


    // Jumps and Calls
    dispatch_table[0x4C] = handle_JMP_ABS;
    dispatch_table[0x6c] = handle_JMP_IND;
    dispatch_table[0x20] = handle_JSR;
    dispatch_table[0x60] = handle_RTS;


    // Branches
    dispatch_table[0x90] = handle_BCC;
    dispatch_table[0xB0] = handle_BCS;
    dispatch_table[0xF0] = handle_BEQ;
    dispatch_table[0x30] = handle_BMI;
    dispatch_table[0xD0] = handle_BNE;
    dispatch_table[0x10] = handle_BPL;
    dispatch_table[0x50] = handle_BVC;
    dispatch_table[0x70] = handle_BVS;


    // Status Flag Changes
    dispatch_table[0x18] = handle_CLC;
    dispatch_table[0xD8] = handle_CLD;
    dispatch_table[0x58] = handle_CLI;
    dispatch_table[0xB8] = handle_CLV;
    dispatch_table[0x38] = handle_SEC;
    dispatch_table[0xF8] = handle_SED;
    dispatch_table[0x78] = handle_SEI;


    // System Functions
    dispatch_table[0x00] = handle_BRK;
    dispatch_table[0xEA] = handle_NOP;
    dispatch_table[0x40] = handle_RTI;
}


// Memory
// Initializes memory to 0x0000
void Memory::initMemory() {
    for (unsigned char & i : data) {
        i = 0;
    }

    std::cout << "Memory initialized" << std::endl;
}

// Sets the whole memory to the value passed in
void Memory::setMemory(Byte to_set) {
    for (unsigned char & i : data) {
        i = to_set;
    }

    outputByte(to_set, "Memory Set to: ");
}

// Loads a .bin file into the memory
bool Memory::loadMemory(std::string &loc) {
    std::ifstream file(loc, std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file: " << loc << std::endl;
        return false;
    }


    file.seekg(0, std::ios::end);
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (file_size > MAX_MEMORY) {
        std::cerr << "File too large! Maximum size is " << MAX_MEMORY << " bytes." << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(data), MAX_MEMORY);
    std::streamsize bytes_read = file.gcount();

    if (bytes_read == 0) {
        std::cerr << "Warning: File is empty or could not be read " << loc << std::endl;
        return false;
    }

    std::cout << "Loaded " << bytes_read << " bytes into memory" << std::endl;
    return true;
}

// Function to help the user in loading memory.
void Memory::promptMemoryLoad() {
    std::string input;
    std::cout << "Would you like to load a file? (y/n): ";
    std::getline(std::cin, input);

    if (!input.empty() && input[0] == 'y' || input[0] == 'Y') {
        std::cout << "Enter full file path: ";
        std::getline(std::cin, input);
        if (!loadMemory(input)) {
            std::cerr << "Failed to load file: " << input << std::endl;
        }
    } else {
        std::cout << "Enter value to initialise memory (press enter for 0x00): ";
        std::getline(std::cin, input);
        if (input.empty()) {
            initMemory();
        } else {
            std::istringstream iss(input);
            int value;
            if ((input.rfind("0x", 0) == 0 || input.rfind("0X", 0) == 0)) {
                iss >> std::hex >> value;
            } else {
                iss >> value;
            }

            if (iss.fail() || value < 0 || value > 255) {
                std::cerr << "Invalid input. Using 0x00." << std::endl;
                value = 0;
            }

            setMemory(static_cast<Byte>(value));
        }
    }
}


// Dumps the given memory section to the console
void Memory::dumpMemory(size_t start, size_t length) {
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

// Dumps the given memory section to the file
void Memory::dumpMemoryToFile(size_t start, size_t length) {
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

    constexpr size_t bytes_per_row = 16;

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

// Write a word to the specified memory address
void Memory::writeWord(s32 &clock_cycles, u32 address, Word value) {
    data[address] = value & 0xFF;
    data[address + 1] = (value >> 8);
    clock_cycles -= 2;
}




// CPU
// Packs the status flags into a byte for writing to stack
Byte CPU::packStatusFlags(StatusFlags flags) {
    Byte status = 0;
    if (flags.C) status |= carry_bit;
    if (flags.Z) status |= zero_bit;
    if (flags.I) status |= interrupt_bit;
    if (flags.D) status |= decimal_bit;
    if (flags.B) status |= break_bit;
    status |= unused_bit;
    if (flags.V) status |= overflow_bit;
    if (flags.N) status |= negative_bit;
    return status;
}

// Unpacks the status flags from a byte
CPU::StatusFlags CPU::unpackStatusFlags(Byte value) {
    StatusFlags flags{};
    flags.C = isBitSet(value, carry_bit);
    flags.Z = isBitSet(value, zero_bit);
    flags.I = isBitSet(value, interrupt_bit);
    flags.D = isBitSet(value, decimal_bit);
    flags.B = isBitSet(value, break_bit);
    flags.unused = isBitSet(value, unused_bit);
    flags.V = isBitSet(value, overflow_bit);
    flags.N = isBitSet(value, negative_bit);
    return flags;
}

// Sets the 6502 into a reset state
void CPU::reset(Memory& memory) {
    initDispatchTable();

    PC = 0xFFFC; // Reset Vector 0xFFFC 0xFFFD
    SP = 0xFF;

    // Gets the address from the reset vector
    Byte start_low = memory[PC];
    Byte start_high = memory[PC+1];
    Word start_addr = start_low | (start_high << 8);

    PC = start_addr; // <- Where to start program from

    Accumulator = X_reg = Y_reg = 0;
    flags = {}; // Resets flags to zero
    flags.unused = 1; // This is always set
    //memory.initMemory();
}

// *** Reading from memory ***
// Gets and returns the Byte value at PC, increments PC
Byte CPU::fetchByte(s32& clock_cycles, Memory& memory) {
    Byte data = memory[PC];
    PC++;

    clock_cycles--;

    return data;
}

// Gets and returns the Byte value at the address, DOES NOT increment PC (Takes byte as address)
/*
 Byte CPU::readByte(s32 &clock_cycles, Memory &memory, Byte address) {
    Byte data = memory[address];
    clock_cycles--;
    return data;
}
*/

// Gets and returns the Byte value at the address, DOES NOT increment PC (Takes word as address)
Byte CPU::readByte(s32 &clock_cycles, Memory &memory, Word address) {
    Byte data = memory[address];
    clock_cycles--;
    return data;
}

// Gets and returns the Word value at PC, increments PC
Word CPU::fetchWord(s32& clock_cycles, Memory& memory) {
    // Note: Little Endian

    Word data = memory[PC];
    PC++;

    data |= (memory[PC] << 8);
    PC++;

    clock_cycles-=2;
    return data;
}

// Gets and returns the Word value at PC, DOES NOT increment PC
Word CPU::readWord(s32& clock_cycles, Memory& memory, Word address) {
    Byte low_byte = readByte(clock_cycles, memory, address);
    Byte high_byte = readByte(clock_cycles, memory, (address + 1));
    return low_byte | (high_byte << 8);
}


// *** Writing to memory ***
// Writes the byte 'value' to the memory address specified
void CPU::writeByte(s32 &clock_cycles, Memory &memory, Word address, Byte value) {
    memory[address] = value;
    clock_cycles--;
}

// Executes the specified cycle amount of cycles on the 6502
void CPU::execute(s32 cycles, Memory& memory) {

    while (cycles > 0) {
        // Fetch
        Byte instruction = fetchByte(cycles, memory);

        // Decode
        InstructionHandler handler = dispatch_table[instruction];

        // Execute (or handle error)
        if (handler) {
            handler(*this, cycles, memory);
        } else {
            memory.dumpMemoryToFile(0, Memory::MAX_MEMORY); // Dump full memory to file
            throw InvalidInstructionException(PC -1);
        }
    }

}


// *** Address Helpers ***
// Gets the indirect addressing method address for the x register
Word CPU::getIndirectXAddr(s32 &clock_cycles, Memory &memory) {
    Byte zp_address = fetchByte(clock_cycles, memory);
    zp_address += X_reg;
    clock_cycles--;
    Word useful_addr = readWord(clock_cycles, memory, zp_address);
    return useful_addr;
}

// Gets the indirect addressing method address for the y register
Word CPU::getIndirectYAddr(s32 &clock_cycles, Memory &memory) {
    Byte zp_address = fetchByte(clock_cycles, memory);
    Word useful_addr = readWord(clock_cycles, memory, zp_address);
    Word useful_y_addr = useful_addr + Y_reg;
    if (useful_y_addr - useful_addr >= 0xFF) {
        clock_cycles--;
    }
    return useful_y_addr;
}

// Gets the absolute addressing method address
Word CPU::getAbsoluteAddr(s32 &clock_cycles, Memory &memory) {
    Word abs_addr = fetchWord(clock_cycles, memory);
    return abs_addr;
}

// Gets the absolute addressing method address + an offset (handles crossing boundary)
Word CPU::getAbsoluteAddrOffset(s32 &clock_cycles, Memory &memory, Byte& offset) {
    Word abs_addr = fetchWord(clock_cycles, memory);
    Word abs_offset = abs_addr + offset;
    const bool page_crossed = (abs_addr ^ abs_offset) >> 8;
    if (page_crossed) {
        clock_cycles--;
    }
    return abs_offset;
}

// Gets the absolute addressing method address + an offset (DOES NOT handle crossing boundary)
Word CPU::getAbsoluteAddrOffset_NP(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Word abs_addr = fetchWord(clock_cycles, memory);
    Word abs_offset = abs_addr + offset;
    clock_cycles--;
    return abs_offset;
}

// Gets the zero point address
Byte CPU::getZPAddr(s32 &clock_cycles, Memory &memory) {
    Byte zp_addr = fetchByte(clock_cycles, memory);
    return zp_addr;
}

// Gets the zero point address + an offset
Byte CPU::getZPAddrOffset(s32 &clock_cycles, Memory &memory, Byte& offset) {
    Byte zp_addr = fetchByte(clock_cycles, memory);
    zp_addr += offset;
    clock_cycles--;
    return zp_addr;
}



// *** Stack Helpers ***
// Convert the stack pointer as a 16-bit address
Word CPU::pointerToAddress() const {
    return 0x100 | SP;
}

// Writes the value to the top of the stack as a 16-bit word (2 CC)
void CPU::pushToStack(s32 &clock_cycles, Memory &memory, Word value) {
    writeByte(clock_cycles, memory, pointerToAddress(), value >> 8);
    SP--;

    writeByte(clock_cycles, memory, pointerToAddress(), value & 0xFF);
    SP--;
}

// Writes the value to the top of the stack as an 8-bit byte (2 CC)
void CPU::pushToStack_8(s32 &clock_cycles, Memory &memory, Word value) {
    memory[pointerToAddress()] = value;
    clock_cycles--;
    SP--;
    clock_cycles--;
}

// Pulls the top most value from the stack and returns it as a 16-bit word (3 CC)
Word CPU::popFromStack(s32 &clock_cycles, Memory &memory) {
    Word stack_value = readWord(clock_cycles, memory, pointerToAddress()+1);
    SP += 2;
    clock_cycles--;
    return stack_value;
}

// Pulls the top most value from the stack and returns it as an 8-bit byte (2 CC)
Byte CPU::popFromStack_8(s32 &clock_cycles, Memory &memory) {
    SP++;
    clock_cycles--;

    Byte stack_value = memory[pointerToAddress()];
    clock_cycles--;
    return stack_value;
}


// *** Load Registers ***
// Sets the processor status flags for LD_ instructions
void CPU::setRegisterFlag(Byte& reg) {
    flags.Z = (reg == 0);
    flags.N = (reg & 0b1000000) > 0;
}

// Loads the specified register with the value at the next memory address
void CPU::loadRegister(s32 &clock_cycles, Memory &memory, Byte &reg) {
    reg = fetchByte(clock_cycles, memory);
    setRegisterFlag(reg);
}

// Loads the specified register with the value at the next memory address in ZP
void CPU::loadZPRegister(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte zp_addr = getZPAddr(clock_cycles, memory);
    reg = readByte(clock_cycles, memory, zp_addr);
    setRegisterFlag(reg);
}

// Loads the specified register with the value at the next memory address in ZP + offset
void CPU::loadZPOffsetRegister(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Byte zp_addr = getZPAddrOffset(clock_cycles, memory, offset);

    reg = readByte(clock_cycles, memory, zp_addr);
    setRegisterFlag(reg);
}

// Loads the specified register with the value at the next memory address (Absolute addressing)
void CPU::loadAbsRegister(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Word abs_addr = getAbsoluteAddr(clock_cycles, memory);
    reg = readByte(clock_cycles, memory, abs_addr);
    setRegisterFlag(reg);
}

// Loads the specified register with the value at the next memory address (Absolute addressing) + offset
void CPU::loadAbsOffsetRegister(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Word abs_offset = getAbsoluteAddrOffset(clock_cycles, memory, offset);
    reg = readByte(clock_cycles, memory, abs_offset);
}

// Loads the specified register with the value at the next memory address (Indirect X addressing mode)
void CPU::loadIndirectXRegister(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Word indirect_addr = getIndirectXAddr(clock_cycles, memory);
    reg = readByte(clock_cycles, memory, indirect_addr);
}

// Loads the specified register with the value at the next memory address (Indirect Y addressing mode)
void CPU::loadIndirectYRegister(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Word indirect_addr = getIndirectYAddr(clock_cycles, memory);
    reg = readByte(clock_cycles, memory, indirect_addr);
}


// *** Store Registers ***
void CPU::storeRegisterZP(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Word zp_addr = getZPAddr(clock_cycles, memory);
    writeByte(clock_cycles, memory, zp_addr, reg);
}

void CPU::storeRegisterZPOffset(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Word zp_addr = fetchWord(clock_cycles, memory);
    zp_addr += offset;
    clock_cycles--;
    writeByte(clock_cycles, memory, zp_addr, reg);
}

void CPU::storeAbsRegister(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Word abs_addr = getAbsoluteAddr(clock_cycles, memory);
    writeByte(clock_cycles, memory, abs_addr, reg);
}

void CPU::storeAbsOffsetRegister(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Word abs_offset = getAbsoluteAddrOffset_NP(clock_cycles, memory, offset);
    writeByte(clock_cycles, memory, abs_offset, reg);
}

void CPU::storeRegisterIndirectX(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Word indirect_addr = getIndirectXAddr(clock_cycles, memory);
    writeByte(clock_cycles, memory, indirect_addr, reg);
}

void CPU::storeRegisterIndirectY(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Word indirect_addr = getIndirectYAddr(clock_cycles, memory);
    writeByte(clock_cycles, memory, indirect_addr, reg);
}

// *** Register Transfers ***
// Transfers the value from 'reg_from' to 'reg_to' (2 clock cycles)
void CPU::transferRegister(s32 &clock_cycles, Memory &memory, Byte &reg_from, Byte &reg_to) {
    Byte reg1_val = readByte(clock_cycles, memory, reg_from);
    writeByte(clock_cycles, memory, reg_to, reg1_val);
    setRegisterFlag(reg_to);
}


// *** Stack Operations ***
// Pushes the 8-bit value of the accumulator to the Stack as a 16 bit value
void CPU::pushAccumulator(s32 &clock_cycles, Memory &memory) {
    pushToStack_8(clock_cycles, memory, Accumulator);
}

// Pushes the processor status flags as a byte to the stack
void CPU::pushProcessorStatus(s32 &clock_cycles, Memory &memory) {
    Byte status = packStatusFlags(flags);
    pushToStack_8(clock_cycles, memory, status);
}

// Pulls an 8-bit value from the stack and puts it into the accumulator
void CPU::pullAccumulator(s32 &clock_cycles, Memory &memory) {
    Byte value = popFromStack_8(clock_cycles, memory);
    Accumulator = value;
    clock_cycles --;
    setRegisterFlag(Accumulator);
}

// Pulls the processor stats flags from the stack and assigns them
void CPU::pullProcessorStatus(s32 &clock_cycles, Memory &memory) {
    Byte status = popFromStack_8(clock_cycles, memory);
    flags = unpackStatusFlags(status);
    flags.unused = 1;
    clock_cycles --;
}


// *** Logical ***
// BITWISE AND
// Performs a logical bitwise AND on the register specified with the value in the memory
void CPU::bitwiseAndIM(s32 &clock_cycles, Memory &memory, Byte& reg) {
    Byte value = fetchByte(clock_cycles, memory);
    reg &= value;
    setRegisterFlag(reg);
}

// Performs a logical bitwise AND on the register specified with the value in the memory from ZP
void CPU::bitwiseAndZP(s32& clock_cycles, Memory &memory, Byte& reg) {
    Byte zp_addr = getZPAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    reg &= value;
    setRegisterFlag(reg);
}

// Performs a logical bitwise AND on the register specified with the value in the memory from ZP + offset
void CPU::bitwiseAndZPOffset(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Byte zp_addr = getZPAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    reg &= value;
}

// Performs a logical bitwise AND on the register specified with the value in the memory from Abs
void CPU::bitwiseAndAbs(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte abs_addr = getAbsoluteAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    reg &= value;
    setRegisterFlag(reg);
}

// Performs a logical bitwise AND on the register specified with the value in the memory from Abs + offset
void CPU::bitwiseAndAbsOffset(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Byte abs_addr = getAbsoluteAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    reg &= value;
    setRegisterFlag(reg);
}

// Performs a logical bitwise AND on the register specified with the value in the memory (Indirect X Addr Mode)
void CPU::bitwiseAndIndirectX(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte indirect_addr = getIndirectXAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, indirect_addr);
    reg &= value;
    setRegisterFlag(reg);
}

// Performs a logical bitwise AND on the register specified with the value in the memory (Indirect Y Addr Mode)
void CPU::bitwiseAndIndirectY(s32& clock_cycles, Memory& memory, Byte& reg) {
    Byte indirect_addr = getIndirectYAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, indirect_addr);
    reg &= value;
    setRegisterFlag(reg);
}

// Exclusive OR
// An exclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory.
void CPU::exclusiveORIM(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte value = fetchByte(clock_cycles, memory);
    reg ^= value;
    setRegisterFlag(reg);
}

// An exclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory ZP addr
void CPU::exclusiveORZP(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte zp_addr = getZPAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    reg ^= value;
    setRegisterFlag(reg);
}

// An exclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory ZP addr + offset
void CPU::exclusiveORZPOffset(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Byte zp_addr = getZPAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    reg ^= value;
    setRegisterFlag(reg);
}

// An exclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory absolute addressing mode
void CPU::exclusiveORAbs(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte abs_addr = getAbsoluteAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    reg ^= value;
    setRegisterFlag(reg);
}

// An exclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory absolute addressing mode + offset
void CPU::exclusiveORAbsOffset(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Byte abs_addr = getAbsoluteAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    reg ^= value;
    setRegisterFlag(reg);
}

// An exclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory indirect x addressing mode
void CPU::exclusiveORIndirectX(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte indirect_addr = getIndirectXAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, indirect_addr);
    reg ^= value;
    setRegisterFlag(reg);
}

// An exclusive OR is performed, bit by bit, on the accumulator contents using the contents of a byte of memory indirect y addressing mode
void CPU::exclusiveORIndirectY(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte indirect_addr = getIndirectYAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, indirect_addr);
    reg ^= value;
    setRegisterFlag(reg);
}

// Inclusive OR
void CPU::inclusiveORIM(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte value = fetchByte(clock_cycles, memory);
    reg |= value;
    setRegisterFlag(reg);
}

void CPU::inclusiveORZP(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte zp_addr = getZPAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    reg |= value;
    setRegisterFlag(reg);
}

void CPU::inclusiveORZPOffset(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Byte zp_addr = getZPAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    reg |= value;
    setRegisterFlag(reg);
}

void CPU::inclusiveORAbs(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte abs_addr = getAbsoluteAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    reg |= value;
    setRegisterFlag(reg);
}

void CPU::inclusiveORAbsOffset(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Byte abs_addr = getAbsoluteAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    reg |= value;
    setRegisterFlag(reg);
}

void CPU::inclusiveORIndirectX(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte indirect_addr = getIndirectXAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, indirect_addr);
    reg |= value;
    setRegisterFlag(reg);
}

void CPU::inclusiveORIndirectY(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte indirect_addr = getIndirectYAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, indirect_addr);
    reg |= value;
    setRegisterFlag(reg);
}

// Bit Test
// Performs a Bit Test And and sets registers
void CPU::performBitTest(Byte& reg, Byte& value) {
    Byte result = reg & value;

    flags.Z = result ? 0 : 1;
    flags.N = isBitSet(reg, 0x80) ? 1 : 0;
    flags.V = isBitSet(reg, 0x40) ? 1 : 0;
}

// This instruction is used to test if one or more bits are set in a target memory location. The mask pattern in A is ANDed with the value in memory to set or clear the zero flag, but the result is not kept. Bits 7 and 6 of the value from memory are copied into the N and V flags.
void CPU::bitTestZP(s32 &clock_cycles, Memory &memory) {
    Byte zp_addr = getZPAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    performBitTest(Accumulator, value);
}

// This instruction is used to test if one or more bits are set in a target memory location. The mask pattern in A is ANDed with the value in memory to set or clear the zero flag, but the result is not kept. Bits 7 and 6 of the value from memory are copied into the N and V flags.
void CPU::bitTestABS(s32 &clock_cycles, Memory &memory) {
    Byte abs_addr = getAbsoluteAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    performBitTest(Accumulator, value);
}


// *** Arithmetic ***
// ADC
// Adds the value in 'value' to the Accumulator whilst taking into account the carry flag
void CPU::additionWithCarry(Byte &value) {
    Byte sum = Accumulator + value + flags.C;

    bool overflow = (~(Accumulator ^ value) & (Accumulator ^ sum)) & 0x80;

    flags.C = sum > 0xFF;
    flags.V = overflow ? 1 : 0;
    Accumulator = sum;
    setRegisterFlag(Accumulator);
}

// This instruction adds the contents of a memory location to the accumulator together with the carry bit. If overflow occurs the carry bit is set, this enables multiple byte addition to be performed.
void CPU::additionWithCarryIM(s32 &clock_cycles, Memory &memory) {
    Byte value = fetchByte(clock_cycles, memory);
    additionWithCarry(value);
}

// This instruction adds the contents of a memory location to the accumulator together with the carry bit. If overflow occurs the carry bit is set, this enables multiple byte addition to be performed.
void CPU::additionWithCarryZP(s32 &clock_cycles, Memory &memory) {
    Byte zp_addr = getZPAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    additionWithCarry(value);
}

// This instruction adds the contents of a memory location to the accumulator together with the carry bit. If overflow occurs the carry bit is set, this enables multiple byte addition to be performed.
void CPU::additionWithCarryZPOffset(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Byte zp_addr = getZPAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    additionWithCarry(value);
}

// This instruction adds the contents of a memory location to the accumulator together with the carry bit. If overflow occurs the carry bit is set, this enables multiple byte addition to be performed.
void CPU::additionWithCarryAbs(s32 &clock_cycles, Memory &memory) {
    Byte abs_addr = getAbsoluteAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    additionWithCarry(value);
}

// This instruction adds the contents of a memory location to the accumulator together with the carry bit. If overflow occurs the carry bit is set, this enables multiple byte addition to be performed.
void CPU::additionWithCarryAbsOffset(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Byte abs_addr = getAbsoluteAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    additionWithCarry(value);
}

// This instruction adds the contents of a memory location to the accumulator together with the carry bit. If overflow occurs the carry bit is set, this enables multiple byte addition to be performed.
void CPU::additionWithCarryIndirectX(s32 &clock_cycles, Memory &memory) {
    Byte indirect_addr = getIndirectXAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, indirect_addr);
    additionWithCarry(value);
}

// This instruction adds the contents of a memory location to the accumulator together with the carry bit. If overflow occurs the carry bit is set, this enables multiple byte addition to be performed.
void CPU::additionWithCarryIndirectY(s32 &clock_cycles, Memory &memory) {
    Byte indirect_addr = getIndirectYAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, indirect_addr);
    additionWithCarry(value);
}

// SBC
// Subtracts the value in 'value' from the Accumulator whilst taking into account the carry flag
void CPU::subtractionWithCarry(Byte &value) {
    value = ~value;
    additionWithCarry(value);

}

// This instruction subtracts the contents of a memory location to the accumulator together with the not of the carry bit. If overflow occurs the carry bit is clear, this enables multiple byte subtraction to be performed.
void CPU::subtractionWithCarryIM(s32 &clock_cycles, Memory &memory) {
    Byte value = fetchByte(clock_cycles, memory);
    subtractionWithCarry(value);
}

// This instruction subtracts the contents of a memory location to the accumulator together with the not of the carry bit. If overflow occurs the carry bit is clear, this enables multiple byte subtraction to be performed.
void CPU::subtractionWithCarryZP(s32 &clock_cycles, Memory &memory) {
    Byte zp_addr = getZPAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    subtractionWithCarry(value);
}

 // This instruction subtracts the contents of a memory location to the accumulator together with the not of the carry bit. If overflow occurs the carry bit is clear, this enables multiple byte subtraction to be performed.
void CPU::subtractionWithCarryZPOffset(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Byte zp_addr = getZPAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    subtractionWithCarry(value);
}

// This instruction subtracts the contents of a memory location to the accumulator together with the not of the carry bit. If overflow occurs the carry bit is clear, this enables multiple byte subtraction to be performed.
void CPU::subtractionWithCarryAbs(s32 &clock_cycles, Memory &memory) {
    Byte abs_addr = getAbsoluteAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    subtractionWithCarry(value);
}

// This instruction subtracts the contents of a memory location to the accumulator together with the not of the carry bit. If overflow occurs the carry bit is clear, this enables multiple byte subtraction to be performed.
void CPU::subtractionWithCarryAbsOffset(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Byte abs_addr = getAbsoluteAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    subtractionWithCarry(value);
}

// This instruction subtracts the contents of a memory location to the accumulator together with the not of the carry bit. If overflow occurs the carry bit is clear, this enables multiple byte subtraction to be performed.
void CPU::subtractionWithCarryIndirectX(s32 &clock_cycles, Memory &memory) {
    Byte indirect_addr = getIndirectXAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, indirect_addr);
    subtractionWithCarry(value);
}

// This instruction subtracts the contents of a memory location to the accumulator together with the not of the carry bit. If overflow occurs the carry bit is clear, this enables multiple byte subtraction to be performed.
void CPU::subtractionWithCarryIndirectY(s32 &clock_cycles, Memory &memory) {
    Byte indirect_addr = getIndirectYAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, indirect_addr);
    subtractionWithCarry(value);
}

// CMP & CPX & CPY
// Sets the flags for register comparisons
void CPU::setComparisonFlags(Byte &reg, Byte &value) {
    Byte result = reg - value;
    flags.C = reg >= value;
    flags.Z = reg == value;
    flags.N = ((result & 0x80) != 0);
}

// This instruction compares the contents of the X register with another memory held value and sets the zero and carry flags as appropriate.
void CPU::compareRegisterIM(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte value = fetchByte(clock_cycles, memory);
    setComparisonFlags(reg, value);
}

// This instruction compares the contents of the X register with another memory held value and sets the zero and carry flags as appropriate.
void CPU::compareRegisterZP(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte zp_addr = getZPAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    setComparisonFlags(reg, value);
}

// This instruction compares the contents of the X register with another memory held value and sets the zero and carry flags as appropriate.
void CPU::compareRegisterZPOffset(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Byte zp_addr = getZPAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    setComparisonFlags(reg, value);
}

// This instruction compares the contents of the X register with another memory held value and sets the zero and carry flags as appropriate.
void CPU::compareRegisterABS(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte abs_addr = getAbsoluteAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    setComparisonFlags(reg, value);
}

// This instruction compares the contents of the X register with another memory held value and sets the zero and carry flags as appropriate.
void CPU::compareRegisterAbsOffset(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Byte abs_addr = getAbsoluteAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    setComparisonFlags(reg, value);
}

//This instruction compares the contents of the X register with another memory held value and sets the zero and carry flags as appropriate.
void CPU::compareRegisterIndirectX(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte indirect_addr = getIndirectXAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, indirect_addr);
    setComparisonFlags(reg, value);
}

//This instruction compares the contents of the X register with another memory held value and sets the zero and carry flags as appropriate.
void CPU::compareRegisterIndirectY(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte indirect_addr = getIndirectYAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, indirect_addr);
    setComparisonFlags(reg, value);
}


// *** Increments and Decrements ***
// Adds 1 to the specified register and assigns flags
void CPU::incrementRegister(s32 &clock_cycles, Memory &memory, Byte& reg) {
    reg++;
    setRegisterFlag(reg);
    clock_cycles--;
}

// Takes 1 from the specified register and assigns flags
void CPU::decrementRegister(s32 &clock_cycles, Memory &memory, Byte& reg) {
    reg--;
    setRegisterFlag(reg);
    clock_cycles--;
}

// Adds one to the value held at a specified memory location setting the zero and negative flags as appropriate.
void CPU::changeMemoryZP(s32 &clock_cycles, Memory &memory, bool inc) {
    Word zp_addr = fetchByte(clock_cycles, memory);
    Byte zp_value = readByte(clock_cycles, memory, zp_addr);

    if (inc) {
        zp_value++;
    } else {
        zp_value--;
    }

    clock_cycles--;
    writeByte(clock_cycles, memory, zp_addr, zp_value);
    setRegisterFlag(zp_value);
}

// Adds one to the value held at a specified memory location + offset setting the zero and negative flags as appropriate.
void CPU::changeMemoryZPOffset(s32 &clock_cycles, Memory &memory, Byte &offset, bool inc) {
    Byte zp_addr = fetchByte(clock_cycles, memory);
    zp_addr += offset;
    clock_cycles--;

    Byte zp_value = readByte(clock_cycles, memory, zp_addr);
    if (inc) {
        zp_value++;
    } else {
        zp_value--;
    }

    clock_cycles--;
    writeByte(clock_cycles, memory, zp_addr, zp_value);
    setRegisterFlag(zp_value);
}

// Adds one to the value held at a specified absolute memory location
void CPU::changeMemoryAbs(s32 &clock_cycles, Memory &memory, bool inc) {
    Word abs_addr = getAbsoluteAddr(clock_cycles, memory);
    Byte abs_value = readByte(clock_cycles, memory, abs_addr);
    if (inc) {
        abs_value++;
    } else {
        abs_value--;
    }
    writeByte(clock_cycles, memory, abs_addr, abs_value);
    setRegisterFlag(abs_value);
}

// Adds one to the value held at a specified absolute memory location + an offset
void CPU::changeMemoryAbsOffset(s32 &clock_cycles, Memory &memory, Byte &offset, bool inc) {
    Word abs_addr = getAbsoluteAddrOffset_NP(clock_cycles, memory, offset);
    Byte abs_value = readByte(clock_cycles, memory, abs_addr);
    if (inc) {
        abs_value++;
    } else {
        abs_value--;
    }
    clock_cycles--;
    writeByte(clock_cycles, memory, abs_addr, abs_value);
    setRegisterFlag(abs_value);
}


// *** Shifts ***
// Arithmetic Shift Left
// Shifts the value left 1 bit and sets the appropriate flags
void CPU::arithmeticShiftLeft(s32& clock_cycles, Byte &value) {
    flags.C = (value & 0x80) != 0;
    value <<= 1;
    setRegisterFlag(value);
    clock_cycles--;
}

//This operation shifts all the bits of the accumulator or memory contents one bit left. Bit 0 is set to 0 and bit 7 is placed in the carry flag. The effect of this operation is to multiply the memory contents by 2 (ignoring 2's complement considerations), setting the carry if the result will not fit in 8 bits.
void CPU::arithmeticShiftLeftZP(s32 &clock_cycles, Memory &memory) {
    Byte zp_addr = getZPAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    arithmeticShiftLeft(clock_cycles, value);
    clock_cycles--;
}

// This operation shifts all the bits of the accumulator or memory contents one bit left. Bit 0 is set to 0 and bit 7 is placed in the carry flag. The effect of this operation is to multiply the memory contents by 2 (ignoring 2's complement considerations), setting the carry if the result will not fit in 8 bits.
void CPU::arithmeticShiftLeftZPOffset(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Byte zp_addr = getZPAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    arithmeticShiftLeft(clock_cycles, value);
    clock_cycles--;
}

// This operation shifts all the bits of the accumulator or memory contents one bit left. Bit 0 is set to 0 and bit 7 is placed in the carry flag. The effect of this operation is to multiply the memory contents by 2 (ignoring 2's complement considerations), setting the carry if the result will not fit in 8 bits.
void CPU::arithmeticShiftLeftABS(s32 &clock_cycles, Memory &memory) {
    Byte abs_addr = getAbsoluteAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    arithmeticShiftLeft(clock_cycles, value);
    clock_cycles--;
}

// This operation shifts all the bits of the accumulator or memory contents one bit left. Bit 0 is set to 0 and bit 7 is placed in the carry flag. The effect of this operation is to multiply the memory contents by 2 (ignoring 2's complement considerations), setting the carry if the result will not fit in 8 bits.
void CPU::arithmeticShiftLeftAbsOffset(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Byte abs_addr = getAbsoluteAddrOffset_NP(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    arithmeticShiftLeft(clock_cycles, value);
    clock_cycles--;
}

// Logical Shift Right
// Shifts the value right 1 bit and sets the appropriate flags
void CPU::logicalShiftRight(s32 &clock_cycles, Byte &value) {
    flags.C = (value & 0x01) != 0;
    value >>= 1;
    setRegisterFlag(value);
    clock_cycles--;
}

//Each of the bits in A or M is shift one place to the right. The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
void CPU::logicalShiftRightZP(s32 &clock_cycles, Memory &memory) {
    Byte zp_addr = getZPAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    logicalShiftRight(clock_cycles, value);
    clock_cycles--;
}

//Each of the bits in A or M is shift one place to the right. The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
void CPU::logicalShiftRightZPOffset(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Byte zp_addr = getZPAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    logicalShiftRight(clock_cycles, value);
    clock_cycles--;
}

//Each of the bits in A or M is shift one place to the right. The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
void CPU::logicalShiftRightABS(s32 &clock_cycles, Memory &memory) {
    Byte abs_addr = getAbsoluteAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    logicalShiftRight(clock_cycles, value);
    clock_cycles--;
}

//Each of the bits in A or M is shift one place to the right. The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
void CPU::logicalShiftRightAbsOffset(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Byte abs_addr = getAbsoluteAddrOffset_NP(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    logicalShiftRight(clock_cycles, value);
    clock_cycles--;
}

// Rotate Left
// Shifts bits left by 1, MSB becomes C flag, C flag become B0
void CPU::rotateLeft(s32 &clock_cycles, Byte &value) {
    bool prevCarryFlag = flags.C;
    flags.C = (value & 0x80) != 0;
    value = (value << 1) | (prevCarryFlag ? 1 : 0);
    setRegisterFlag(value);
    clock_cycles--;
}

// Move each of the bits in either A or M one place to the left. Bit 0 is filled with the current value of the carry flag whilst the old bit 7 becomes the new carry flag value.
void CPU::rotateLeftZP(s32 &clock_cycles, Memory &memory) {
    Byte zp_addr = getZPAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    rotateLeft(clock_cycles, value);
    clock_cycles--;
}

// Move each of the bits in either A or M one place to the left. Bit 0 is filled with the current value of the carry flag whilst the old bit 7 becomes the new carry flag value.
void CPU::rotateLeftZPOffset(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Byte zp_addr = getZPAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    rotateLeft(clock_cycles, value);
    clock_cycles--;
}

// Move each of the bits in either A or M one place to the left. Bit 0 is filled with the current value of the carry flag whilst the old bit 7 becomes the new carry flag value.
void CPU::rotateLeftABS(s32 &clock_cycles, Memory &memory) {
    Byte abs_addr = getAbsoluteAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    rotateLeft(clock_cycles, value);
    clock_cycles--;
}

// Move each of the bits in either A or M one place to the left. Bit 0 is filled with the current value of the carry flag whilst the old bit 7 becomes the new carry flag value.
void CPU::rotateLeftAbsOffset(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Byte abs_addr = getAbsoluteAddrOffset_NP(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    rotateLeft(clock_cycles, value);
    clock_cycles--;
}

// Rotate Right
// Shifts bits right by 1, LSB become C flag, C flag become B7
void CPU::rotateRight(s32 &clock_cycles, Byte &value) {
    bool prevCarryFlag = flags.C;
    flags.C = (value & 0x80) != 0;
    value = (value >> 1) | (prevCarryFlag ? 0x80 : 0);
    setRegisterFlag(value);
    clock_cycles--;
}

//Move each of the bits in either A or M one place to the right. Bit 7 is filled with the current value of the carry flag whilst the old bit 0 becomes the new carry flag value.
void CPU::rotateRightZP(s32 &clock_cycles, Memory &memory) {
    Byte zp_addr = getZPAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    rotateRight(clock_cycles, value);
    clock_cycles--;
}

//Move each of the bits in either A or M one place to the right. Bit 7 is filled with the current value of the carry flag whilst the old bit 0 becomes the new carry flag value.
void CPU::rotateRightZPOffset(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Byte zp_addr = getZPAddrOffset(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, zp_addr);
    rotateRight(clock_cycles, value);
    clock_cycles--;
}

//Move each of the bits in either A or M one place to the right. Bit 7 is filled with the current value of the carry flag whilst the old bit 0 becomes the new carry flag value.
void CPU::rotateRightABS(s32 &clock_cycles, Memory &memory) {
    Byte abs_addr = getAbsoluteAddr(clock_cycles, memory);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    rotateRight(clock_cycles, value);
    clock_cycles--;
}

//Move each of the bits in either A or M one place to the right. Bit 7 is filled with the current value of the carry flag whilst the old bit 0 becomes the new carry flag value.
void CPU::rotateRightAbsOffset(s32 &clock_cycles, Memory &memory, Byte &offset) {
    Byte abs_addr = getAbsoluteAddrOffset_NP(clock_cycles, memory, offset);
    Byte value = readByte(clock_cycles, memory, abs_addr);
    rotateRight(clock_cycles, value);
    clock_cycles--;
}


// *** Jumps and Calls ***
// Sets the program counter to the value in the next byte of memory
void CPU::jumpAbsolute(s32 &clock_cycles, Memory &memory) {
    Word abs_addr = getAbsoluteAddr(clock_cycles, memory);
    PC = abs_addr;
}

// Sets the program counter to the value stored at the address of the next value in memory
void CPU::jumpIndirect(s32 &clock_cycles, Memory &memory) {
    Word abs_addr = getAbsoluteAddr(clock_cycles, memory);
    abs_addr = readWord(clock_cycles, memory, abs_addr);
    PC = abs_addr;
}

// Jumps to a subroutine defined at the next memory address by utilising the stack
void CPU::jumpToSubroutine(s32 &clock_cycles, Memory &memory) {
    Word subroutine_addr = fetchWord(clock_cycles, memory);
    pushToStack(clock_cycles, memory, PC-1);
    PC = subroutine_addr;
    clock_cycles--;
}

// Returns to the point before a subroutine
void CPU::returnFromSubroutine(s32 &clock_cycles, Memory &memory) {
    Word return_addr = popFromStack(clock_cycles, memory);
    PC = return_addr + 1;
    clock_cycles -= 2;
}


// *** Branches ***
// Todo: Note: If one is wrong they're ALL wrong!
// If the carry flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
void CPU::branchCarryClear(s32 &clock_cycles, Memory &memory) {
    Byte value = fetchByte(clock_cycles, memory);

    if (!isBitSet(flags.C, carry_bit)) {
        // Carry bit is 0 -> Branch happens
        Byte new_pc = PC + value;

        if ((PC & 0xFF00) != (new_pc & 0xFF00)) {
            // Page Crossed
            clock_cycles--;
        }

        clock_cycles--;
        PC = new_pc;
    }
}

// If the carry flag is set then add the relative displacement to the program counter to cause a branch to a new location.
void CPU::branchCarrySet(s32 &clock_cycles, Memory &memory) {
    Byte value = fetchByte(clock_cycles, memory);

    if (isBitSet(flags.C, carry_bit)) {
        // Carry bit is 1 -> Branch happens
        Byte new_pc = PC + value;

        if ((PC & 0xFF00) != (new_pc & 0xFF00)) {
            // Page Crossed
            clock_cycles--;
        }

        clock_cycles--;
        PC = new_pc;
    }
}

// If the zero flag is set then add the relative displacement to the program counter to cause a branch to a new location.
void CPU::branchIfEqual(s32 &clock_cycles, Memory &memory) {
    Byte value = fetchByte(clock_cycles, memory);

    if (isBitSet(flags.Z, zero_bit)) {
        // Zero flag is set -> branch happens
        Byte new_pc = PC + value;

        if ((PC & 0xFF00) != (new_pc & 0xFF00)) {
            // Page Crossed
            clock_cycles--;
        }

        clock_cycles--;
        PC = new_pc;
    }
}

// If the negative flag is set then add the relative displacement to the program counter to cause a branch to a new location.
void CPU::branchIfMinus(s32 &clock_cycles, Memory &memory) {
    Byte value = fetchByte(clock_cycles, memory);

    if (isBitSet(flags.N, negative_bit)) {
        // Negative flag is set -> branch happens
        Byte new_pc = PC + value;

        if ((PC & 0xFF00) != (new_pc & 0xFF00)) {
            // Page Crossed
            clock_cycles--;
        }

        clock_cycles--;
        PC = new_pc;
    }
}

// If the zero flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
void CPU::branchNotEqual(s32 &clock_cycles, Memory &memory) {
    Byte value = fetchByte(clock_cycles, memory);

    if (!isBitSet(flags.Z, zero_bit)) {
        // Zero flag is not set -> branch happens
        Byte new_pc = PC + value;

        if ((PC & 0xFF00) != (new_pc & 0xFF00)) {
            // Page Crossed
            clock_cycles--;
        }

        clock_cycles--;
        PC = new_pc;
    }
}

// If the negative flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
void CPU::branchIfPositive(s32 &clock_cycles, Memory &memory) {
    Byte value = fetchByte(clock_cycles, memory);

    if (!isBitSet(flags.N, negative_bit)) {
        // Negative flag is not set -> branch happens
        Byte new_pc = PC + value;

        if ((PC & 0xFF00) != (new_pc & 0xFF00)) {
            // Page Crossed
            clock_cycles--;
        }

        clock_cycles--;
        PC = new_pc;
    }
}

// If the overflow flag is clear then add the relative displacement to the program counter to cause a branch to a new location.
void CPU::branchIfOverflowClear(s32 &clock_cycles, Memory &memory) {
    Byte value = fetchByte(clock_cycles, memory);

    if (!isBitSet(flags.V, overflow_bit)) {
        // Overflow flag is not set -> branch happens
        Byte new_pc = PC + value;

        if ((PC & 0xFF00) != (new_pc & 0xFF00)) {
            // Page Crossed
            clock_cycles--;
        }

        clock_cycles--;
        PC = new_pc;
    }
}

// If the overflow flag is set then add the relative displacement to the program counter to cause a branch to a new location.
void CPU::branchIfOverflowSet(s32 &clock_cycles, Memory &memory) {
    Byte value = fetchByte(clock_cycles, memory);


    if (isBitSet(flags.V, overflow_bit)) {
        // Overflow flag is set -> branch happens
        Byte new_pc = PC + value;

        if ((PC & 0xFF00) != (new_pc & 0xFF00)) {
            // Page Crossed
            clock_cycles--;
        }

        clock_cycles--;
        PC = new_pc;
    }
}


// *** Status Flag Changes ***
// Sets the specified flag to 0
Byte CPU::clearFlag(s32 &clock_cycles, Memory &memory) {
    clock_cycles--;
    return 0;
}

// Sets the specified flag to 1
Byte CPU::setFlag(s32 &clock_cycles, Memory &memory) {
    clock_cycles--;
    return 1;
}


// *** System Functions ***
// The BRK instruction forces the generation of an interrupt request. The program counter and processor status are pushed on the stack then the IRQ interrupt vector at $FFFE/F is loaded into the PC and the break flag in the status set to one.
void CPU::forceInterrupt(s32 &clock_cycles, Memory &memory) {
    // 1 Byte instruction so need to + PC
    PC++;

    // Push PC to stack
    pushToStack(clock_cycles, memory, PC-1);

    // Push Status Flags to stack
    Byte status = packStatusFlags(flags);
    pushToStack_8(clock_cycles, memory, status);

    // Set flags
    flags.B = 1;
    flags.unused = 1;

    // Load PC to value from interrupt vector
    Byte ir_low = readByte(clock_cycles, memory, 0xFFFE);
    Byte ir_high = readByte(clock_cycles, memory, 0xFFFF);
    PC = (ir_high << 8) | ir_low;
}

// The RTI instruction is used at the end of an interrupt processing routine. It pulls the processor flags from the stack followed by the program counter.
void CPU::returnFromInterrupt(s32 &clock_cycles, Memory &memory) {
    // Read Status Flags +2
    Byte status = popFromStack_8(clock_cycles, memory);
    flags = unpackStatusFlags(status);
    flags.unused = 1;

    // Read PC +2
    Word return_addr = popFromStack(clock_cycles, memory);
    PC = return_addr;

    clock_cycles --;
}
