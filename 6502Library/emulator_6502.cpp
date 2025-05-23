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

    // Jumps and Calls
    dispatch_table[0x4C] = handle_JMP_ABS;
    dispatch_table[0x6c] = handle_JMP_IND;
    dispatch_table[0x20] = handle_JSR;
    dispatch_table[0x60] = handle_RTS;

    // System Functions
    dispatch_table[0xEA] = handle_NOP;
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
        std::cout << "Enter value to initalise memory (press enter for 0x00): ";
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
    flags.C = value & carry_bit;
    flags.Z = value & zero_bit;
    flags.I = value & interrupt_bit;
    flags.D = value & decimal_bit;
    flags.B = value & break_bit;
    flags.unused = value & unused_bit;
    flags.V = value & overflow_bit;
    flags.N = value & negative_bit;
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

// *** Stack Helpers ***
// Convert the stack pointer as a 16-bit address
Word CPU::pointerToAddress() const {
    return 0x100 | SP;
}

// Writes the value to the top of the stack as a 16-bit word
void CPU::pushToStack(s32 &clock_cycles, Memory &memory, Word value) {
    writeByte(clock_cycles, memory, pointerToAddress(), value >> 8);
    SP--;

    writeByte(clock_cycles, memory, pointerToAddress(), value & 0xFF);
    SP--;
}

// Writes the value to the top of the stack as an 8-bit byte
void CPU::pushToStack_8(s32 &clock_cycles, Memory &memory, Word value) {
    memory[pointerToAddress()] = value;
    clock_cycles--;
    SP--;
    clock_cycles--;
}

// Pulls the top most value from the stack and returns it as a 16-bit word
Word CPU::popFromStack(s32 &clock_cycles, Memory &memory) {
    Word stack_value = readWord(clock_cycles, memory, pointerToAddress()+1);
    SP += 2;
    clock_cycles--;
    return stack_value;
}

// Pulls the top most value from the stack and returns it as a 8-bit byte
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

void CPU::loadRegister(s32 &clock_cycles, Memory &memory, Byte &reg) {
    reg = fetchByte(clock_cycles, memory);
    setRegisterFlag(reg);
}

void CPU::loadZPRegister(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Byte zp_addr = fetchByte(clock_cycles, memory);
    reg = readByte(clock_cycles, memory, zp_addr);
    setRegisterFlag(reg);
}

void CPU::loadZPOffsetRegister(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Byte zp_addr = fetchByte(clock_cycles, memory);
    zp_addr += offset;
    clock_cycles--;

    reg = readByte(clock_cycles, memory, zp_addr);
    setRegisterFlag(reg);
}

void CPU::loadAbsRegister(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Word abs_addr = fetchWord(clock_cycles, memory);
    reg = readByte(clock_cycles, memory, abs_addr);
    setRegisterFlag(reg);
}

void CPU::loadAbsOffsetRegister(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Word abs_addr = fetchWord(clock_cycles, memory);
    Word abs_offset = abs_addr + offset;
    reg = readByte(clock_cycles, memory, abs_offset);

    // Handle page jumping
    if (abs_addr - abs_offset >= 0xFF) {
        clock_cycles--;
    }
}

void CPU::loadIndirectXRegister(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Word indirect_addr = getIndirectXAddr(clock_cycles, memory);
    reg = readByte(clock_cycles, memory, indirect_addr);
}

void CPU::loadIndirectYRegister(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Word indirect_addr = getIndirectYAddr(clock_cycles, memory);
    reg = readByte(clock_cycles, memory, indirect_addr);
}


// *** Store Registers ***
void CPU::storeRegisterZP(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Word zp_addr = fetchWord(clock_cycles, memory);
    writeByte(clock_cycles, memory, zp_addr, reg);
}

void CPU::storeRegisterZPOffset(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Word zp_addr = fetchWord(clock_cycles, memory);
    zp_addr += offset;
    clock_cycles--;

    writeByte(clock_cycles, memory, zp_addr, reg);
}

void CPU::storeAbsRegister(s32 &clock_cycles, Memory &memory, Byte &reg) {
    Word abs_addr = fetchWord(clock_cycles, memory);
    writeByte(clock_cycles, memory, abs_addr, reg);
}

void CPU::storeAbsOffsetRegister(s32 &clock_cycles, Memory &memory, Byte &reg, Byte &offset) {
    Word abs_addr = fetchWord(clock_cycles, memory);
    Word abs_offset = abs_addr + offset;
    clock_cycles--;
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
    clock_cycles -= 2; // Todo: This doesnt seem right
}

// Pushes the processor status flags as a byte to the stack
void CPU::pushProcessorStatus(s32 &clock_cycles, Memory &memory) {
    Byte status = packStatusFlags(flags);
    clock_cycles--;
    pushToStack_8(clock_cycles, memory, status);
}

// Pulls an 8-bit value from the stack and puts it into the accumulator
void CPU::pullAccumulator(s32 &clock_cycles, Memory &memory) {
    Byte value = popFromStack_8(clock_cycles, memory);
    Accumulator = value;
    clock_cycles -= 2; // Todo: This doesnt seem right
    setRegisterFlag(Accumulator);
}

void CPU::pullProcessorStatus(s32 &clock_cycles, Memory &memory) {
    Byte status = popFromStack_8(clock_cycles, memory);
    flags = unpackStatusFlags(status);
    clock_cycles -= 2;
}


// *** Jumps and Calls ***
// Sets the program counter to the value in the next byte of memory
void CPU::jumpAbsolute(s32 &clock_cycles, Memory &memory) {
    Word abs_addr = fetchWord(clock_cycles, memory);
    PC = abs_addr;
}

// Sets the program counter to the value stored at the address of the next value in memory
void CPU::jumpIndirect(s32 &clock_cycles, Memory &memory) {
    Word abs_addr = fetchWord(clock_cycles, memory);
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






