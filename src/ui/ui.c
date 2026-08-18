#include "ui.h"

#include <string.h>

#include "raylib.h"

#define UI_DISPLAY_AREA_HEIGHT 80
#define UI_DISPLAY_FONT_SIZE 40
#define UI_DISPLAY_PADDING 16

static Font ui_display_font;

static Color UiColor(unsigned char r, unsigned char g, unsigned char b)
{
    Color c;
    c.r = r; c.g = g; c.b = b; c.a = 255;
    return c;
}

static void InitUiDisplayFont(void)
{
    ui_display_font = LoadFontEx("C:/Windows/Fonts/consola.ttf", UI_DISPLAY_FONT_SIZE, 0, 0);
    if (ui_display_font.texture.id == 0)
    {
        ui_display_font = GetFontDefault();
    }
}

void ui_init(void)
{
    InitUiDisplayFont();
}

void ui_display_set(UiDisplay *display, UiDisplayState state, const char *text)
{
    if (display == NULL) return;
    display->state = state;
    if (text == NULL) text = "";
    strncpy(display->text, text, UI_DISPLAY_MAX_CHARS);
    display->text[UI_DISPLAY_MAX_CHARS] = '\0';
}

void ui_display_render(const UiDisplay *display)
{
    if (display == NULL) return;

    Color panel     = UiColor(255, 255, 255);
    Color separator = UiColor(200, 200, 200);
    Color textColor = UiColor(40, 40, 40);

    DrawRectangle(0, 0, UI_WINDOW_WIDTH, UI_DISPLAY_AREA_HEIGHT, panel);
    DrawLine(0, UI_DISPLAY_AREA_HEIGHT, UI_WINDOW_WIDTH, UI_DISPLAY_AREA_HEIGHT, separator);

    if (display->state == UI_DISPLAY_ERROR)
    {
        textColor = UiColor(220, 60, 60);
    }

    int textWidth = (int)MeasureTextEx(ui_display_font, display->text, UI_DISPLAY_FONT_SIZE, 2).x;
    Vector2 pos = {
        (float)(UI_WINDOW_WIDTH - UI_DISPLAY_PADDING - textWidth),
        (float)((UI_DISPLAY_AREA_HEIGHT - UI_DISPLAY_FONT_SIZE) / 2)
    };

    DrawTextEx(ui_display_font, display->text, pos, UI_DISPLAY_FONT_SIZE, 2, textColor);
}
