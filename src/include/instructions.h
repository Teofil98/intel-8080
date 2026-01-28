#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "definitions.h"
#include <stdint.h>
#include <stdbool.h>

inline uint8_t update_flags(const uint8_t result, const uint8_t A,
                            const uint8_t B,const bool addition,
                            uint8_t flags, const uint8_t mask)
{
    if(HAS_FLAG_SET(mask, ZERO_FLAG)) {
        flags = result == 0 ? SET_FLAG(flags, ZERO_FLAG)
                             : CLEAR_FLAG(flags, ZERO_FLAG);

    }

    if(HAS_FLAG_SET(mask, SIGN_FLAG)) {
        flags = (result >> 7) == 1 ? SET_FLAG(flags, SIGN_FLAG)
                                      : CLEAR_FLAG(flags, SIGN_FLAG);
    }

    if(HAS_FLAG_SET(mask, PARITY_FLAG)) {
        flags = (result % 2) == 0 ? SET_FLAG(flags, PARITY_FLAG)
                                     : CLEAR_FLAG(flags, PARITY_FLAG);
    }

    if(HAS_FLAG_SET(mask, CARRY_FLAG) && addition) {
        flags = ((A + B) > 255) ? SET_FLAG(flags, CARRY_FLAG)
                                   : CLEAR_FLAG(flags, CARRY_FLAG);
    }

    if(HAS_FLAG_SET(mask, CARRY_FLAG) && (!addition)) {
        flags = (A < B) ? SET_FLAG(flags, CARRY_FLAG)
                           : CLEAR_FLAG(flags, CARRY_FLAG);
    }

    if(HAS_FLAG_SET(mask, AUXILIARY_CARRY_FLAG) && addition) {
        flags = (((A & 0x0F) + (B & 0x0F)) > 15)
                       ? SET_FLAG(flags, AUXILIARY_CARRY_FLAG)
                       : CLEAR_FLAG(flags, AUXILIARY_CARRY_FLAG);
    }

    if(HAS_FLAG_SET(mask, AUXILIARY_CARRY_FLAG) && (!addition)) {
        flags = ((A & 0x0F) < (B & 0x0F))
                       ? SET_FLAG(flags, AUXILIARY_CARRY_FLAG)
                       : CLEAR_FLAG(flags, AUXILIARY_CARRY_FLAG);
    }

    return flags;
}

// NOTE: Instructions return the number of cycles they take

inline uint8_t MVI_mem(const uint8_t* registers, uint8_t* memory,
                      const uint8_t imm)
{
    uint16_t address = ADDRESS(registers[REG_H], registers[REG_L]);
    memory[address] = imm;
    return 10;
}

inline uint8_t LDA(uint8_t* registers, const uint8_t* memory,
               const uint16_t address)
{
    registers[REG_A] = memory[address];
    return 13;
}

inline uint8_t STA(const uint8_t* registers, uint8_t* memory,
               const uint16_t address)
{
    memory[address] = registers[REG_A];
    return 13;
}

inline uint8_t LHLD(uint8_t* registers, const uint8_t* memory,
                uint16_t const address)
{
    registers[REG_L] = memory[address];
    registers[REG_H] = memory[address + 1];
    return 16;
}

inline uint8_t SHLD(const uint8_t* registers, uint8_t* memory,
                const uint16_t address)
{
    memory[address] = registers[REG_L];
    memory[address + 1] = registers[REG_H];
    return 16;
}

inline uint8_t XCHG(uint8_t* registers)
{
    uint8_t aux;
    aux = registers[REG_H];
    registers[REG_H] = registers[REG_D];
    registers[REG_D] = aux;

    aux = registers[REG_L];
    registers[REG_L] = registers[REG_E];
    registers[REG_E] = aux;

    return 4;
}

inline uint8_t ADD_mem(uint8_t* registers, const uint8_t* memory, uint8_t* flags)
{
    uint8_t h = registers[REG_H];
    uint8_t l = registers[REG_L];

    uint8_t A = registers[REG_A];
    uint8_t B = memory[ADDRESS(h, l)];

    registers[REG_A] = A + B;

    (*flags) = update_flags(registers[REG_A], A, B, true, *flags, ALL_FLAGS);

    return 7;
}

inline uint8_t ADD_imm(uint8_t* registers, const uint8_t imm, uint8_t* flags)
{
    uint8_t A = registers[REG_A];
    uint8_t B = imm;

    registers[REG_A] = A + B;

    (*flags) = update_flags(registers[REG_A], A, B, true, *flags, ALL_FLAGS);

    return 7;
}

inline uint8_t ADC_mem(uint8_t* registers, const uint8_t* memory,
                   uint8_t* flags)
{
    uint8_t h = registers[REG_H];
    uint8_t l = registers[REG_L];
    uint8_t carry = HAS_FLAG_SET(*flags, CARRY_FLAG) ? 1 : 0;
    uint8_t address = ADDRESS(h, l);

    uint8_t A = registers[REG_A];
    uint8_t B = memory[address];

    registers[REG_A] = A + B + carry;

    // Manually set the C and AC flags since we have 3 terms in the addition
    (*flags) = update_flags(registers[REG_A], 0, 0, true, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG);

    (*flags) = ((A + B + carry) > 255) ? SET_FLAG(*flags, CARRY_FLAG)
                                    : CLEAR_FLAG(*flags, CARRY_FLAG);

    (*flags) = (((A & 0x0F) + (B & 0x0F) + 1) > 15)
                       ? SET_FLAG(*flags, AUXILIARY_CARRY_FLAG)
                       : CLEAR_FLAG(*flags, AUXILIARY_CARRY_FLAG);

    return 7;
}

inline uint8_t ACI(uint8_t* registers, uint8_t imm, uint8_t* flags)
{
    uint8_t carry = HAS_FLAG_SET(*flags, CARRY_FLAG) ? 1 : 0;
    uint8_t A = registers[REG_A];
    uint8_t B = imm;

    registers[REG_A] = A + B + carry;

    // Manually set the C and AC flags since we have 3 terms in the addition
    (*flags) = update_flags(registers[REG_A], 0, 0, true, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG);

    (*flags) = ((A + B + carry) > 255) ? SET_FLAG(*flags, CARRY_FLAG)
                                    : CLEAR_FLAG(*flags, CARRY_FLAG);

    (*flags) = (((A & 0x0F) + (B & 0x0F) + 1) > 15)
                       ? SET_FLAG(*flags, AUXILIARY_CARRY_FLAG)
                       : CLEAR_FLAG(*flags, AUXILIARY_CARRY_FLAG);

    return 7;
}

#endif
