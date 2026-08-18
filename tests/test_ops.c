#include "calc.h"
#include "test_util.h"

static void expect_expression(CalcContext *ctx, const char *expected, const char *msg)
{
    const char *actual = calc_expression_text(ctx);
    test_checks++;
    if (strcmp(actual, expected) != 0)
    {
        test_failures++;
        printf("FAIL: %s - expected \"%s\", got \"%s\"\n", msg, expected, actual);
    }
}

static void TestAdd(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "12");
    calc_select_operator(&ctx, CALC_OP_ADD);
    enter_text(&ctx, "7");
    calc_equals(&ctx);
    expect_display(&ctx, "19", "AC-01: 12 + 7 = 19");
    expect_result(&ctx, 19.0, "AC-01 result value");
    expect_expression(&ctx, "12 + 7 =", "AC-01 expression text");
}

static void TestSub(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "20");
    calc_select_operator(&ctx, CALC_OP_SUB);
    enter_text(&ctx, "5");
    calc_equals(&ctx);
    expect_display(&ctx, "15", "AC-02: 20 - 5 = 15");
}

static void TestMul(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "6");
    calc_select_operator(&ctx, CALC_OP_MUL);
    enter_text(&ctx, "7");
    calc_equals(&ctx);
    expect_display(&ctx, "42", "AC-03: 6 x 7 = 42");
}

static void TestDiv(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "100");
    calc_select_operator(&ctx, CALC_OP_DIV);
    enter_text(&ctx, "4");
    calc_equals(&ctx);
    expect_display(&ctx, "25", "AC-04: 100 / 4 = 25");
}

static void TestDecimalOps(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "3.5");
    calc_select_operator(&ctx, CALC_OP_ADD);
    enter_text(&ctx, "2.25");
    calc_equals(&ctx);
    expect_display(&ctx, "5.75", "AC-05: 3.5 + 2.25 = 5.75");
}

static void TestNegative(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "-5");
    calc_select_operator(&ctx, CALC_OP_ADD);
    enter_text(&ctx, "3");
    calc_equals(&ctx);
    expect_display(&ctx, "-2", "AC-06: -5 + 3 = -2");
}

static void TestNegativeSecondOperand(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "5");
    calc_select_operator(&ctx, CALC_OP_ADD);
    enter_text(&ctx, "-3");
    calc_equals(&ctx);
    expect_display(&ctx, "2", "5 + (-3) = 2");
}

static void TestDivisionByZero(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "10");
    calc_select_operator(&ctx, CALC_OP_DIV);
    enter_text(&ctx, "0");
    calc_equals(&ctx);
    TCHECK(calc_state(&ctx) == CALC_STATE_ERROR, "AC-07: division by zero enters error state");
    expect_display(&ctx, "Error: Division by zero", "AC-07: error message shown");
    expect_expression(&ctx, "10 / 0 =", "AC-07: expression recorded on error");
}

static void TestErrorRecovery(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "10");
    calc_select_operator(&ctx, CALC_OP_DIV);
    enter_text(&ctx, "0");
    calc_equals(&ctx);
    TCHECK(calc_state(&ctx) == CALC_STATE_ERROR, "precondition: error state");

    TCHECK(!calc_digit(&ctx, '5'), "AC-08: input ignored in error state");
    calc_backspace(&ctx);
    calc_toggle_sign(&ctx);
    calc_select_operator(&ctx, CALC_OP_ADD);
    calc_equals(&ctx);
    TCHECK(calc_state(&ctx) == CALC_STATE_ERROR, "error state persists until clear");

    calc_clear(&ctx);
    TCHECK(calc_state(&ctx) == CALC_STATE_OPERAND1, "AC-08: clear restores normal state");
    expect_display(&ctx, "0", "AC-08: display reset after clear");
    enter_text(&ctx, "2");
    calc_select_operator(&ctx, CALC_OP_ADD);
    enter_text(&ctx, "2");
    calc_equals(&ctx);
    expect_display(&ctx, "4", "AC-08: calculation works after recovery");
}

static void TestClear(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "8");
    calc_select_operator(&ctx, CALC_OP_DIV);
    enter_text(&ctx, "2");
    calc_equals(&ctx);
    expect_expression(&ctx, "8 / 2 =", "expression before clear");
    calc_clear(&ctx);
    expect_display(&ctx, "0", "AC-09: clear empties display");
    TCHECK(calc_state(&ctx) == CALC_STATE_OPERAND1, "AC-09: clear resets all state");
    expect_expression(&ctx, "", "AC-09: clear resets expression line");
}

