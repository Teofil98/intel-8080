#ifndef INTEL_8080_H
#define INTEL_8080_H

#include <stdint.h>

#define MAX_MEMORY_SIZE 65535

typedef struct cpu_state_t {
    uint8_t registers[8];
    uint8_t flags;
    uint16_t SP, PC; // stack pointer and program counter
    uint8_t memory[MAX_MEMORY_SIZE];
} cpu_state_t;

void cpu_init();
void cpu_step();

#endif // INTEL_8080_H
