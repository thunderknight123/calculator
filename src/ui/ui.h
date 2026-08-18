#ifndef CALCULATOR_UI_H
#define CALCULATOR_UI_H

#define UI_WINDOW_WIDTH  300
#define UI_WINDOW_HEIGHT 420
#define UI_DISPLAY_MAX_CHARS 20

typedef enum {
    UI_DISPLAY_INPUT = 0,
    UI_DISPLAY_RESULT,
    UI_DISPLAY_ERROR
} UiDisplayState;

typedef struct {
    char text[UI_DISPLAY_MAX_CHARS + 1];
    UiDisplayState state;
} UiDisplay;

void ui_init(void);
void ui_display_set(UiDisplay *display, UiDisplayState state, const char *text);
void ui_display_render(const UiDisplay *display);

#endif
