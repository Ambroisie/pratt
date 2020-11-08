#include <criterion/criterion.h>

#include "eval.h"

TestSuite(eval_string);

static void do_success(const char *input, int exptected) {
    int val;

    cr_assert(eval_string(input, &val));
    cr_assert_eq(val, exptected);
}

static void do_failure(const char *input) {
    int val;

    cr_assert_not(eval_string(input, &val));
}

#define SUCCESS(Name, Input) \
    Test(eval_string, Name) { do_success(#Input, (Input)); }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses"
SUCCESS(one, 1)
SUCCESS(the_answer, 42)
SUCCESS(int_max, 2147483647)
SUCCESS(addition, 1 + 2)
SUCCESS(additions, 1 + 2 + 3 + 4)
SUCCESS(substraction, 1 - 2)
SUCCESS(substractions, 1 - 1 - 1)
SUCCESS(multiplication, 2 * 3)
SUCCESS(multiplications, 2 * 3 * 4)
SUCCESS(division, 6 / 3)
SUCCESS(divisions, 6 / 3 / 2)
SUCCESS(modulo, 6 % 5)
SUCCESS(modulos, 127 % 19 % 3)
SUCCESS(left_shift, 1 << 3)
SUCCESS(left_shifts, 1 << 2 << 3)
SUCCESS(left_shift_not_one, 42 << 3)
SUCCESS(right_shift, 1024 >> 3)
SUCCESS(right_shifts, 1 >> 2 >> 3)
SUCCESS(right_shift_not_one, 1337 >> 3)
SUCCESS(eq, 1 == 1)
SUCCESS(neq, 1 != 1)
SUCCESS(not_true, !1)
SUCCESS(not_false, !0)
SUCCESS(bit_not, ~0)
SUCCESS(bit_not_the_answer, ~42)
SUCCESS(bit_and, 0 & 1)
SUCCESS(bit_and_non_zero, 255 & 127)
SUCCESS(bit_xor, 0 ^ 1)
SUCCESS(bit_xor_non_zero, 255 ^ 127)
SUCCESS(and, 1 && 0)
SUCCESS(and_non_false, 1 && 2)
SUCCESS(or, 0 || 1)
SUCCESS(or_false, 0 || 0)
SUCCESS(ternary_true, 1 ? 12 : 27)
SUCCESS(ternary_false, 0 ? 12 : 27)
SUCCESS(arith_priorities, 1 + 2 * 3 % 4 * 52 / 3)
SUCCESS(parenthesis, (1 + 2) * 3)
SUCCESS(shift_priorities, 1 + 2 << 5 - 3 >> 1)
SUCCESS(bitwise_priorities, 3241235 ^ 1234 & 4321 | 94)
SUCCESS(boolean_priorities, 1 == 3 & 1 | 2048)
SUCCESS(ternary_priorities, 1 == 2 ? 1 << 3 - 1 >> 1 : 42 << 4)
SUCCESS(ternary_associativity, 1 == 1 ? 1 : 0 == 0 ? 2 : 3)
// Other operators
Test(eval_string, power) { do_success("4 ** 3 ** 2", 262144); }
Test(eval_string, power_explicit) { do_success("4 ** (3 ** 2)", 262144); }
#pragma GCC diagnostic pop

#define FAILURE(Name, Input) \
    Test(eval_string, Name) { do_failure(Input); }

FAILURE(empty, "")
FAILURE(right_parenthesis, ")")
FAILURE(empty_parenthesis, "()")
FAILURE(trailing_operator, "1 +")
FAILURE(trailing_expression, "1 1")
FAILURE(double_operator, "1 * * 1")
FAILURE(prefix_as_infix, "1 ! 1")
FAILURE(infix_as_prefix, "*1")
