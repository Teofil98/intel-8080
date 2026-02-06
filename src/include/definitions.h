#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define HAS_FLAG_SET(a, b) ( ((a) & (b) ) > 0 )
#define GET_FLAG(a, b) (HAS_FLAG_SET((a), (b)) ? 1 : 0)
#define SET_FLAG(a, b) ( (b) | (a))
#define CLEAR_FLAG(a, b)  ( (~(b)) & (a) )
#define CARRY_FLAG (1 << 0)
#define PARITY_FLAG (1 << 2)
#define AUXILIARY_CARRY_FLAG (1 << 4)
#define ZERO_FLAG (1 << 6)
#define SIGN_FLAG (1 << 7)
#define ALL_FLAGS (CARRY_FLAG | PARITY_FLAG | AUXILIARY_CARRY_FLAG | ZERO_FLAG | SIGN_FLAG)

#define REG_A 0b111
#define REG_B 0b000
#define REG_C 0b001
#define REG_D 0b010
#define REG_E 0b011
#define REG_H 0b100
#define REG_L 0b101

#define ADDRESS(h, l) (((h) << 8) | (l))

#endif