static void TestOperatorReplace(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "5");
    calc_select_operator(&ctx, CALC_OP_ADD);
    calc_select_operator(&ctx, CALC_OP_MUL);
    enter_text(&ctx, "3");
    calc_equals(&ctx);
    expect_display(&ctx, "15", "AC-11: operator replaced, 5 x 3 = 15");
}

static void TestChainedCalculation(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "2");
    calc_select_operator(&ctx, CALC_OP_ADD);
    enter_text(&ctx, "3");
    calc_equals(&ctx);
    expect_display(&ctx, "5", "AC-12: 2 + 3 = 5");
    expect_expression(&ctx, "2 + 3 =", "AC-12: first expression");
    calc_select_operator(&ctx, CALC_OP_MUL);
    enter_text(&ctx, "4");
    calc_equals(&ctx);
    expect_display(&ctx, "20", "AC-12: result reused, 5 x 4 = 20");
    expect_expression(&ctx, "5 x 4 =", "AC-12: chained expression uses result as operand1");
}

static void TestResultAsFirstOperand(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "2");
    calc_select_operator(&ctx, CALC_OP_ADD);
    enter_text(&ctx, "3");
    calc_equals(&ctx);
    calc_select_operator(&ctx, CALC_OP_ADD);
    enter_text(&ctx, "5");
    calc_equals(&ctx);
    expect_display(&ctx, "10", "FR-11: 5 + 5 = 10");
}

static void TestEqualsWithoutOperator(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "12");
    calc_equals(&ctx);
    expect_display(&ctx, "12", "equals without operator shows operand");
}

static void TestEqualsWithOnlyOperand1(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "5");
    calc_select_operator(&ctx, CALC_OP_ADD);
    calc_equals(&ctx);
    expect_display(&ctx, "10", "5 + = 10 (operand1 reused)");
}

static void TestBackspaceAfterOperator(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "12");
    calc_select_operator(&ctx, CALC_OP_ADD);
    calc_backspace(&ctx);
    expect_display(&ctx, "1", "FR-09: backspace edits first operand while waiting");
    enter_text(&ctx, "3");
    calc_equals(&ctx);
    expect_display(&ctx, "4", "1 + 3 = 4");
}

static void TestNewInputAfterResult(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "2");
    calc_select_operator(&ctx, CALC_OP_ADD);
    enter_text(&ctx, "3");
    calc_equals(&ctx);
    enter_text(&ctx, "9");
    expect_display(&ctx, "9", "typing after result starts fresh");
    expect_expression(&ctx, "2 + 3 =", "previous expression kept while typing");
    calc_equals(&ctx);
    expect_display(&ctx, "9", "fresh operand standalone equals");
    expect_expression(&ctx, "9 =", "expression updated on new equals");
}

static void TestOperatorAfterSecondOperand(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "5");
    calc_select_operator(&ctx, CALC_OP_ADD);
    enter_text(&ctx, "3");
    calc_select_operator(&ctx, CALC_OP_MUL);
    enter_text(&ctx, "4");
    calc_equals(&ctx);
    expect_display(&ctx, "20", "operator after second operand: discards 3, 5 x 4 = 20");
}

static void TestFloatFormatting(void)
{
    CalcContext ctx;
    calc_init(&ctx);
    enter_text(&ctx, "1");
    calc_select_operator(&ctx, CALC_OP_DIV);
    enter_text(&ctx, "3");
    calc_equals(&ctx);
    expect_display(&ctx, "0.3333333333", "1/3 formatted to 10 significant digits");

    calc_clear(&ctx);
    enter_text(&ctx, "0.1");
    calc_select_operator(&ctx, CALC_OP_ADD);
    enter_text(&ctx, "0.2");
    calc_equals(&ctx);
    expect_display(&ctx, "0.3", "0.1 + 0.2 formats cleanly");

    calc_clear(&ctx);
    enter_text(&ctx, "2");
    calc_select_operator(&ctx, CALC_OP_DIV);
    enter_text(&ctx, "4");
    calc_equals(&ctx);
    expect_display(&ctx, "0.5", "2/4 = 0.5");
}

int main(void)
{
    TestAdd();
    TestSub();
    TestMul();
    TestDiv();
    TestDecimalOps();
    TestNegative();
    TestNegativeSecondOperand();
    TestDivisionByZero();
    TestErrorRecovery();
    TestClear();
    TestOperatorReplace();
    TestChainedCalculation();
    TestResultAsFirstOperand();
    TestEqualsWithoutOperator();
    TestEqualsWithOnlyOperand1();
    TestBackspaceAfterOperator();
    TestNewInputAfterResult();
    TestOperatorAfterSecondOperand();
    TestFloatFormatting();

    printf("%d checks, %d failures\n", test_checks, test_failures);
    return test_failures == 0 ? 0 : 1;
}
