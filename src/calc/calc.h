#ifndef CALCULATOR_CALC_H
#define CALCULATOR_CALC_H

#include <stdbool.h>

#define CALC_OPERAND_MAX_CHARS 20
#define CALC_DISPLAY_MAX_CHARS 32
#define CALC_ERROR_TEXT "Error: Division by zero"

typedef enum {
    CALC_OP_NONE = 0,
    CALC_OP_ADD,
    CALC_OP_SUB,
    CALC_OP_MUL,
    CALC_OP_DIV
} CalcOperator;

typedef enum {
    CALC_STATE_OPERAND1 = 0,
    CALC_STATE_OPERATOR,
    CALC_STATE_OPERAND2,
    CALC_STATE_RESULT,
    CALC_STATE_ERROR
} CalcState;

typedef struct {
    char text[CALC_OPERAND_MAX_CHARS + 1];
    int length;
    CalcOperator pendingOp;
    bool secondStarted;
} CalcInput;

typedef struct {
    CalcInput input;
    CalcState state;
    CalcOperator currentOp;
    double operand1;
    double result;
    char display[CALC_DISPLAY_MAX_CHARS];
    char expression[CALC_DISPLAY_MAX_CHARS];
    char operand1Text[CALC_OPERAND_MAX_CHARS + 1];
} CalcContext;

void calc_input_init(CalcInput *input);
bool calc_input_digit(CalcInput *input, char digit);
bool calc_input_decimal(CalcInput *input);
void calc_input_toggle_sign(CalcInput *input);
void calc_input_backspace(CalcInput *input);
void calc_input_clear(CalcInput *input);
void calc_input_set_operator(CalcInput *input, CalcOperator op);
const char *calc_input_text(const CalcInput *input);
double calc_input_value(const CalcInput *input);

void calc_init(CalcContext *ctx);
bool calc_digit(CalcContext *ctx, char digit);
bool calc_decimal(CalcContext *ctx);
void calc_toggle_sign(CalcContext *ctx);
void calc_backspace(CalcContext *ctx);
void calc_select_operator(CalcContext *ctx, CalcOperator op);
void calc_equals(CalcContext *ctx);
void calc_clear(CalcContext *ctx);
const char *calc_display_text(const CalcContext *ctx);
const char *calc_expression_text(const CalcContext *ctx);
CalcState calc_state(const CalcContext *ctx);
double calc_operand_value(const CalcContext *ctx);
double calc_result_value(const CalcContext *ctx);

#endif
