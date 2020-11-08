#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define UNREACHABLE() __builtin_unreachable()
#define ARR_SIZE(Arr) (sizeof(Arr) / sizeof(*Arr))
#define OP_STRING(...) (const char[]){__VA_ARGS__}
#define OP_SIZE(...) (sizeof(OP_STRING(__VA_ARGS__)) - 1)

typedef bool (nul_f)(const char **input, int *res, int until);
typedef bool (left_f)(int lhs, const char **input, int *res, int until);

// Define the different tokens
enum token_kind {
#define OP(Name, ...) Name,
#include "operators.inc"
};

struct token {
    enum token_kind kind;
    int val; // Used for NUMBER
};

// Forward declare functions
#define NulFunc(Name) \
    static nul_f eval_ ## Name ## _nul;
#define LeftFunc(Name) \
    static left_f eval_ ## Name ## _left;

#define PREFIX_POSTFIX(Name)  \
    NulFunc(Name) \
    LeftFunc(Name)
#define PREFIX(Name)  \
    NulFunc(Name)
#define PREFIX_INFIX(Name)  \
    NulFunc(Name) \
    LeftFunc(Name)
#define INFIX(Name)  \
    LeftFunc(Name)
#define POWER(Name)  \
    LeftFunc(Name)
#define TERN(Name)  \
    LeftFunc(Name)
#define PAREN(Name) \
    NulFunc(Name)
#define NOT_OP(Name)
# define OP(Name, NulPrio, LeftPrio, Type, ...) \
    Type(Name)
#include "operators.inc"

#undef NulFunc
#undef LeftFunc

// Symbol table
static const struct {
    const char *op;
    const size_t op_len;
    const int nul_prio;
    const int left_prio;
    nul_f *const nul_func;
    left_f *const left_func;
} ops[] = {
#define NulFunc(Name) \
    eval_ ## Name ## _nul
#define LeftFunc(Name) \
    eval_ ## Name ## _left

#define PREFIX_POSTFIX(Name, NulPrio, LeftPrio, ...)  \
    [Name] = {OP_STRING(__VA_ARGS__), OP_SIZE(__VA_ARGS__), NulPrio, LeftPrio, NulFunc(Name), LeftFunc(Name), },
#define PREFIX(Name, NulPrio, LeftPrio, ...)  \
    [Name] = {OP_STRING(__VA_ARGS__), OP_SIZE(__VA_ARGS__), NulPrio, LeftPrio, NulFunc(Name), NULL, },
#define PREFIX_INFIX(Name, NulPrio, LeftPrio, ...)  \
    [Name] = {OP_STRING(__VA_ARGS__), OP_SIZE(__VA_ARGS__), NulPrio, LeftPrio, NulFunc(Name), LeftFunc(Name), },
#define INFIX(Name, NulPrio, LeftPrio, ...)  \
    [Name] = {OP_STRING(__VA_ARGS__), OP_SIZE(__VA_ARGS__), NulPrio, LeftPrio, NULL, LeftFunc(Name), },
#define POWER(Name, NulPrio, LeftPrio, ...)  \
    [Name] = {OP_STRING(__VA_ARGS__), OP_SIZE(__VA_ARGS__), NulPrio, LeftPrio, NULL, LeftFunc(Name), },
#define TERN(Name, NulPrio, LeftPrio, ...)  \
    [Name] = {OP_STRING(__VA_ARGS__), OP_SIZE(__VA_ARGS__), NulPrio, LeftPrio, NULL, LeftFunc(Name), },
#define PAREN(Name, NulPrio, LeftPrio, ...) \
    [Name] = {OP_STRING(__VA_ARGS__), OP_SIZE(__VA_ARGS__), NulPrio, LeftPrio, NulFunc(Name), NULL, },
#define NOT_OP(Name, NulPrio, LeftPrio, ...) \
    [Name] = {OP_STRING(__VA_ARGS__), OP_SIZE(__VA_ARGS__), NulPrio, LeftPrio, NULL, NULL, },
# define OP(Name, NulPrio, LeftPrio, Type, Operator, /* Operator String */ ...) \
    Type(Name, NulPrio, LeftPrio, __VA_ARGS__)
#include "operators.inc"

#undef LeftFunc
#undef NulFunc
};

// Lexing functions
static void skip_whitespace(const char **input) {
    while (*input[0] && isspace(*input[0]))
        *input += 1; // Skip this character
}

static bool expect(enum token_kind expected, const char **input) {
    skip_whitespace(input);

    if (strncmp(*input, ops[expected].op, ops[expected].op_len) != 0)
        return false;

    *input += ops[expected].op_len;
    return true;
}

static size_t my_atoi(const char **input, int *val) {
    size_t len = 0;

    *val = 0; // Initialize its value
    while (isdigit((*input)[len]))
    {
        *val *= 10;
        *val += (*input)[len] - '0';
        len += 1;
    };

    return len;
}

