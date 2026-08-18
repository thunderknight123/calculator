#include "raylib.h"
#include "ui.h"

static UiDisplayState DisplayStateFromCalc(CalcState state)
{
    switch (state)
    {
        case CALC_STATE_ERROR:  return UI_DISPLAY_ERROR;
        case CALC_STATE_RESULT: return UI_DISPLAY_RESULT;
        default:                return UI_DISPLAY_INPUT;
    }
}

int main(void)
{
    CalcContext calc;
    UiDisplay display;

    calc_init(&calc);

    InitWindow(UI_WINDOW_WIDTH, UI_WINDOW_HEIGHT, "Calculator");
    SetExitKey(KEY_NULL);
    ui_init();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        UiEvent event;
        ui_begin_frame();
        while ((event = ui_poll_input()).key != UI_KEY_NONE)
        {
            ui_apply_event(&calc, event);
        }

        ui_display_set(&display, DisplayStateFromCalc(calc.state), calc_display_text(&calc));

        BeginDrawing();
        ClearBackground((Color){ 245, 245, 245, 255 });
        ui_display_render(&display);
        ui_render_buttons();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
