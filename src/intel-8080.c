#include "include/intel-8080.h"
#include "include/instructions.h"

#define MAX_MEMORY_SIZE 65535

// registers
static uint8_t registers[8];
static uint8_t flags;

// stack pointer and program counter
static uint16_t SP, PC;

uint8_t memory[MAX_MEMORY_SIZE];

// typedef enum state_t {
//     READY,
//     BUSY
// } state_t;

// static state_t state;

// cycles until current instruction is finished
static int8_t busy_cycles = 0;

void cpu_init()
{
    PC = 0;
    flags = 0b00000010;
}

void cpu_step()
{
    busy_cycles--;
    if(busy_cycles > 0) {
        return;
    }

    uint8_t opcode = memory[PC];

    switch(opcode) {
        case 0b00110110: // Move to memory immediate
            busy_cycles = MVI_mem(registers, memory, memory[PC + 1]);
            PC += 2;
            break;
        case 0b00111010: // Load accumulator direct
            busy_cycles = LDA(registers, memory,
                              ADDRESS(memory[PC + 2], memory[PC + 1]));
            PC += 3;
            break;
        case 0b00110010: // Store accumulator direct
            busy_cycles = STA(registers, memory,
                              ADDRESS(memory[PC + 2], memory[PC + 1]));
            PC += 3;
            break;
        case 0b00101010: // Load H and L direct
            busy_cycles = LHLD(registers, memory,
                               ADDRESS(memory[PC + 2], memory[PC + 1]));
            PC += 3;
            break;
        case 0b00100010: // Store H and L direct
            busy_cycles = SHLD(registers, memory,
                               ADDRESS(memory[PC + 2], memory[PC + 1]));
            PC += 3;
            break;
        case 0b11101011: // Exchange H and L with D and E
            busy_cycles = XCHG(registers);
            PC += 1;
            break;
        case 0b10000110: // Add memory
            busy_cycles = ADD_mem(registers, memory, &flags);
            PC += 1;
            break;
        case 0b11000110: // Add immediate
            busy_cycles = ADD_imm(registers, memory[PC + 1], &flags);
            PC += 2;
            break;
        case 0b10001110: // Add memory with carry
            busy_cycles = ADC_mem(registers, memory, &flags);
            PC += 1;
            break;
        case 0b11001110: // Add immediate with carry
            busy_cycles = ACI(registers, memory[PC + 1], &flags);
            PC += 2;
            break;
        default:
            // TODO: Handle unknown instruction
            break;
    }
}
