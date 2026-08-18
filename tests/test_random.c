#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "calc.h"
#include "test_util.h"

#define RANDOM_RUNS 100

static void DriveCalculation(CalcContext *ctx, double a, CalcOperator op, double b)
{
    char buf[64];
    const char *p;

    calc_init(ctx);

    snprintf(buf, sizeof(buf), "%.2f", a);
    for (p = buf; *p != '\0'; p++)
    {
        if (*p == '.') calc_decimal(ctx);
        else if (*p == '-') calc_toggle_sign(ctx);
        else calc_digit(ctx, *p);
    }

    calc_select_operator(ctx, op);

    snprintf(buf, sizeof(buf), "%.2f", b);
    for (p = buf; *p != '\0'; p++)
    {
        if (*p == '.') calc_decimal(ctx);
        else if (*p == '-') calc_toggle_sign(ctx);
        else calc_digit(ctx, *p);
    }

    calc_equals(ctx);
}

static double ReferenceOp(CalcOperator op, double a, double b)
{
    switch (op)
    {
        case CALC_OP_ADD: return a + b;
        case CALC_OP_SUB: return a - b;
        case CALC_OP_MUL: return a * b;
        case CALC_OP_DIV: return a / b;
        default: return 0.0;
    }
}

int main(void)
{
    CalcContext ctx;
    unsigned int seed = 20260818u;
    int i;

    srand(seed);

    for (i = 0; i < RANDOM_RUNS; i++)
    {
        int aInt = rand() % 20001 - 10000;
        int bInt = rand() % 20001 - 10000;
        int opInt = rand() % 4;
        double a = aInt / 10.0;
        double b = bInt / 10.0;
        CalcOperator op = (CalcOperator)(opInt + 1);
        double expected;

        if (op == CALC_OP_DIV && b == 0.0)
        {
            b = 1.0;
        }

        DriveCalculation(&ctx, a, op, b);
        expected = ReferenceOp(op, a, b);

        if (calc_state(&ctx) != CALC_STATE_RESULT)
        {
            test_checks++;
            test_failures++;
            printf("FAIL: run %d - unexpected state %d for %g op%d %g\n",
                   i + 1, calc_state(&ctx), a, opInt + 1, b);
            continue;
        }

        {
            double actual = calc_result_value(&ctx);
            test_checks++;
            if (fabs(actual - expected) > 1e-6 * (fabs(expected) + 1.0))
            {
                test_failures++;
                printf("FAIL: run %d - %g op%d %g expected %g, got %g (display \"%s\")\n",
                       i + 1, a, opInt + 1, b, expected, actual, calc_display_text(&ctx));
            }
        }
    }

    TCHECK(calc_state(&ctx) == CALC_STATE_RESULT || calc_state(&ctx) == CALC_STATE_ERROR,
           "AC-19: process alive and in a valid state after 100 runs");

    printf("%d random runs, %d checks, %d failures\n", RANDOM_RUNS, test_checks, test_failures);
    return test_failures == 0 ? 0 : 1;
}
