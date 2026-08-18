#include "raylib.h"
#include "ui.h"

int main(void)
{
    UiDisplay display;

    InitWindow(UI_WINDOW_WIDTH, UI_WINDOW_HEIGHT, "Calculator");
    ui_init();
    ui_display_set(&display, UI_DISPLAY_INPUT, "0");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground((Color){ 245, 245, 245, 255 });
        ui_display_render(&display);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
