#pragma int

#include "rom.h"
#include "register.h"
#include "memory.h"
#include <array>
#include <variant>

class Cpu {
private:
    Registers registers;
    Rom& rom;
    Memory memory;
public:
    /// the value that programs expect the SP to be initialized to
    static constexpr uint16_t STACK_STARTING_VALUE = 0xFFFE;
    /// the pointer to the top of VRAM
    static constexpr uint16_t VRAM_TOP = 0x9FFF;

    Cpu::Cpu(Rom& rom) : registers(Registers()), rom(rom), memory(registers) {}
    Cpu::~Cpu() {}

    enum LogicalOperation {
        AND,
        OR,
        XOR,
    };
    
    //NOTE: These function definitions generally follow the pattern of the reference guide

    // 8 bit transfer operations
    //NOTE: unimplemented for now, left here in case we want to use it for better hardware emulation
    uint8_t load_to_reg8(Register8 dest, BinOpt8 arg);
    uint8_t load_to_mem8(BinOpt8 arg);
    // 16 bit transfer operations
    uint16_t load_to_reg16(Register16 dest, BinOpt16 arg);
    // not implemented for now as the behavior is identical to add_sp_signed, the only difference is how the result is used
    uint16_t ldhl_sp(int8_t operand);

    /// @brief pushes a value onto stack and decrements SP by 2
    /// @param arg register to be stored onto the stack
    /// @return new value of SP 
    uint16_t push(Register16 arg);
    /// @brief pops a value off of stack, stores it in dest and increments SP by 2
    /// @param dest destination regiser
    /// @return new value of SP 
    uint16_t pop(Register16 dest);
    // 8 bit arith operations
    uint8_t add8(BinOpt8 arg, bool subtraction, bool carry = false, BinOpt dest = A);
    uint8_t logical_operation8(BinOpt8 arg, LogicalOperation op);
    uint8_t step8(bool increment);
    bool compare8(BinOpt8 arg);
    // 16 bit arith operations
    uint16_t add16(Register16 dest, Register16 operand);
    uint16_t add_sp_signed(int8_t operand);
    uint16_t step16(Register16 dest, bool increment);
    bool complement_carry_flag();
    bool set_carry_flag();
    uint8_t complement_accumulator();
    uint8_t decimal_adjust_acc();
    // ---- TODO: left unimplemented for now because they're so simple ---
    uint16_t jump(uint16_t addr);
    uint16_t jump_cc(uint16_t addr);
    // ---- unimplemented
    
    void call(uint16_t faddr, bool condition = false);
    void ret(bool condition = false, bool interrupt = false);
};
