/*
 * 10 '?:' ternary (set to 0 in middle)
 *
 * Do not use any assignation yet
 */
#ifndef OP
# define OP(Name, NulPrio, LeftPrio, Type, Operator, /* Operator String */ ...)
#endif

// Simple operators
OP(  PLUS_PLUS, 130, 120, PREFIX_POSTFIX,          ++, '+', '+', 0)
OP(MINUS_MINUS, 130, 120, PREFIX_POSTFIX,          --, '-', '-', 0)
OP(        NOT, 130,  -1,         PREFIX,           !, '!', 0)
OP(    BIT_NOT, 130,  -1,         PREFIX,           ~, '~', 0)
OP(       PLUS, 110,  90,   PREFIX_INFIX,           +, '+', 0)
OP(      MINUS, 110,  90,   PREFIX_INFIX,           -, '-', 0)
OP(       POWER, -1, 120,          POWER, PLACEHOLDER, '*', '*', 0)
OP(       TIMES, -1, 100,          INFIX,           *, '*', 0)
OP(     DIVIDES, -1, 100,          INFIX,           /, '/', 0)
OP(      MODULO, -1, 100,          INFIX,           %, '%', 0)
OP(  LEFT_SHIFT, -1,  80,          INFIX,          <<, '<', '<', 0)
OP( RIGHT_SHIFT, -1,  80,          INFIX,          >>, '>', '>', 0)
OP(       EQUAL, -1,  70,          INFIX,          ==, '=', '=', 0)
OP(   NOT_EQUAL, -1,  70,          INFIX,          !=, '!', '=', 0)
OP(     BIT_AND, -1,  60,          INFIX,           &, '&', 0)
OP(     BIT_XOR, -1,  50,          INFIX,           ^, '^', 0)
OP(      BIT_OR, -1,  40,          INFIX,           |, '|', 0)
OP(         AND, -1,  30,          INFIX,          &&, '&', '&', 0)
OP(          OR, -1,  20,          INFIX,          ||, '|', '|', 0)
// Special operators
OP(     TERNARY, -1,  10,           TERN, PLACEHOLDER, '?', 0)
OP(       COLON, -1,  -1,         NOT_OP, PLACEHOLDER, ':', 0)
OP(     L_PAREN,  0,   0,          PAREN, PLACEHOLDER, '(', 0)
OP(     R_PAREN, -1,  -1,         NOT_OP, PLACEHOLDER, ')', 0)
// Special tokens
OP(      NUMBER,  0,  -1,         NOT_OP, PLACEHOLDER, 0)

#undef OP

#undef PREFIX_POSTFIX
#undef PREFIX
#undef PREFIX_INFIX
#undef INFIX
#undef POWER
#undef TERN
#undef PAREN
#undef NOT_OP
