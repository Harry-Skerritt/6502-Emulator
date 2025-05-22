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
#include <fstream>
#include <filesystem>
#include <chrono>


namespace  emulator_6502 {

    using Byte = uint8_t;
    using Word = uint16_t;

    void outputByte(Byte value, const std::string& before_text = "", bool error = false);
    void outputWord(Word value, const std::string& before_text = "", bool error = false);

    using u32 = uint32_t;

    class Memory {
    public:
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
        void dumpMemoryToFile(size_t start = 0, size_t length = 256);

    };

    class CPU {
    public:
        Word PC;    // Program Counter
        Byte SP;    // Stack Pointer
        Byte Accumulator;     // Accumulator (register)
        Byte X_reg;     // X Register
        Byte Y_reg;     // Y Register

        struct StatusFlags {
            Byte C : 1; // Carry
            Byte Z : 1; // Zero
            Byte I : 1; // Interrupt Disable
            Byte D : 1; // Decimal Mode
            Byte B : 1; // Break
            Byte V : 1; // Overflow
            Byte N : 1; // Negative
            Byte unused : 1; // Usually bit 5 is unused
        } flags;

        void reset(Memory& memory);

        Byte fetchByte(u32& clock_cycles, Memory& memory);
        //Byte readByte(u32& clock_cycles, Memory& memory, Byte address);
        static Byte readByte(u32& clock_cycles, Memory& memory, Word address);

        Word fetchWord(u32& clock_cycles, Memory& memory);

        static Word readWord(u32& clock_cycles, Memory& memory, Word address);

        void execute(u32 cycles, Memory& memory);

        // *** Load Registers ***
        void setRegisterFlag(Byte& reg);
        void loadRegister(u32& clock_cycles, Memory& memory, Byte& reg);
        void loadZPRegister(u32& clock_cycles, Memory& memory, Byte& reg);
        void loadZPOffsetRegister(u32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);
        void loadAbsRegister(u32& clock_cycles, Memory& memory, Byte& reg);
        void loadAbsOffsetRegister(u32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);

        // LDA Only
        void loadIndirectXRegister(u32& clock_cycles, Memory& memory, Byte& reg);
        void loadIndirectYRegister(u32& clock_cycles, Memory& memory, Byte& reg);
    };

    // Opcode dispatch table
    using InstructionHandler = void (*)(CPU& cpu, u32& cycles, Memory& memory);
    static constexpr int OPCODE_COUNT = 256;
    inline InstructionHandler dispatch_table[OPCODE_COUNT] = { nullptr };

    void initDispatchTable();

    // Wrapper functions - LDA
    inline void handle_LDA_IM(CPU& cpu, u32& cycles, Memory& memory) {
        cpu.loadRegister(cycles, memory, cpu.Accumulator);
    }
    inline void handle_LDA_ZP(CPU& cpu, u32& cycles, Memory& memory) {
        cpu.loadZPRegister(cycles, memory, cpu.Accumulator);
    }
    inline void handle_LDA_ZPX(CPU& cpu, u32& cycles, Memory& memory) {
        cpu.loadZPOffsetRegister(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_LDA_ABS(CPU& cpu, u32& cycles, Memory& memory) {
        cpu.loadAbsRegister(cycles, memory, cpu.Accumulator);
    }
    inline void handle_LDA_ABSX(CPU& cpu, u32& cycles, Memory& memory) {
        cpu.loadAbsOffsetRegister(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_LDA_ABSY(CPU& cpu, u32& cycles, Memory& memory) {
        cpu.loadAbsOffsetRegister(cycles, memory, cpu.Accumulator, cpu.Y_reg);
    }
    inline void handle_LDA_INDX(CPU& cpu, u32& cycles, Memory& memory) {
        cpu.loadIndirectXRegister(cycles, memory, cpu.Accumulator);
    }
    inline void handle_LDA_INDY(CPU& cpu, u32& cycles, Memory& memory) {
        cpu.loadIndirectYRegister(cycles, memory, cpu.Accumulator);
    }

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
