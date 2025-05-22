//
// Created by Harry Skerritt on 22/05/2025.
//

#ifndef EMULATOR_6502_H
#define EMULATOR_6502_H

#include <string>
#include <iostream>
#include <iomanip>
#include <set>
#include <sstream>

namespace  emulator_6502 {

    using Byte = uint8_t;
    using Word = uint16_t;

    void outputByte(Byte value, const std::string& before_text = "", bool error = false);
    void outputWord(Word value, const std::string& before_text = "", bool error = false);

    using u32 = uint32_t;

    struct Memory {
        static const u32 MAX_MEMORY = 1024 * 64;
        Byte data[MAX_MEMORY];


        // Read 1 Byte
        Byte operator[] (u32 address) const {
            return data[address];
        }

        // Write 1 Bytes
        Byte& operator[] (u32 address) {
            return data[address];
        }

        void initMemory();
        //void loadMemory(std::string& loc);

        void dumpMemory(size_t start = 0, size_t length = 256);

    };

    // Opcodes
    static constexpr Byte
        // Load Registers Opcodes
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_LDA_ABS = 0xAD,
        INS_LDA_ABSX = 0xBD,
        INS_LDA_ABSY = 0xB9,
        INS_LDA_INDX = 0xA1,
        INS_LDA_INDY = 0xB1,

        INS_LDX_IM = 0xA2,
        INS_LDX_ZP = 0xA6,
        INS_LDX_ZPY = 0xB6,
        INS_LDX_ABS = 0xAE,
        INS_LDX_ABSY = 0xBE,

        INS_LDY_IM = 0xA0,
        INS_LDY_ZP = 0xA4,
        INS_LDY_ZPX = 0xB4,
        INS_LDY_ABS = 0xAC,
        INS_LDY_ABSX = 0xBC,

        // Store Registers Opcodes
        INS_STA_ZP = 0x85,
        INS_STA_ZPX = 0x95,
        INS_STA_ABS = 0x8D,
        INS_STA_ABSX = 0x9D,
        INS_STA_ABSY = 0x99,
        INS_STA_INDX = 0x81,
        INS_STA_INDY = 0x91;

    struct CPU {

        Word PC;    // Program Counter
        Byte SP;    // Stack Pointer
        Byte Accumulator;     // Accumulator (register)
        Byte X_reg;     // X Register
        Byte Y_reg;     // Y Register

        /*
         N = Negative Flag
         V = Overflow Flag
         B = Break Flag
         I = Interrupt Disabled
         Z = Zero Flag
         C = Carry Flag
         D = Decimal Flag
         */
        Byte N, V, B, I, Z, C, D : 1; // Status Flags

        void reset(Memory& memory);

        Byte fetchByte(u32& clock_cycles, Memory& memory);
        //Byte readByte(u32& clock_cycles, Memory& memory, Byte address);
        Byte readByte(u32& clock_cycles, Memory& memory, Word address);

        Word fetchWord(u32& clock_cycles, Memory& memory);

        Word readWord(u32& clock_cycles, Memory& memory, Word address);

        void execute(u32 cycles, Memory& memory);

        // *** Load Registers ***
        void setRegisterFlag(Byte& reg);
        // LDA
        void checkLDA(u32& cycles, Memory& memory, Byte& ins);
        // LDX
        void checkLDX(u32& cycles, Memory& memory, Byte& ins);
        // LDX
        void checkLDY(u32& cycles, Memory& memory, Byte& ins);

        // *** Store Registers ***
        void checkSTA(u32& cycles, Memory& memory, Byte& ins);
    };


};

class InvalidInstructionException : public std::exception {
    std::string message;

public:
    explicit InvalidInstructionException(uint16_t pc_value) {
        std::ostringstream oss;
        oss << "Invalid instruction at address: 0x"
            << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << pc_value;
        message = oss.str();
    }

    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif //EMULATOR_6502_H
