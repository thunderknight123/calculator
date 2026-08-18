#include "calc.h"

#include <stdlib.h>
#include <string.h>

static void StartFreshOperand(CalcInput *input)
{
    input->text[0] = '\0';
    input->length = 0;
}

static bool AppendChar(CalcInput *input, char c)
{
    if (input->length >= CALC_OPERAND_MAX_CHARS) return false;
    input->text[input->length++] = c;
    input->text[input->length] = '\0';
    return true;
}

static void BeginSecondOperand(CalcInput *input)
{
    if (input->pendingOp != CALC_OP_NONE && !input->secondStarted)
    {
        StartFreshOperand(input);
        input->secondStarted = true;
    }
}

void calc_input_init(CalcInput *input)
{
    if (input == NULL) return;
    memset(input, 0, sizeof(*input));
    input->text[0] = '\0';
    input->pendingOp = CALC_OP_NONE;
}

bool calc_input_digit(CalcInput *input, char digit)
{
    if (input == NULL || digit < '0' || digit > '9') return false;
    BeginSecondOperand(input);
    return AppendChar(input, digit);
}

bool calc_input_decimal(CalcInput *input)
{
    if (input == NULL) return false;
    BeginSecondOperand(input);
    if (strchr(input->text, '.') != NULL) return false;
    if (input->length == 0)
    {
        if (!AppendChar(input, '0')) return false;
    }
    return AppendChar(input, '.');
}

void calc_input_toggle_sign(CalcInput *input)
{
    if (input == NULL) return;
    BeginSecondOperand(input);
    if (input->length > 0 && input->text[0] == '-')
    {
        memmove(input->text, input->text + 1, (size_t)input->length);
        input->length--;
        input->text[input->length] = '\0';
    }
    else if (input->length < CALC_OPERAND_MAX_CHARS)
    {
        memmove(input->text + 1, input->text, (size_t)input->length + 1);
        input->text[0] = '-';
        input->length++;
    }
}

void calc_input_backspace(CalcInput *input)
{
    if (input == NULL || input->length == 0) return;
    input->length--;
    input->text[input->length] = '\0';
}

void calc_input_clear(CalcInput *input)
{
    if (input == NULL) return;
    StartFreshOperand(input);
    input->pendingOp = CALC_OP_NONE;
    input->secondStarted = false;
}

void calc_input_set_operator(CalcInput *input, CalcOperator op)
{
    if (input == NULL) return;
    input->pendingOp = op;
    input->secondStarted = false;
}

const char *calc_input_text(const CalcInput *input)
{
    static const char zero[] = "0";
    if (input == NULL || input->length == 0) return zero;
    return input->text;
}

double calc_input_value(const CalcInput *input)
{
    if (input == NULL || input->length == 0) return 0.0;
    return atof(input->text);
}
