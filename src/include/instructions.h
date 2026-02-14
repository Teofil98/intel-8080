#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "definitions.h"
#include <stdint.h>
#include <stdbool.h>

inline uint8_t get_mem_HL(const uint8_t* registers, const uint8_t* memory)
{
    const uint8_t h = registers[REG_H];
    const uint8_t l = registers[REG_L];
    const uint8_t address = ADDRESS(h, l);
    return memory[address];
}

inline uint8_t update_flags(const uint8_t result, const uint8_t A,
                            const uint8_t B, const bool addition,
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
        flags = (((uint16_t)A + B) > 255) ? SET_FLAG(flags, CARRY_FLAG)
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
    // TODO: Replace explicit calls with the get_mem_HL function (multiple
    // places in code)
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

inline uint8_t ADI(uint8_t* registers, const uint8_t imm, uint8_t* flags)
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
    uint8_t carry = GET_FLAG(*flags, CARRY_FLAG);
    uint8_t address = ADDRESS(h, l);

    uint8_t A = registers[REG_A];
    uint8_t B = memory[address];

    registers[REG_A] = A + B + carry;

    // Manually set the C and AC flags since we have 3 terms in the addition
    (*flags) = update_flags(registers[REG_A], 0, 0, true, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG);

    (*flags) = (((uint16_t)A + B + carry) > 255)
                   ? SET_FLAG(*flags, CARRY_FLAG)
                   : CLEAR_FLAG(*flags, CARRY_FLAG);

    (*flags) = (((A & 0x0F) + (B & 0x0F) + carry) > 15)
                       ? SET_FLAG(*flags, AUXILIARY_CARRY_FLAG)
                       : CLEAR_FLAG(*flags, AUXILIARY_CARRY_FLAG);

    return 7;
}

inline uint8_t ACI(uint8_t* registers, uint8_t imm, uint8_t* flags)
{
    uint8_t carry = GET_FLAG(*flags, CARRY_FLAG);
    uint8_t A = registers[REG_A];
    uint8_t B = imm;

    registers[REG_A] = A + B + carry;

    // Manually set the C and AC flags since we have 3 terms in the addition
    (*flags) = update_flags(registers[REG_A], 0, 0, true, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG);

    (*flags) = (((uint16_t)A + B + carry) > 255)
                   ? SET_FLAG(*flags, CARRY_FLAG)
                   : CLEAR_FLAG(*flags, CARRY_FLAG);

    (*flags) = (((A & 0x0F) + (B & 0x0F) + carry) > 15)
                       ? SET_FLAG(*flags, AUXILIARY_CARRY_FLAG)
                       : CLEAR_FLAG(*flags, AUXILIARY_CARRY_FLAG);

    return 7;
}

inline uint8_t SUB_mem(uint8_t* registers, const uint8_t* memory, uint8_t* flags)
{
    uint8_t h = registers[REG_H];
    uint8_t l = registers[REG_L];

    uint8_t A = registers[REG_A];
    uint8_t B = memory[ADDRESS(h, l)];

    registers[REG_A] = A - B;

    (*flags) = update_flags(registers[REG_A], A, B, false, *flags, ALL_FLAGS);

    return 7;
}

inline uint8_t SUI(uint8_t* registers, const uint8_t imm, uint8_t* flags)
{
    uint8_t A = registers[REG_A];
    uint8_t B = imm;

    registers[REG_A] = A - B;

    (*flags) = update_flags(registers[REG_A], A, B, false, *flags, ALL_FLAGS);

    return 7;
}

inline uint8_t SBB_mem(uint8_t* registers, const uint8_t* memory,
                   uint8_t* flags)
{
    uint8_t h = registers[REG_H];
    uint8_t l = registers[REG_L];
    uint8_t carry = GET_FLAG(*flags, CARRY_FLAG);
    uint8_t address = ADDRESS(h, l);

    uint8_t A = registers[REG_A];
    uint8_t B = memory[address];

    registers[REG_A] = A - B - carry;

    // Manually set the C and AC flags since we have 3 terms in the addition
    (*flags) = update_flags(registers[REG_A], 0, 0, false, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG);

    (*flags) = (A < ((uint16_t)B + carry)) ? SET_FLAG(*flags, CARRY_FLAG)
                                    : CLEAR_FLAG(*flags, CARRY_FLAG);

    (*flags) = ( (A & 0x0F) < ((B & 0x0F) + carry) )
                       ? SET_FLAG(*flags, AUXILIARY_CARRY_FLAG)
                       : CLEAR_FLAG(*flags, AUXILIARY_CARRY_FLAG);

    return 7;
}

inline uint8_t SBI(uint8_t* registers, const uint8_t imm, uint8_t* flags)
{
    uint8_t carry = GET_FLAG(*flags, CARRY_FLAG);
    uint8_t A = registers[REG_A];
    uint8_t B = imm;

    registers[REG_A] = A - B - carry;

    // Manually set the C and AC flags since we have 3 terms in the addition
    (*flags) = update_flags(registers[REG_A], 0, 0, true, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG);

    (*flags) = (A < (B + carry)) ? SET_FLAG(*flags, CARRY_FLAG)
                                    : CLEAR_FLAG(*flags, CARRY_FLAG);

    (*flags) = ((A & 0x0F) < ((B & 0x0F) + carry))
                       ? SET_FLAG(*flags, AUXILIARY_CARRY_FLAG)
                       : CLEAR_FLAG(*flags, AUXILIARY_CARRY_FLAG);

    return 7;
}

