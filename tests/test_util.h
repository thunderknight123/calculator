#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <math.h>
#include <stdio.h>
#include <string.h>

static int test_checks = 0;
static int test_failures = 0;

#define TCHECK(cond, msg) \
    do { \
        test_checks++; \
        if (!(cond)) { \
            test_failures++; \
            printf("FAIL: %s (line %d)\n", msg, __LINE__); \
        } \
    } while (0)

static void enter_text(CalcContext *ctx, const char *text)
{
    const char *p;
    for (p = text; *p != '\0'; p++)
    {
        if (*p == '.') calc_decimal(ctx);
        else if (*p == '-') calc_toggle_sign(ctx);
        else calc_digit(ctx, *p);
    }
}

static void expect_display(CalcContext *ctx, const char *expected, const char *msg)
{
    const char *actual = calc_display_text(ctx);
    test_checks++;
    if (strcmp(actual, expected) != 0)
    {
        test_failures++;
        printf("FAIL: %s - expected \"%s\", got \"%s\" (state=%d)\n", msg, expected, actual, calc_state(ctx));
    }
}

static void expect_result(CalcContext *ctx, double expected, const char *msg)
{
    double actual = calc_result_value(ctx);
    test_checks++;
    if (fabs(actual - expected) > 1e-9)
    {
        test_failures++;
        printf("FAIL: %s - expected %g, got %g\n", msg, expected, actual);
    }
}

#endif
