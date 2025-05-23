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
    using s32 = signed int;

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
        void setMemory(Byte to_set);
        bool loadMemory(std::string& loc);

        void promptMemoryLoad();

        // Memory Dumps
        void dumpMemory(size_t start = 0, size_t length = 256);
        void dumpMemoryToFile(size_t start = 0, size_t length = 256);

        // Writing
        void writeWord(s32& clock_cycles, u32 address, Word value);


    };

    class CPU {
    public:
        Word PC;    // Program Counter
        Byte SP;    // Stack Pointer
        Byte Accumulator;     // Accumulator (register)
        Byte X_reg;     // X Register
        Byte Y_reg;     // Y Register

        // *** Staus Flags ***
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

        static constexpr Byte
            carry_bit     = 0b00000001, // Bit 0
            zero_bit      = 0b00000010, // Bit 1
            interrupt_bit = 0b00000100, // Bit 2
            decimal_bit   = 0b00001000, // Bit 3
            break_bit     = 0b00010000, // Bit 4
            unused_bit    = 0b00100000, // Bit 5
            overflow_bit  = 0b01000000, // Bit 6
            negative_bit  = 0b10000000; // Bit 7

        static Byte packStatusFlags(StatusFlags flags);
        static StatusFlags unpackStatusFlags(Byte value);

        // Reset
        void reset(Memory& memory);

        // Reading
        Byte fetchByte(s32& clock_cycles, Memory& memory);
        //Byte readByte(s32& clock_cycles, Memory& memory, Byte address);
        static Byte readByte(s32& clock_cycles, Memory& memory, Word address);

        Word fetchWord(s32& clock_cycles, Memory& memory);
        static Word readWord(s32& clock_cycles, Memory& memory, Word address);

        // Writing
        static void writeByte(s32& clock_cycles, Memory& memory, Word address, Byte value);


        void execute(s32 cycles, Memory& memory);

        // *** Address Helpers ***
        Word getIndirectXAddr(s32& clock_cycles, Memory& memory);
        Word getIndirectYAddr(s32& clock_cycles, Memory& memory);

        // *** Stack Helpers ***
        Word pointerToAddress() const;
        void pushToStack(s32& clock_cycles, Memory& memory, Word value);
        void pushToStack_8(s32& clock_cycles, Memory& memory, Word value);
        Word popFromStack(s32& clock_cycles, Memory& memory);
        Byte popFromStack_8(s32& clock_cycles, Memory& memory);


        // *** Load Registers ***
        void setRegisterFlag(Byte& reg);
        void loadRegister(s32& clock_cycles, Memory& memory, Byte& reg);
        void loadZPRegister(s32& clock_cycles, Memory& memory, Byte& reg);
        void loadZPOffsetRegister(s32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);
        void loadAbsRegister(s32& clock_cycles, Memory& memory, Byte& reg);
        void loadAbsOffsetRegister(s32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);

        // LDA Only
        void loadIndirectXRegister(s32& clock_cycles, Memory& memory, Byte& reg);
        void loadIndirectYRegister(s32& clock_cycles, Memory& memory, Byte& reg);


        // *** Store Registers ***
        void storeRegisterZP(s32& clock_cycles, Memory& memory, Byte& reg);
        void storeRegisterZPOffset(s32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);
        void storeAbsRegister(s32& clock_cycles, Memory& memory, Byte& reg);
        void storeAbsOffsetRegister(s32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);

        // STA Only
        void storeRegisterIndirectX(s32& clock_cycles, Memory& memory, Byte& reg);
        void storeRegisterIndirectY(s32& clock_cycles, Memory& memory, Byte& reg);

        // *** Register Transfers / SP Transfers ***
        void transferRegister(s32& clock_cycles, Memory& memory, Byte& reg_from, Byte& reg_to);

        // *** Stack Operations ***
        void pushAccumulator(s32& clock_cycles, Memory& memory);
        void pushProcessorStatus(s32& clock_cycles, Memory& memory);
        void pullAccumulator(s32& clock_cycles, Memory& memory);
        void pullProcessorStatus(s32& clock_cycles, Memory& memory);

        // *** Jumps & Calls ***
        void jumpAbsolute(s32& clock_cycles, Memory& memory);
        void jumpIndirect(s32& clock_cycles, Memory& memory);
        void jumpToSubroutine(s32& clock_cycles, Memory& memory);
        void returnFromSubroutine(s32& clock_cycles, Memory& memory);
    };

    // Opcode dispatch table
    using InstructionHandler = void (*)(CPU& cpu, s32& cycles, Memory& memory);
    static constexpr int OPCODE_COUNT = 256;
    inline InstructionHandler dispatch_table[OPCODE_COUNT] = { nullptr };

    void initDispatchTable();

    // Wrapper functions - LDA
    inline void handle_LDA_IM(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadRegister(cycles, memory, cpu.Accumulator);
    }
    inline void handle_LDA_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadZPRegister(cycles, memory, cpu.Accumulator);
    }
    inline void handle_LDA_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadZPOffsetRegister(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_LDA_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadAbsRegister(cycles, memory, cpu.Accumulator);
    }
    inline void handle_LDA_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadAbsOffsetRegister(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_LDA_ABSY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadAbsOffsetRegister(cycles, memory, cpu.Accumulator, cpu.Y_reg);
    }
    inline void handle_LDA_INDX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadIndirectXRegister(cycles, memory, cpu.Accumulator);
    }
    inline void handle_LDA_INDY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadIndirectYRegister(cycles, memory, cpu.Accumulator);
    }

    // Wrapper functions - LDX
    inline void handle_LDX_IM(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadRegister(cycles, memory, cpu.X_reg);
    }
    inline void handle_LDX_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadZPRegister(cycles, memory, cpu.X_reg);
    }
    inline void handle_LDX_ZPY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadZPOffsetRegister(cycles, memory, cpu.X_reg, cpu.Y_reg);
    }
    inline void handle_LDX_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadAbsRegister(cycles, memory, cpu.X_reg);
    }
    inline void handle_LDX_ABSY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadAbsOffsetRegister(cycles, memory, cpu.X_reg, cpu.Y_reg);
    }

    // Wrapper functions - LDY
    inline void handle_LDY_IM(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadRegister(cycles, memory, cpu.Y_reg);
    }
    inline void handle_LDY_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadZPRegister(cycles, memory, cpu.Y_reg);
    }
    inline void handle_LDY_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadZPOffsetRegister(cycles, memory, cpu.Y_reg, cpu.X_reg);
    }
    inline void handle_LDY_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadAbsRegister(cycles, memory, cpu.Y_reg);
    }
    inline void handle_LDY_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadAbsOffsetRegister(cycles, memory, cpu.Y_reg, cpu.X_reg);
    }

    // Wrapper functions - STA
    inline void handle_STA_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.storeRegisterZP(cycles, memory, cpu.Accumulator);
    }
    inline void handle_STA_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.storeRegisterZPOffset(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_STA_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.storeAbsRegister(cycles, memory, cpu.Accumulator);
    }
    inline void handle_STA_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.storeAbsOffsetRegister(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_STA_ABSY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.storeAbsOffsetRegister(cycles, memory, cpu.Accumulator, cpu.Y_reg);
    }
    inline void handle_STA_INDX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadIndirectXRegister(cycles, memory, cpu.Accumulator);
    }
    inline void handle_STA_INDY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.loadIndirectYRegister(cycles, memory, cpu.Accumulator);
    }

    // Wrapper functions - STX
    inline void handle_STX_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.storeRegisterZP(cycles, memory, cpu.X_reg);
    }
    inline void handle_STX_ZPY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.storeRegisterZPOffset(cycles, memory, cpu.X_reg, cpu.Y_reg);
    }
    inline void handle_STX_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.storeAbsRegister(cycles, memory, cpu.X_reg);
    }

    // Wrapper functions - STY
    inline void handle_STY_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.storeRegisterZP(cycles, memory, cpu.Y_reg);
    }
    inline void handle_STY_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.storeRegisterZPOffset(cycles, memory, cpu.Y_reg, cpu.X_reg);
    }
    inline void handle_STY_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.storeAbsRegister(cycles, memory, cpu.Y_reg);
    }

    // Wrapper functions - Transfer Registers
    inline void handle_TAX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.transferRegister(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_TAY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.transferRegister(cycles, memory, cpu.Accumulator, cpu.Y_reg);
    }
    inline void handle_TXA(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.transferRegister(cycles, memory, cpu.X_reg, cpu.Accumulator);
    }
    inline void handle_TYA(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.transferRegister(cycles, memory, cpu.Y_reg, cpu.Accumulator);
    }

    // Wrapper functions - Stack Pointer Operations
    inline void handle_TSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.transferRegister(cycles, memory, cpu.SP, cpu.X_reg);
    }
    inline void handle_TXS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.transferRegister(cycles, memory, cpu.X_reg, cpu.SP);
    }
    inline void handle_PHA(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.pushAccumulator(cycles, memory);
    }
    inline void handle_PHP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.pushProcessorStatus(cycles, memory);
    }
    inline void handle_PLA(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.pullAccumulator(cycles, memory);
    }
    inline void handle_PLP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.pullProcessorStatus(cycles, memory);
    }


    // Wrapper functions - Jumps and Calls
    inline void handle_JMP_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.jumpAbsolute(cycles, memory);
    }
    inline void handle_JMP_IND(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.jumpIndirect(cycles, memory);
    }
    inline void handle_JSR(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.jumpToSubroutine(cycles, memory);
    }
    inline void handle_RTS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.returnFromSubroutine(cycles, memory);
    }

    // Wrapper function - System Functions
    inline void handle_NOP(CPU& cpu, s32& cycles, Memory& memory) {
        cycles--;
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
