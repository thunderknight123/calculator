#ifndef CALCULATOR_CALC_H
#define CALCULATOR_CALC_H

#include <stdbool.h>

#define CALC_OPERAND_MAX_CHARS 20

typedef enum {
    CALC_OP_NONE = 0,
    CALC_OP_ADD,
    CALC_OP_SUB,
    CALC_OP_MUL,
    CALC_OP_DIV
} CalcOperator;

typedef struct {
    char text[CALC_OPERAND_MAX_CHARS + 1];
    int length;
    CalcOperator pendingOp;
    bool secondStarted;
} CalcInput;

void calc_input_init(CalcInput *input);
bool calc_input_digit(CalcInput *input, char digit);
bool calc_input_decimal(CalcInput *input);
void calc_input_toggle_sign(CalcInput *input);
void calc_input_backspace(CalcInput *input);
void calc_input_clear(CalcInput *input);
void calc_input_set_operator(CalcInput *input, CalcOperator op);
const char *calc_input_text(const CalcInput *input);
double calc_input_value(const CalcInput *input);

#endif
