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

    Cpu(Rom& rom);
    ~Cpu();

    enum LogicalOperation {
        AND,
        OR,
        XOR,
    };

    // 8 bit arith operations
    uint8_t add8(BinOpt8 arg, bool subtraction, bool carry = false);
    uint8_t logicalOperation8(BinOpt8 arg, LogicalOperation op);
    uint8_t step8(bool increment);
    bool compare8(BinOpt8 arg);
    // 16 bit arith operations
    uint16_t add16(Register16 dest, Register16 operand);
    uint16_t addSpSigned(int8_t operand);
    uint16_t step16(Register16 dest, bool increment);
};