inline uint8_t INR_mem(const uint8_t* registers, uint8_t* memory,
                       uint8_t* flags)
{
    uint8_t h = registers[REG_H];
    uint8_t l = registers[REG_L];
    uint8_t address = ADDRESS(h, l);

    const uint8_t A = memory[address];
    const uint8_t B = 1;
    memory[address] = A + B;

    (*flags) = update_flags(memory[address], A, B, true, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG
                                | AUXILIARY_CARRY_FLAG);

    return 10;
}

inline uint8_t DCR_mem(const uint8_t* registers, uint8_t* memory,
                       uint8_t* flags)
{
    uint8_t h = registers[REG_H];
    uint8_t l = registers[REG_L];
    uint8_t address = ADDRESS(h, l);

    const uint8_t A = memory[address];
    const uint8_t B = 1;
    memory[address] = A - B;

    (*flags) = update_flags(memory[address], A, B, false, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG
                                | AUXILIARY_CARRY_FLAG);

    return 10;
}

inline uint8_t DDA(uint8_t* registers, uint8_t* flags)
{
    // TODO: Not sure how the flags should be set here, double check/test!

    // 1. If the value of the least significant 4 bits of the accumulator is
    // greater than 9 or if the AC flag is set, 6 is added to the accumulator.
    if((registers[REG_A] & 0x0F) > 9
       || HAS_FLAG_SET(*flags, AUXILIARY_CARRY_FLAG)) {
        uint8_t A = registers[REG_A];
        uint8_t B = 6;
        registers[REG_A] = A + B;
        // NOTE: Probably from the Z80 manual
        // If a carry out of the least significant four bits occurs during Step
        // (1), the Auxiliary Carry bit is set; otherwise it is reset.
        (*flags) = update_flags(registers[REG_A], A, B, true, *flags,
                                AUXILIARY_CARRY_FLAG);
    }

    // 2. If the value of the most significant 4 bits of the accumulator is now
    // greater than 9, or if the CY flag is set, 6 is added to the most
    // significant 4 bits of the accumulator.
    if(((registers[REG_A] & 0xF0) > (0x09 << 4))
       || HAS_FLAG_SET(*flags, CARRY_FLAG)) {
        uint8_t A = registers[REG_A];
        uint8_t B = (0x06 << 4);
        registers[REG_A] = A + B;
        // NOTE: Probably from the Z80 manual
        // If a carry out of the most significant four bits occurs during Step
        // (2), the normal Carry bit is set; otherwise, it is unaffected
        if((uint8_t)A + B > 255) {
            (*flags) = SET_FLAG(*flags, CARRY_FLAG);
        }

        // Set the other flags
        (*flags) = update_flags(registers[REG_A], 0, 0, true, *flags,
                                ZERO_FLAG | PARITY_FLAG | SIGN_FLAG);
    }

    return 4;
}

inline uint8_t ANA_mem(uint8_t* registers, uint8_t* memory, uint8_t* flags)
{
    const uint8_t A = registers[REG_A];
    const uint8_t B = get_mem_HL(registers, memory);
    registers[REG_A] = A & B;

    (*flags) = update_flags(registers[REG_A], 0, 0, false, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG);

    // NOTE: From 8080/8085 manual: The CY flag is cleared and AC is set to the
    // OR’ing of bits 3 of the operands (8080).
    (*flags) = CLEAR_FLAG(*flags, CARRY_FLAG);
    uint8_t ac_flag = ((A & 0x08) | (B & 0x08));
    if(ac_flag > 0) {
        (*flags) = SET_FLAG(*flags, AUXILIARY_CARRY_FLAG);
    } else {
        (*flags) = CLEAR_FLAG(*flags, AUXILIARY_CARRY_FLAG);
    }

    return 7;
}

inline uint8_t ANI(uint8_t* registers, const uint8_t imm, uint8_t* flags)
{
    registers[REG_A] = registers[REG_A] & imm;

    (*flags) = update_flags(registers[REG_A], 0, 0, false, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG);
    (*flags) = CLEAR_FLAG(*flags, CARRY_FLAG);
    (*flags) = CLEAR_FLAG(*flags, AUXILIARY_CARRY_FLAG);

    return 7;
}

inline uint8_t XRA_mem(uint8_t* registers, uint8_t* memory, uint8_t* flags)
{
    const uint8_t A = registers[REG_A];
    const uint8_t B = get_mem_HL(registers, memory);
    registers[REG_A] = A ^ B;

    (*flags) = update_flags(registers[REG_A], 0, 0, false, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG);

    (*flags) = CLEAR_FLAG(*flags, CARRY_FLAG);
    (*flags) = CLEAR_FLAG(*flags, AUXILIARY_CARRY_FLAG);

    return 7;
}

