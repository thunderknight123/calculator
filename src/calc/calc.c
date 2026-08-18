#include "calc.h"

#include <stdio.h>
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

static void SetDisplay(CalcContext *ctx, const char *text)
{
    snprintf(ctx->display, sizeof(ctx->display), "%s", text ? text : "");
}

static void SyncDisplayFromInput(CalcContext *ctx)
{
    SetDisplay(ctx, calc_input_text(&ctx->input));
}

static bool InErrorState(const CalcContext *ctx)
{
    return ctx->state == CALC_STATE_ERROR;
}

static void EnterErrorState(CalcContext *ctx)
{
    ctx->state = CALC_STATE_ERROR;
    SetDisplay(ctx, CALC_ERROR_TEXT);
}

static double ApplyOperator(CalcOperator op, double a, double b, CalcContext *ctx)
{
    switch (op)
    {
        case CALC_OP_ADD: return a + b;
        case CALC_OP_SUB: return a - b;
        case CALC_OP_MUL: return a * b;
        case CALC_OP_DIV:
            if (b == 0.0)
            {
                EnterErrorState(ctx);
                return 0.0;
            }
            return a / b;
        default: return b;
    }
}

static void FormatNumber(double value, char *out, size_t size)
{
    if (value == 0.0)
    {
        snprintf(out, size, "0");
        return;
    }
    snprintf(out, size, "%.10g", value);
}

static void StartNewCalculation(CalcContext *ctx)
{
    StartFreshOperand(&ctx->input);
    ctx->input.pendingOp = CALC_OP_NONE;
    ctx->input.secondStarted = false;
    ctx->currentOp = CALC_OP_NONE;
    ctx->operand1 = 0.0;
    ctx->result = 0.0;
    ctx->state = CALC_STATE_OPERAND1;
    SetDisplay(ctx, "0");
}

void calc_init(CalcContext *ctx)
{
    if (ctx == NULL) return;
    memset(ctx, 0, sizeof(*ctx));
    StartNewCalculation(ctx);
}

bool calc_digit(CalcContext *ctx, char digit)
{
    if (ctx == NULL || InErrorState(ctx)) return false;
    if (ctx->state == CALC_STATE_RESULT) StartNewCalculation(ctx);
    bool ok = calc_input_digit(&ctx->input, digit);
    if (ok)
    {
        if (ctx->state == CALC_STATE_OPERATOR && ctx->input.secondStarted)
        {
            ctx->state = CALC_STATE_OPERAND2;
        }
        SyncDisplayFromInput(ctx);
    }
    return ok;
}

bool calc_decimal(CalcContext *ctx)
{
    if (ctx == NULL || InErrorState(ctx)) return false;
    if (ctx->state == CALC_STATE_RESULT) StartNewCalculation(ctx);
    bool ok = calc_input_decimal(&ctx->input);
    if (ok)
    {
        if (ctx->state == CALC_STATE_OPERATOR && ctx->input.secondStarted)
        {
            ctx->state = CALC_STATE_OPERAND2;
        }
        SyncDisplayFromInput(ctx);
    }
    return ok;
}

void calc_toggle_sign(CalcContext *ctx)
{
    if (ctx == NULL || InErrorState(ctx)) return;
    if (ctx->state == CALC_STATE_RESULT) StartNewCalculation(ctx);
    calc_input_toggle_sign(&ctx->input);
    if (ctx->state == CALC_STATE_OPERATOR && ctx->input.secondStarted)
    {
        ctx->state = CALC_STATE_OPERAND2;
    }
    SyncDisplayFromInput(ctx);
}

void calc_backspace(CalcContext *ctx)
{
    if (ctx == NULL || InErrorState(ctx) || ctx->state == CALC_STATE_RESULT) return;
    calc_input_backspace(&ctx->input);
    if (ctx->state == CALC_STATE_OPERATOR)
    {
        ctx->operand1 = calc_input_value(&ctx->input);
    }
    SyncDisplayFromInput(ctx);
}

void calc_select_operator(CalcContext *ctx, CalcOperator op)
{
    if (ctx == NULL || InErrorState(ctx) || op == CALC_OP_NONE) return;

    if (ctx->state == CALC_STATE_RESULT)
    {
        ctx->operand1 = ctx->result;
        ctx->input.pendingOp = op;
        ctx->input.secondStarted = false;
        ctx->currentOp = op;
        ctx->state = CALC_STATE_OPERATOR;
        return;
    }

    if (ctx->state == CALC_STATE_OPERAND2)
    {
        StartFreshOperand(&ctx->input);
        ctx->input.secondStarted = false;
        ctx->currentOp = op;
        ctx->state = CALC_STATE_OPERATOR;
        FormatNumber(ctx->operand1, ctx->display, sizeof(ctx->display));
        return;
    }

    ctx->operand1 = calc_input_value(&ctx->input);
    ctx->currentOp = op;
    ctx->input.pendingOp = op;
    ctx->input.secondStarted = false;
    ctx->state = CALC_STATE_OPERATOR;
    SyncDisplayFromInput(ctx);
}

void calc_equals(CalcContext *ctx)
{
    if (ctx == NULL || InErrorState(ctx) || ctx->state == CALC_STATE_RESULT) return;

    if (ctx->state == CALC_STATE_OPERATOR)
    {
        ctx->result = ApplyOperator(ctx->currentOp, ctx->operand1, ctx->operand1, ctx);
    }
    else if (ctx->state == CALC_STATE_OPERAND2)
    {
        double operand2 = calc_input_value(&ctx->input);
        ctx->result = ApplyOperator(ctx->currentOp, ctx->operand1, operand2, ctx);
    }
    else
    {
        ctx->result = calc_input_value(&ctx->input);
    }

    if (ctx->state == CALC_STATE_ERROR) return;

    ctx->state = CALC_STATE_RESULT;
    FormatNumber(ctx->result, ctx->display, sizeof(ctx->display));
}

void calc_clear(CalcContext *ctx)
{
    if (ctx == NULL) return;
    StartNewCalculation(ctx);
}

const char *calc_display_text(const CalcContext *ctx)
{
    if (ctx == NULL) return "0";
    return ctx->display;
}

CalcState calc_state(const CalcContext *ctx)
{
    if (ctx == NULL) return CALC_STATE_OPERAND1;
    return ctx->state;
}

double calc_operand_value(const CalcContext *ctx)
{
    if (ctx == NULL) return 0.0;
    return calc_input_value(&ctx->input);
}

double calc_result_value(const CalcContext *ctx)
{
    if (ctx == NULL) return 0.0;
    return ctx->result;
}
