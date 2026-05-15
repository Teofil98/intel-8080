#include "include/intel-8080.h"
#include "include/instructions.h"

cpu_state_t g_state;

// cycles until current instruction is finished
static int8_t busy_cycles = 0;

void cpu_init()
{
    g_state.PC = 0;
    g_state.flags = 0b00000010;
}

void cpu_step()
{
    busy_cycles--;
    if(busy_cycles > 0) {
        return;
    }
    uint8_t* memory = g_state.memory;

    uint8_t opcode = g_state.memory[g_state.PC];

    switch(opcode) {
        case 0b00110110: // Move to memory immediate
            busy_cycles = MVI_mem(memory[g_state.PC + 1]);
            g_state.PC += 2;
            break;
        case 0b00111010: // Load accumulator direct
            busy_cycles = LDA(ADDRESS(memory[g_state.PC + 2], memory[g_state.PC + 1]));
            g_state.PC += 3;
            break;
        case 0b00110010: // Store accumulator direct
            busy_cycles = STA(ADDRESS(memory[g_state.PC + 2], memory[g_state.PC + 1]));
            g_state.PC += 3;
            break;
        case 0b00101010: // Load H and L direct
            busy_cycles = LHLD(ADDRESS(memory[g_state.PC + 2], memory[g_state.PC + 1]));
            g_state.PC += 3;
            break;
        case 0b00100010: // Store H and L direct
            busy_cycles = SHLD(ADDRESS(memory[g_state.PC + 2], memory[g_state.PC + 1]));
            g_state.PC += 3;
            break;
        case 0b11101011: // Exchange H and L with D and E
            busy_cycles = XCHG();
            g_state.PC += 1;
            break;
        case 0b10000110: // Add memory
            busy_cycles = ADD_mem();
            g_state.PC += 1;
            break;
        case 0b11000110: // Add immediate
            busy_cycles = ADI(memory[g_state.PC + 1]);
            g_state.PC += 2;
            break;
        case 0b10001110: // Add memory with carry
            busy_cycles = ADC_mem();
            g_state.PC += 1;
            break;
        case 0b11001110: // Add immediate with carry
            busy_cycles = ACI(memory[g_state.PC + 1]);
            g_state.PC += 2;
            break;
        case 0b10010110: // Subtract memory
            busy_cycles = SUB_mem();
            g_state.PC += 1;
            break;
        case 0b11010110:  // Subtract immediate
            busy_cycles = SUI(memory[g_state.PC + 1]);
            g_state.PC += 2;
            break;
        case 0b10011110: // Subtract memory with borrow
            busy_cycles = SBB_mem();
            g_state.PC += 1;
            break;
        case 0b11011110: //  Subtract immediate with borrow
            busy_cycles = SBI(memory[g_state.PC + 1]);
            g_state.PC += 2;
            break;
        case 0b00110100: // Increment memory
            busy_cycles = INR_mem();
            g_state.PC += 1;
            break;
        case 0b00110101: // Decrement memory
            busy_cycles = DCR_mem();
            g_state.PC += 1;
            break;
        case 0b00100111: // Decimal Adjust Accumulator
            busy_cycles = DDA();
            g_state.PC += 1;
            break;
        case 0b10100110: // AND memory
            busy_cycles = ANA_mem();
            g_state.PC += 1;
            break;
        case 0b11100110: // AND immediate
            busy_cycles = ANI(memory[g_state.PC + 1]);
            g_state.PC += 2;
            break;
        case 0b10101110: // XOR memory
            busy_cycles = XRA_mem();
            g_state.PC += 1;
            break;
        case 0b11101110: // XOR immediate
            busy_cycles = XRI(memory[g_state.PC + 1]);
            g_state.PC += 2;
            break;
        case 0b10110110: // OR memory
            busy_cycles = ORA_mem();
            g_state.PC += 1;
            break;
        case 0b11110110: // OR immediate
            busy_cycles = ORI(memory[g_state.PC + 1]);
            g_state.PC += 2;
            break;
        case 0b10111110: // Compare memory
            busy_cycles = CMP_mem();
            g_state.PC += 1;
            break;
        case 0b11111110: // Compare memory immediate
            busy_cycles = CPI(memory[g_state.PC + 1]);
            g_state.PC += 2;
            break;
        case 0b00000111: // Rotate left
            busy_cycles = RLC();
            g_state.PC += 1;
            break;
        case 0b00001111: // Rotate right
            busy_cycles = RRC();
            g_state.PC += 1;
            break;
        case 0b00010111: // Rotate left through carry
            busy_cycles = RAL();
            g_state.PC += 1;
            break;
        case 0b00011111: // Rotate right through carry
            busy_cycles = RAR();
            g_state.PC += 1;
            break;
        case 0b00101111: // Complement accumulator
            busy_cycles = CMA();
            g_state.PC += 1;
            break;
        case 0b00111111: // Complement carry
            busy_cycles = CMC();
            g_state.PC += 1;
            break;
        case 0b00110111: // Set carry
            busy_cycles = STC();
            g_state.PC += 1;
            break;
        case 0b11000011: // Jump
            busy_cycles = JMP(memory[g_state.PC + 1], memory[g_state.PC + 2]);
            break;
        case 0b11001101: // Call
            busy_cycles = CALL(memory[g_state.PC + 1], memory[g_state.PC + 2]);
            break;
        case 0b11001001: // Return
            busy_cycles = RET();
            break;
        case 0b11101001: // Jump H and L indirect
            busy_cycles = PCHL();
            break;
        case 0b11110101: // Push processor status word
            busy_cycles = PUSH_PSW();
            g_state.PC += 1;
            break;
        case 0b11110001:
            busy_cycles = POP_PSW();
            g_state.PC += 1;
            break;
        default:
            // TODO: Handle unknown instruction
            break;
    }
}