inline uint8_t XRI(uint8_t* registers, const uint8_t imm, uint8_t* flags)
{
    registers[REG_A] = registers[REG_A] ^ imm;

    (*flags) = update_flags(registers[REG_A], 0, 0, false, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG);
    (*flags) = CLEAR_FLAG(*flags, CARRY_FLAG);
    (*flags) = CLEAR_FLAG(*flags, AUXILIARY_CARRY_FLAG);

    return 7;
}

inline uint8_t ORA_mem(uint8_t* registers, uint8_t* memory, uint8_t* flags)
{
    const uint8_t A = registers[REG_A];
    const uint8_t B = get_mem_HL(registers, memory);
    registers[REG_A] = A | B;

    (*flags) = update_flags(registers[REG_A], 0, 0, false, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG);

    (*flags) = CLEAR_FLAG(*flags, CARRY_FLAG);
    (*flags) = CLEAR_FLAG(*flags, AUXILIARY_CARRY_FLAG);

    return 7;
}

inline uint8_t ORI(uint8_t* registers, const uint8_t imm, uint8_t* flags)
{
    registers[REG_A] = registers[REG_A] | imm;

    (*flags) = update_flags(registers[REG_A], 0, 0, false, *flags,
                            ZERO_FLAG | SIGN_FLAG | PARITY_FLAG);
    (*flags) = CLEAR_FLAG(*flags, CARRY_FLAG);
    (*flags) = CLEAR_FLAG(*flags, AUXILIARY_CARRY_FLAG);

    return 7;
}

inline uint8_t CMP_mem(const uint8_t* registers, const uint8_t* memory,
                       uint8_t* flags)
{
    const uint8_t A = registers[REG_A];
    const uint8_t B = get_mem_HL(registers, memory);
    const uint8_t res = A - B;
    (*flags) = update_flags(res, A, B, FLAG_OPERATION_SUBTRACTION, *flags,
                            ALL_FLAGS);

    return 7;
}

inline uint8_t CPI(const uint8_t* registers, const uint8_t imm, uint8_t* flags)
{
    const uint8_t A = registers[REG_A];
    const uint8_t B = imm;
    const uint8_t res = A - B;
    (*flags) = update_flags(res, A, B, FLAG_OPERATION_SUBTRACTION, *flags,
                            ALL_FLAGS);

    return 7;
}

inline uint8_t RLC(uint8_t* registers, uint8_t* flags)
{
    uint8_t msb = ((registers[REG_A] & 0x80) >> 7);
    registers[REG_A] <<= 1;
    if(msb == 0) {
        // TODO: Maybe this should also set the flag, not just return the new
        // value
        (*flags) = CLEAR_FLAG(*flags, CARRY_FLAG);
    } else {
        registers[REG_A] |= 0x01;
        (*flags) = SET_FLAG(*flags, CARRY_FLAG);
    }

    return 4;
}

inline uint8_t RRC(uint8_t* registers, uint8_t* flags)
{
    uint8_t lsb = (registers[REG_A] & 0x01);
    registers[REG_A] >>= 1;
    if(lsb == 0) {
        (*flags) = CLEAR_FLAG(*flags, CARRY_FLAG);
    } else {
        registers[REG_A] |= 0x80;
        (*flags) = SET_FLAG(*flags, CARRY_FLAG);
    }

    return 4;
}

inline uint8_t RAL(uint8_t* registers, uint8_t* flags)
{
    uint8_t msb = ((registers[REG_A] & 0x80) >> 7);
    registers[REG_A] <<= 1;


    if(HAS_FLAG_SET(*flags, CARRY_FLAG)) {
        registers[REG_A] |= 0x01;
    }

    if(msb == 0) {
        // TODO: Maybe this should also set the flag, not just return the new
        // value
        (*flags) = CLEAR_FLAG(*flags, CARRY_FLAG);
    } else {
        (*flags) = SET_FLAG(*flags, CARRY_FLAG);
    }

    return 4;
}


inline uint8_t RAR(uint8_t* registers, uint8_t* flags)
{
    uint8_t lsb = (registers[REG_A] & 0x01);
    registers[REG_A] >>= 1;

    if(HAS_FLAG_SET(*flags, CARRY_FLAG)) {
        registers[REG_A] |= 0x80;
    }

    if(lsb == 0) {
        (*flags) = CLEAR_FLAG(*flags, CARRY_FLAG);
    } else {
        (*flags) = SET_FLAG(*flags, CARRY_FLAG);
    }

    return 4;
}

inline uint8_t CMA(uint8_t* registers)
{
    registers[REG_A] = (~registers[REG_A]);
    return 4;
}


inline uint8_t CMC(uint8_t* flags)
{
    (*flags) ^= CARRY_FLAG;
    return 4;
}

inline uint8_t STC(uint8_t* flags)
{
    (*flags) = SET_FLAG(*flags, CARRY_FLAG);
    return 4;
}

inline uint8_t JMP(const uint8_t addr_low, const uint8_t addr_high, uint16_t* PC)
{
    (*PC) = ADDRESS(addr_high, addr_low);
    return 10;
}

#endif // INSTRUCTIONS_H
