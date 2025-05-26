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
        static constexpr u32 MAX_MEMORY = 1024 * 64;
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
        Word PC;              // Program Counter
        Byte SP;              // Stack Pointer
        Byte Accumulator;     // Accumulator (register)
        Byte X_reg;           // X Register
        Byte Y_reg;           // Y Register

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

        Word getAbsoluteAddr(s32& clock_cycles, Memory& memory);
        Word getAbsoluteAddrOffset(s32& clock_cycles, Memory& memory, Byte& offset);
        Word getAbsoluteAddrOffset_NP(s32& clock_cycles, Memory& memory, Byte& offset);

        Byte getZPAddr(s32& clock_cycles, Memory& memory);
        Byte getZPAddrOffset(s32& clock_cycles, Memory& memory, Byte& offset);

        // *** Stack Helpers ***
        [[nodiscard]] Word pointerToAddress() const;
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

        // *** Logical ***
        // AND
        void bitwiseAndIM(s32& clock_cycles, Memory& memory, Byte& reg);
        void bitwiseAndZP(s32& clock_cycles, Memory& memory, Byte& reg);
        void bitwiseAndZPOffset(s32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);
        void bitwiseAndAbs(s32& clock_cycles, Memory& memory, Byte& reg);
        void bitwiseAndAbsOffset(s32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);
        void bitwiseAndIndirectX(s32& clock_cycles, Memory& memory, Byte& reg);
        void bitwiseAndIndirectY(s32& clock_cycles, Memory& memory, Byte& reg);

        // Exclusive OR
        void exclusiveORIM(s32& clock_cycles, Memory& memory, Byte& reg);
        void exclusiveORZP(s32& clock_cycles, Memory& memory, Byte& reg);
        void exclusiveORZPOffset(s32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);
        void exclusiveORAbs(s32& clock_cycles, Memory& memory, Byte& reg);
        void exclusiveORAbsOffset(s32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);
        void exclusiveORIndirectX(s32& clock_cycles, Memory& memory, Byte& reg);
        void exclusiveORIndirectY(s32& clock_cycles, Memory& memory, Byte& reg);

        // Inclusive OR
        void inclusiveORIM(s32& clock_cycles, Memory& memory, Byte& reg);
        void inclusiveORZP(s32& clock_cycles, Memory& memory, Byte& reg);
        void inclusiveORZPOffset(s32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);
        void inclusiveORAbs(s32& clock_cycles, Memory& memory, Byte& reg);
        void inclusiveORAbsOffset(s32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);
        void inclusiveORIndirectX(s32& clock_cycles, Memory& memory, Byte& reg);
        void inclusiveORIndirectY(s32& clock_cycles, Memory& memory, Byte& reg);

        // Bit Test
        void performBitTest(Byte& reg, Byte& value);
        void bitTestZP(s32& clock_cycles, Memory& memory);
        void bitTestABS(s32& clock_cycles, Memory& memory);


        // *** Arithmetic ***

        // Compare A, X, Y
        void setComparisonFlags(Byte& reg, Byte& value);
        void compareRegisterIM(s32& clock_cycles, Memory& memory, Byte& reg);
        void compareRegisterZP(s32& clock_cycles, Memory& memory, Byte& reg);
        void compareRegisterZPOffset(s32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);
        void compareRegisterABS(s32& clock_cycles, Memory& memory, Byte& reg);
        void compareRegisterAbsOffset(s32& clock_cycles, Memory& memory, Byte& reg, Byte& offset);
        void compareRegisterIndirectX(s32& clock_cycles, Memory& memory, Byte& reg);
        void compareRegisterIndirectY(s32& clock_cycles, Memory& memory, Byte& reg);

        // *** Increments and Decrements ***
        void incrementRegister(s32& clock_cycles, Memory& memory, Byte& reg);
        void decrementRegister(s32& clock_cycles, Memory& memory, Byte& reg);
        void changeMemoryZP(s32& clock_cycles, Memory& memory, bool inc = true);
        void changeMemoryZPOffset(s32& clock_cycles, Memory& memory, Byte& offset, bool inc = true);
        void changeMemoryAbs(s32& clock_cycles, Memory& memory, bool inc = true);
        void changeMemoryAbsOffset(s32& clock_cycles, Memory& memory, Byte& offset, bool inc = true);

        // *** Shifts ***
        // ASL
        void arithmeticShiftLeft(s32& clock_cycles, Byte& value);
        void arithmeticShiftLeftZP(s32& clock_cycles, Memory& memory);
        void arithmeticShiftLeftZPOffset(s32& clock_cycles, Memory& memory, Byte& offset);
        void arithmeticShiftLeftABS(s32& clock_cycles, Memory& memory);
        void arithmeticShiftLeftAbsOffset(s32& clock_cycles, Memory& memory, Byte& offset);

        // LSR
        void logicalShiftRight(s32& clock_cycles, Byte& value);
        void logicalShiftRightZP(s32& clock_cycles, Memory& memory);
        void logicalShiftRightZPOffset(s32& clock_cycles, Memory& memory, Byte& offset);
        void logicalShiftRightABS(s32& clock_cycles, Memory& memory);
        void logicalShiftRightAbsOffset(s32& clock_cycles, Memory& memory, Byte& offset);

        // ROL
        void rotateLeft(s32& clock_cycles, Byte& value);
        void rotateLeftZP(s32& clock_cycles, Memory& memory);
        void rotateLeftZPOffset(s32& clock_cycles, Memory& memory, Byte& offset);
        void rotateLeftABS(s32& clock_cycles, Memory& memory);
        void rotateLeftAbsOffset(s32& clock_cycles, Memory& memory, Byte& offset);

        // ROR
        void rotateRight(s32& clock_cycles, Byte& value);
        void rotateRightZP(s32& clock_cycles, Memory& memory);
        void rotateRightZPOffset(s32& clock_cycles, Memory& memory, Byte& offset);
        void rotateRightABS(s32& clock_cycles, Memory& memory);
        void rotateRightAbsOffset(s32& clock_cycles, Memory& memory, Byte& offset);

        // *** Jumps & Calls ***
        void jumpAbsolute(s32& clock_cycles, Memory& memory);
        void jumpIndirect(s32& clock_cycles, Memory& memory);
        void jumpToSubroutine(s32& clock_cycles, Memory& memory);
        void returnFromSubroutine(s32& clock_cycles, Memory& memory);

        // *** Branches ***
        void branchCarryClear(s32& clock_cycles, Memory& memory);
        void branchCarrySet(s32& clock_cycles, Memory& memory);
        void branchIfEqual(s32& clock_cycles, Memory& memory);
        void branchIfMinus(s32& clock_cycles, Memory& memory);
        void branchNotEqual(s32& clock_cycles, Memory& memory);
        void branchIfPositive(s32& clock_cycles, Memory& memory);
        void branchIfOverflowClear(s32& clock_cycles, Memory& memory);
        void branchIfOverflowSet(s32& clock_cycles, Memory& memory);

        // *** Status Flag Changes ***
        static Byte clearFlag(s32& clock_cycles, Memory& memory);
        static Byte setFlag(s32& clock_cycles, Memory& memory);

        // *** System Functions ***
        void forceInterrupt(s32& clock_cycles, Memory& memory);
        void returnFromInterrupt(s32& clock_cycles, Memory& memory);

    };

    // Returns t/f based on whether a bit is set
    inline bool isBitSet(Byte value, Byte bitmask) {
        return (value & bitmask) != 0;
    }

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

    // Wrapper functions - Logical
    // AND
    inline void handle_AND_IM(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.bitwiseAndIM(cycles, memory, cpu.Accumulator);
    }
    inline void handle_AND_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.bitwiseAndZP(cycles, memory, cpu.Accumulator);
    }
    inline void handle_AND_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.bitwiseAndZPOffset(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_AND_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.bitwiseAndAbs(cycles, memory, cpu.Accumulator);
    }
    inline void handle_AND_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.bitwiseAndAbsOffset(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_AND_ABSY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.bitwiseAndAbsOffset(cycles, memory, cpu.Accumulator, cpu.Y_reg);
    }
    inline void handle_AND_INDX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.bitwiseAndIndirectX(cycles, memory, cpu.Accumulator);
    }
    inline void handle_AND_INDY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.bitwiseAndIndirectY(cycles, memory, cpu.Accumulator);
    }

    // Exclusive OR
    inline void handle_EOR_IM(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.exclusiveORIM(cycles, memory, cpu.Accumulator);
    }
    inline void handle_EOR_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.exclusiveORZP(cycles, memory, cpu.Accumulator);
    }
    inline void handle_EOR_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.exclusiveORZPOffset(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_EOR_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.exclusiveORAbs(cycles, memory, cpu.Accumulator);
    }
    inline void handle_EOR_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.exclusiveORAbsOffset(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_EOR_ABSY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.exclusiveORAbsOffset(cycles, memory, cpu.Accumulator, cpu.Y_reg);
    }
    inline void handle_EOR_INDX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.exclusiveORIndirectX(cycles, memory, cpu.Accumulator);
    }
    inline void handle_EOR_INDY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.exclusiveORIndirectY(cycles, memory, cpu.Accumulator);
    }

    // Inclusive OR
    inline void handle_IOR_IM(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.inclusiveORIM(cycles, memory, cpu.Accumulator);
    }
    inline void handle_IOR_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.inclusiveORZP(cycles, memory, cpu.Accumulator);
    }
    inline void handle_IOR_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.inclusiveORZPOffset(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_IOR_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.inclusiveORAbs(cycles, memory, cpu.Accumulator);
    }
    inline void handle_IOR_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.exclusiveORAbsOffset(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_IOR_ABSY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.exclusiveORAbsOffset(cycles, memory, cpu.Accumulator, cpu.Y_reg);
    }
    inline void handle_IOR_INDX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.inclusiveORIndirectX(cycles, memory, cpu.Accumulator);
    }
    inline void handle_IOR_INDY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.inclusiveORIndirectY(cycles, memory, cpu.Accumulator);
    }

    // Bit test
    inline void handle_BIT_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.bitTestZP(cycles, memory);
    }
    inline void handle_BIT_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.bitTestABS(cycles, memory);
    }


    // Wrapper functions - Arithmetic

    // Comparisons
    inline void handle_CMP_IM(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterIM(cycles, memory, cpu.Accumulator);
    }
    inline void handle_CMP_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterZP(cycles, memory, cpu.Accumulator);
    }
    inline void handle_CMP_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterZPOffset(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_CMP_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterABS(cycles, memory, cpu.Accumulator);
    }
    inline void handle_CMP_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterAbsOffset(cycles, memory, cpu.Accumulator, cpu.X_reg);
    }
    inline void handle_CMP_ABSY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterAbsOffset(cycles, memory, cpu.Accumulator, cpu.Y_reg);
    }
    inline void handle_CMP_INDX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterIndirectX(cycles, memory, cpu.Accumulator);
    }
    inline void handle_CMP_INDY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterIndirectY(cycles, memory, cpu.Accumulator);
    }

    inline void handle_CPX_IM(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterIM(cycles, memory, cpu.X_reg);
    }
    inline void handle_CPX_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterZP(cycles, memory, cpu.X_reg);
    }
    inline void handle_CPX_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterABS(cycles, memory, cpu.X_reg);
    }

    inline void handle_CPY_IM(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterIM(cycles, memory, cpu.Y_reg);
    }
    inline void handle_CPY_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterZP(cycles, memory, cpu.Y_reg);
    }
    inline void handle_CPY_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.compareRegisterABS(cycles, memory, cpu.Y_reg);
    }

    // Wrapper functions - Increments and decrements
    inline void handle_INC_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.changeMemoryZP(cycles, memory);
    }
    inline void handle_INC_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.changeMemoryZPOffset(cycles, memory, cpu.X_reg);
    }
    inline void handle_INC_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.changeMemoryAbs(cycles, memory);
    }
    inline void handle_INC_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.changeMemoryAbsOffset(cycles, memory, cpu.X_reg);
    }
    inline void handle_INX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.incrementRegister(cycles, memory, cpu.X_reg);
    }
    inline void handle_INY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.incrementRegister(cycles, memory, cpu.Y_reg);
    }
    inline void handle_DEC_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.changeMemoryZP(cycles, memory, false);
    }
    inline void handle_DEC_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.changeMemoryZPOffset(cycles, memory, cpu.X_reg, false);
    }
    inline void handle_DEC_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.changeMemoryAbs(cycles, memory, false);
    }
    inline void handle_DEC_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.changeMemoryAbsOffset(cycles, memory, cpu.X_reg, false);
    }
    inline void handle_DEX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.decrementRegister(cycles, memory, cpu.X_reg);
    }
    inline void handle_DEY(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.decrementRegister(cycles, memory, cpu.Y_reg);
    }

    // Wrapper functions - Shifts
    inline void handle_ASL(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.arithmeticShiftLeft(cycles, cpu.Accumulator);
    }
    inline void handle_ASL_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.arithmeticShiftLeftZP(cycles, memory);
    }
    inline void handle_ASL_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.arithmeticShiftLeftZPOffset(cycles, memory, cpu.X_reg);
    }
    inline void handle_ASL_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.arithmeticShiftLeftABS(cycles, memory);
    }
    inline void handle_ASL_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.arithmeticShiftLeftAbsOffset(cycles, memory, cpu.X_reg);
    }

    inline void handle_LSR(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.logicalShiftRight(cycles, cpu.Accumulator);
    }
    inline void handle_LSR_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.logicalShiftRightZP(cycles, memory);
    }
    inline void handle_LSR_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.logicalShiftRightZPOffset(cycles, memory, cpu.X_reg);
    }
    inline void handle_LSR_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.logicalShiftRightABS(cycles, memory);
    }
    inline void handle_LSR_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.logicalShiftRightAbsOffset(cycles, memory, cpu.X_reg);
    }

    inline void handle_ROL(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.rotateLeft(cycles, cpu.Accumulator);
    }
    inline void handle_ROL_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.rotateLeftZP(cycles, memory);
    }
    inline void handle_ROL_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.rotateLeftZPOffset(cycles, memory, cpu.X_reg);
    }
    inline void handle_ROL_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.rotateLeftABS(cycles, memory);
    }
    inline void handle_ROL_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.rotateLeftAbsOffset(cycles, memory, cpu.X_reg);
    }

    inline void handle_ROR(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.rotateRight(cycles, cpu.Accumulator);
    }
    inline void handle_ROR_ZP(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.rotateRightZP(cycles, memory);
    }
    inline void handle_ROR_ZPX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.rotateRightZPOffset(cycles, memory, cpu.X_reg);
    }
    inline void handle_ROR_ABS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.rotateRightABS(cycles, memory);
    }
    inline void handle_ROR_ABSX(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.rotateRightAbsOffset(cycles, memory, cpu.X_reg);
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

    // Wrapper functions - Branches
    inline void handle_BCC(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.branchCarryClear(cycles, memory);
    }
    inline void handle_BCS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.branchCarrySet(cycles, memory);
    }
    inline void handle_BEQ(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.branchIfEqual(cycles, memory);
    }
    inline void handle_BMI(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.branchIfMinus(cycles, memory);
    }
    inline void handle_BNE(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.branchNotEqual(cycles, memory);
    }
    inline void handle_BPL(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.branchIfPositive(cycles, memory);
    }
    inline void handle_BVC(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.branchIfOverflowClear(cycles, memory);
    }
    inline void handle_BVS(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.branchIfOverflowSet(cycles, memory);
    }


    // Wrapper functions - Status Flag Changes
    inline void handle_CLC(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.flags.C = emulator_6502::CPU::clearFlag(cycles, memory);
    }
    inline void handle_CLD(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.flags.D = emulator_6502::CPU::clearFlag(cycles, memory);
    }
    inline void handle_CLI(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.flags.I = emulator_6502::CPU::clearFlag(cycles, memory);
    }
    inline void handle_CLV(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.flags.V = emulator_6502::CPU::clearFlag(cycles, memory);
    }
    inline void handle_SEC(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.flags.C = emulator_6502::CPU::setFlag(cycles, memory);
    }
    inline void handle_SED(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.flags.D = emulator_6502::CPU::setFlag(cycles, memory);
    }
    inline void handle_SEI(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.flags.I = emulator_6502::CPU::setFlag(cycles, memory);
    }

    // Wrapper function - System Functions
    inline void handle_BRK(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.forceInterrupt(cycles, memory);
    }
    inline void handle_NOP(CPU& cpu, s32& cycles, Memory& memory) {
        cycles--;
    }
    inline void handle_RTI(CPU& cpu, s32& cycles, Memory& memory) {
        cpu.returnFromInterrupt(cycles, memory);
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

    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif //EMULATOR_6502_H
