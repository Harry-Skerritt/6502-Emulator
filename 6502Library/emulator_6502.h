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
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_LDA_ABS = 0xAD,
        INS_LDA_ABSX = 0xBD,
        INS_LDA_ABSY = 0xB9,
        INS_LDA_INDX = 0xA1,
        INS_LDA_INDY = 0xB1;

    struct CPU {

        Word PC;    // Program Counter
        Byte SP;    // Stack Pointer
        Byte A;     // Accumulator (register)
        Byte X;     // X Register
        Byte Y;     // Y Register

        Byte N, V, B, I, Z, C, D : 1; // Status Flags

        void reset(Memory& memory);

        Byte fetchByte(u32& clock_cycles, Memory& memory);
        Byte readByte(u32& clock_cycles, Memory& memory, Byte& address);

        Word fetchWord(u32& clock_cycles, Memory& memory);

        void execute(u32 cycles, Memory& memory);

        // LDA
        void checkLDA(u32& cycles, Memory& memory, Byte& ins);
        void setLDAFlags();
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
