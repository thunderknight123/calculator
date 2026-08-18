#ifndef CALCULATOR_UI_H
#define CALCULATOR_UI_H

#include "calc.h"

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

typedef enum {
    UI_KEY_NONE = 0,
    UI_KEY_DIGIT,
    UI_KEY_DOT,
    UI_KEY_SIGN,
    UI_KEY_CLEAR,
    UI_KEY_BACKSPACE,
    UI_KEY_ADD,
    UI_KEY_SUB,
    UI_KEY_MUL,
    UI_KEY_DIV,
    UI_KEY_EQUALS
} UiKey;

typedef struct {
    UiKey key;
    char digit;
} UiEvent;

void ui_init(void);
void ui_begin_frame(void);
void ui_display_set(UiDisplay *display, UiDisplayState state, const char *text);
void ui_display_render(const UiDisplay *display);
UiEvent ui_poll_input(void);
void ui_render_buttons(void);
void ui_apply_event(CalcContext *ctx, UiEvent event);

#endif
