#include <math.h>
#include <stdio.h>
#include <string.h>

#include "calc.h"

static int checks = 0;
static int failures = 0;

static void expect_text(CalcInput *input, const char *expected, const char *msg)
{
    const char *actual = calc_input_text(input);
    checks++;
    if (strcmp(actual, expected) != 0)
    {
        failures++;
        printf("FAIL: %s - expected \"%s\", got \"%s\"\n", msg, expected, actual);
    }
}

static void expect_double(CalcInput *input, double expected, const char *msg)
{
    double actual = calc_input_value(input);
    checks++;
    if (fabs(actual - expected) > 1e-9)
    {
        failures++;
        printf("FAIL: %s - expected %g, got %g\n", msg, expected, actual);
    }
}

static void expect_true(int condition, const char *msg)
{
    checks++;
    if (!condition)
    {
        failures++;
        printf("FAIL: %s\n", msg);
    }
}

static void expect_false(int condition, const char *msg)
{
    checks++;
    if (condition)
    {
        failures++;
        printf("FAIL: %s\n", msg);
    }
}

static void TestDigitAppend(void)
{
    CalcInput in;
    calc_input_init(&in);
    calc_input_digit(&in, '1');
    calc_input_digit(&in, '2');
    calc_input_digit(&in, '3');
    expect_text(&in, "123", "digit append");
    expect_double(&in, 123.0, "digit value");
}

static void TestBackspace(void)
{
    CalcInput in;
    calc_input_init(&in);
    calc_input_digit(&in, '1');
    calc_input_digit(&in, '2');
    calc_input_digit(&in, '3');
    calc_input_backspace(&in);
    expect_text(&in, "12", "backspace removes last char (AC-10)");
    calc_input_backspace(&in);
    calc_input_backspace(&in);
    expect_text(&in, "0", "backspace to empty shows 0");
    calc_input_backspace(&in);
    expect_text(&in, "0", "backspace on empty is safe");
}

static void TestDecimalSingle(void)
{
    CalcInput in;
    calc_input_init(&in);
    calc_input_digit(&in, '1');
    calc_input_decimal(&in);
    calc_input_digit(&in, '2');
    expect_false(calc_input_decimal(&in), "second decimal rejected (FR-07)");
    calc_input_digit(&in, '3');
    expect_text(&in, "1.23", "decimal limit result (AC-13)");
    expect_double(&in, 1.23, "decimal value");
}

static void TestDecimalOnEmpty(void)
{
    CalcInput in;
    calc_input_init(&in);
    calc_input_decimal(&in);
    expect_text(&in, "0.", "decimal on empty becomes 0.");
}

static void TestToggleSign(void)
{
    CalcInput in;
    calc_input_init(&in);
    calc_input_digit(&in, '5');
    calc_input_toggle_sign(&in);
    expect_text(&in, "-5", "toggle sign negative (AC-06 input)");
    expect_double(&in, -5.0, "negative value");
    calc_input_toggle_sign(&in);
    expect_text(&in, "5", "toggle sign back to positive");
}

static void TestToggleSignOnEmpty(void)
{
    CalcInput in;
    calc_input_init(&in);
    calc_input_toggle_sign(&in);
    calc_input_digit(&in, '3');
    expect_text(&in, "-3", "sign then digit");
}

static void TestLengthLimit(void)
{
    CalcInput in;
    calc_input_init(&in);
    int i;
    for (i = 0; i < 20; i++)
    {
        expect_true(calc_input_digit(&in, '7'), "digit within limit accepted");
    }
    expect_text(&in, "77777777777777777777", "20 chars max");
    expect_false(calc_input_digit(&in, '7'), "21st digit rejected (FR-06/AC-14)");
}

static void TestReInputOverwrite(void)
{
    CalcInput in;
    calc_input_init(&in);
    calc_input_digit(&in, '5');
    calc_input_set_operator(&in, CALC_OP_ADD);
    expect_text(&in, "5", "first operand shown after operator");
    calc_input_digit(&in, '3');
    expect_text(&in, "3", "re-input overwrites first operand (FR-09)");
    expect_double(&in, 3.0, "second operand value");
}

static void TestReInputOverwriteDecimalAndSign(void)
{
    CalcInput in;
    calc_input_init(&in);
    calc_input_digit(&in, '5');
    calc_input_set_operator(&in, CALC_OP_MUL);
    calc_input_decimal(&in);
    calc_input_digit(&in, '5');
    expect_text(&in, "0.5", "decimal starts fresh second operand");

    calc_input_clear(&in);
    calc_input_digit(&in, '9');
    calc_input_set_operator(&in, CALC_OP_SUB);
    calc_input_toggle_sign(&in);
    calc_input_digit(&in, '4');
    expect_text(&in, "-4", "sign starts fresh second operand");
}

static void TestClear(void)
{
    CalcInput in;
    calc_input_init(&in);
    calc_input_digit(&in, '8');
    calc_input_set_operator(&in, CALC_OP_DIV);
    calc_input_clear(&in);
    expect_text(&in, "0", "clear empties display");
    expect_true(in.pendingOp == CALC_OP_NONE, "clear resets operator");
    expect_false(in.secondStarted, "clear resets second operand flag");
}

static void TestInvalidInputs(void)
{
    CalcInput in;
    calc_input_init(&in);
    expect_false(calc_input_digit(&in, 'a'), "non-digit rejected");
    calc_input_digit(NULL, '1');
    calc_input_decimal(NULL);
    calc_input_toggle_sign(NULL);
    calc_input_backspace(NULL);
    calc_input_clear(NULL);
    calc_input_set_operator(NULL, CALC_OP_ADD);
    expect_true(1, "NULL calls are safe (NFR-04)");
}

int main(void)
{
    TestDigitAppend();
    TestBackspace();
    TestDecimalSingle();
    TestDecimalOnEmpty();
    TestToggleSign();
    TestToggleSignOnEmpty();
    TestLengthLimit();
    TestReInputOverwrite();
    TestReInputOverwriteDecimalAndSign();
    TestClear();
    TestInvalidInputs();

    printf("%d checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