static size_t lex_operator(const char **input, enum token_kind *op) {
    size_t best_len = 0;

    for (size_t i = 0; i < ARR_SIZE(ops); ++i)
    {
        if (ops[i].op_len <= best_len) // Only look at longer operators
            continue;
        if (strncmp(*input, ops[i].op, ops[i].op_len) == 0)
        {
            best_len = ops[i].op_len;
            *op = i;
        }
    }

    return best_len;
}

static size_t lex_token(const char **input, struct token *tok) {
    skip_whitespace(input);

    size_t len;

    if ((len = lex_operator(input, &tok->kind)))
        return len;

    // Assume that it is a number
    tok->kind = NUMBER;
    return my_atoi(input, &tok->val);
}

static bool parse_until_left(int lhs, const char **input, int *res, int prio) {
    struct token tok;
    size_t len = 0;

    while ((len = lex_token(input, &tok)) &&
            prio < ops[tok.kind].left_prio) {
        *input += len;
        left_f *left_func = ops[tok.kind].left_func;
        if (!left_func)
            return false; // Error: not a prefix operator
        if (!left_func(lhs, input, res, prio))
            return false; // Error: could not parse right-hand-side
        lhs = *res;
    }

    return true;
}
static bool parse_until(const char **input, int *res, int prio) {
    struct token tok;
    size_t len = 0;

    // Parse left token
    if (!(len = lex_token(input, &tok)))
        return false;

    // Assume prefix
    *input += len;
    if (tok.kind == NUMBER) {
        *res = tok.val;
    } else {
        nul_f *nul_func = ops[tok.kind].nul_func;
        if (!nul_func)
            return false; // Error: not a prefix operator
        if (!nul_func(input, res, prio))
            return false; // Error: could not parse right-hand-side
    }

    // Do left-loop
    return parse_until_left(*res, input, res, prio);
}

bool eval_string(const char *input, int *res) {
    if (!parse_until(&input, res, 0))
        return false;
    // Verify that only the expression exists
    skip_whitespace(&input);
    return *input == '\0';
}

#define NulFunc(Name) \
    static bool eval_ ## Name ## _nul( \
            const char **input, \
            int *res, \
            __attribute__((unused)) int until)
#define LeftFunc(Name) \
    static bool eval_ ## Name ## _left( \
            int lhs, \
            const char **input, \
            int *res, \
            __attribute__((unused)) int until)

#define PREFIX_POSTFIX(Name, NulPrio, LeftPrio, Operator)  \
    LeftFunc(Name) { \
        return parse_until_left(lhs Operator, input, res, LeftPrio); \
    } \
    NulFunc(Name) { \
        if (!parse_until(input, res, NulPrio)) \
            return false; \
        Operator *res; \
        return true; \
    }
#define PREFIX(Name, NulPrio, LeftPrio, Operator)  \
    NulFunc(Name) { \
        if (!parse_until(input, res, NulPrio)) \
            return false; \
        *res = Operator *res; \
        return true; \
    }
#define PREFIX_INFIX(Name, NulPrio, LeftPrio, Operator)  \
    LeftFunc(Name) { \
        if (!parse_until(input, res, LeftPrio)) \
            return false; \
        *res = lhs Operator *res; \
        return true; \
    } \
    NulFunc(Name) { \
        if (!parse_until(input, res, NulPrio)) \
            return false; \
        *res = Operator *res; \
        return true; \
    }
#define INFIX(Name, NulPrio, LeftPrio, Operator) \
    LeftFunc(Name) { \
        if (!parse_until(input, res, LeftPrio)) \
            return false; \
        *res = lhs Operator *res; \
        return true; \
    }
static int my_pow(int lhs, int rhs) {
    if (!rhs)
        return 1;
    int rec = my_pow(lhs * lhs, rhs / 2);
    if (rhs & 1)
        rec *= lhs;
    return rec;
}
#define POWER(Name, NulPrio, LeftPrio, Operator) \
    LeftFunc(Name) { \
        if (!parse_until(input, res, LeftPrio - 1)) \
            return false; \
        *res = my_pow(lhs, *res); \
        return true; \
    }
#define TERN(Name, NulPrio, LeftPrio, Operator) \
    LeftFunc(Name) { \
        int true_val; \
        if (!parse_until(input, &true_val, 0) || !expect(COLON, input)) \
            return false; \
        int false_val; \
        if (!parse_until(input, &false_val, until)) \
            return false; \
        *res = (lhs ? true_val : false_val); \
        return true; \
    }
#define PAREN(Name, NulPrio, LeftPrio, Operator) \
    NulFunc(Name) { \
        if (!parse_until(input, res, 0)) \
            return false; \
        return expect(R_PAREN, input); \
    }
#define NOT_OP(Name, NulPrio, LeftPrio, Operator) /* Nothing */
# define OP(Name, NulPrio, LeftPrio, Type, Operator, ...) \
    Type(Name, NulPrio, LeftPrio, Operator)
#include "operators.inc"

#undef LeftFunc
#undef NulFunc
