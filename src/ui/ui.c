#include "ui.h"

#include <string.h>

#include "raylib.h"

#define UI_DISPLAY_AREA_HEIGHT 80
#define UI_DISPLAY_FONT_SIZE 40
#define UI_DISPLAY_PADDING 16
#define UI_BUTTON_PAD 8
#define UI_BUTTON_GAP 6

typedef struct {
    Rectangle rect;
    const char *label;
    UiKey key;
    char digit;
} UiButton;

static Font ui_display_font;
static UiButton ui_buttons[19];
static int ui_button_count = 0;
static bool frame_key_events_consumed = false;

static Color UiColor(unsigned char r, unsigned char g, unsigned char b)
{
    Color c;
    c.r = r; c.g = g; c.b = b; c.a = 255;
    return c;
}

static Color Shade(Color c, float factor)
{
    Color out;
    out.r = (unsigned char)(c.r * factor);
    out.g = (unsigned char)(c.g * factor);
    out.b = (unsigned char)(c.b * factor);
    out.a = 255;
    return out;
}

static void InitUiDisplayFont(void)
{
    ui_display_font = LoadFontEx("C:/Windows/Fonts/consola.ttf", UI_DISPLAY_FONT_SIZE, 0, 0);
    if (ui_display_font.texture.id == 0)
    {
        ui_display_font = GetFontDefault();
    }
}

static void AddButton(int row, int col, int colspan, const char *label, UiKey key, char digit)
{
    float colWidth = (UI_WINDOW_WIDTH - 2 * UI_BUTTON_PAD - 3 * UI_BUTTON_GAP) / 4.0f;
    float rowHeight = (UI_WINDOW_HEIGHT - UI_DISPLAY_AREA_HEIGHT - UI_BUTTON_PAD - 4 * UI_BUTTON_GAP) / 5.0f;

    ui_buttons[ui_button_count].rect.x = UI_BUTTON_PAD + col * (colWidth + UI_BUTTON_GAP);
    ui_buttons[ui_button_count].rect.y = UI_DISPLAY_AREA_HEIGHT + UI_BUTTON_PAD + row * (rowHeight + UI_BUTTON_GAP);
    ui_buttons[ui_button_count].rect.width = colWidth * colspan + (colspan - 1) * UI_BUTTON_GAP;
    ui_buttons[ui_button_count].rect.height = rowHeight;
    ui_buttons[ui_button_count].label = label;
    ui_buttons[ui_button_count].key = key;
    ui_buttons[ui_button_count].digit = digit;
    ui_button_count++;
}

static void InitUiButtons(void)
{
    AddButton(0, 0, 2, "<-", UI_KEY_BACKSPACE, 0);
    AddButton(0, 2, 1, "C", UI_KEY_CLEAR, 0);
    AddButton(0, 3, 1, "/", UI_KEY_DIV, 0);
    AddButton(1, 0, 1, "7", UI_KEY_DIGIT, '7');
    AddButton(1, 1, 1, "8", UI_KEY_DIGIT, '8');
    AddButton(1, 2, 1, "9", UI_KEY_DIGIT, '9');
    AddButton(1, 3, 1, "x", UI_KEY_MUL, 0);
    AddButton(2, 0, 1, "4", UI_KEY_DIGIT, '4');
    AddButton(2, 1, 1, "5", UI_KEY_DIGIT, '5');
    AddButton(2, 2, 1, "6", UI_KEY_DIGIT, '6');
    AddButton(2, 3, 1, "-", UI_KEY_SUB, 0);
    AddButton(3, 0, 1, "1", UI_KEY_DIGIT, '1');
    AddButton(3, 1, 1, "2", UI_KEY_DIGIT, '2');
    AddButton(3, 2, 1, "3", UI_KEY_DIGIT, '3');
    AddButton(3, 3, 1, "+", UI_KEY_ADD, 0);
    AddButton(4, 0, 1, "+/-", UI_KEY_SIGN, 0);
    AddButton(4, 1, 1, "0", UI_KEY_DIGIT, '0');
    AddButton(4, 2, 1, ".", UI_KEY_DOT, 0);
    AddButton(4, 3, 1, "=", UI_KEY_EQUALS, 0);
}

void ui_init(void)
{
    InitUiDisplayFont();
    InitUiButtons();
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

static UiButton *FindButtonAt(Vector2 point)
{
    int i;
    for (i = 0; i < ui_button_count; i++)
    {
        if (CheckCollisionPointRec(point, ui_buttons[i].rect))
        {
            return &ui_buttons[i];
        }
    }
    return NULL;
}

void ui_begin_frame(void)
{
    frame_key_events_consumed = false;
}

UiEvent ui_poll_input(void)
{
    UiEvent ev;
    ev.key = UI_KEY_NONE;
    ev.digit = 0;

    int c = GetCharPressed();
    if (c >= '0' && c <= '9') { ev.key = UI_KEY_DIGIT; ev.digit = (char)c; return ev; }
    if (c == '.') { ev.key = UI_KEY_DOT; return ev; }
    if (c == '+') { ev.key = UI_KEY_ADD; return ev; }
    if (c == '-') { ev.key = UI_KEY_SUB; return ev; }
    if (c == '*') { ev.key = UI_KEY_MUL; return ev; }
    if (c == '/') { ev.key = UI_KEY_DIV; return ev; }
    if (c == '=') { ev.key = UI_KEY_EQUALS; return ev; }

    if (frame_key_events_consumed) return ev;
    frame_key_events_consumed = true;

    if (IsKeyPressed(KEY_KP_0)) { ev.key = UI_KEY_DIGIT; ev.digit = '0'; return ev; }
    if (IsKeyPressed(KEY_KP_1)) { ev.key = UI_KEY_DIGIT; ev.digit = '1'; return ev; }
    if (IsKeyPressed(KEY_KP_2)) { ev.key = UI_KEY_DIGIT; ev.digit = '2'; return ev; }
    if (IsKeyPressed(KEY_KP_3)) { ev.key = UI_KEY_DIGIT; ev.digit = '3'; return ev; }
    if (IsKeyPressed(KEY_KP_4)) { ev.key = UI_KEY_DIGIT; ev.digit = '4'; return ev; }
    if (IsKeyPressed(KEY_KP_5)) { ev.key = UI_KEY_DIGIT; ev.digit = '5'; return ev; }
    if (IsKeyPressed(KEY_KP_6)) { ev.key = UI_KEY_DIGIT; ev.digit = '6'; return ev; }
    if (IsKeyPressed(KEY_KP_7)) { ev.key = UI_KEY_DIGIT; ev.digit = '7'; return ev; }
    if (IsKeyPressed(KEY_KP_8)) { ev.key = UI_KEY_DIGIT; ev.digit = '8'; return ev; }
    if (IsKeyPressed(KEY_KP_9)) { ev.key = UI_KEY_DIGIT; ev.digit = '9'; return ev; }

    if (IsKeyPressed(KEY_KP_DECIMAL)) { ev.key = UI_KEY_DOT; return ev; }
    if (IsKeyPressed(KEY_BACKSPACE)) { ev.key = UI_KEY_BACKSPACE; return ev; }
    if (IsKeyPressed(KEY_ESCAPE)) { ev.key = UI_KEY_CLEAR; return ev; }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_KP_EQUAL)) { ev.key = UI_KEY_EQUALS; return ev; }

    if (IsKeyPressed(KEY_KP_ADD)) { ev.key = UI_KEY_ADD; return ev; }
    if (IsKeyPressed(KEY_KP_SUBTRACT)) { ev.key = UI_KEY_SUB; return ev; }
    if (IsKeyPressed(KEY_KP_MULTIPLY)) { ev.key = UI_KEY_MUL; return ev; }
    if (IsKeyPressed(KEY_KP_DIVIDE)) { ev.key = UI_KEY_DIV; return ev; }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        UiButton *button = FindButtonAt(GetMousePosition());
        if (button != NULL)
        {
            ev.key = button->key;
            ev.digit = button->digit;
            return ev;
        }
    }

    return ev;
}

static Color ButtonColor(const UiButton *button)
{
    switch (button->key)
    {
        case UI_KEY_DIGIT:
        case UI_KEY_DOT:
            return UiColor(232, 232, 232);
        case UI_KEY_CLEAR:
            return UiColor(255, 120, 120);
        default:
            return UiColor(255, 176, 77);
    }
}

static Color ButtonTextColor(const UiButton *button)
{
    switch (button->key)
    {
        case UI_KEY_DIGIT:
        case UI_KEY_DOT:
            return UiColor(40, 40, 40);
        default:
            return UiColor(255, 255, 255);
    }
}

void ui_render_buttons(void)
{
    Vector2 mouse = GetMousePosition();
    int i;

    for (i = 0; i < ui_button_count; i++)
    {
        UiButton *button = &ui_buttons[i];
        bool hovered = CheckCollisionPointRec(mouse, button->rect);
        bool pressed = hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

        Color color = ButtonColor(button);
        Color textColor = ButtonTextColor(button);

        if (pressed)
        {
            color = Shade(color, 0.75f);
        }
        else if (hovered)
        {
            color = Shade(color, 0.9f);
        }

        DrawRectangleRounded(button->rect, 0.15f, 8, color);
        DrawRectangleRoundedLines(button->rect, 0.15f, 8, UiColor(150, 150, 150));

        int labelWidth = (int)MeasureText(button->label, 26);
        Vector2 pos = {
            button->rect.x + (button->rect.width - labelWidth) / 2.0f,
            button->rect.y + (button->rect.height - 26) / 2.0f
        };
        DrawText(button->label, (int)pos.x, (int)pos.y, 26, textColor);
    }
}

void ui_apply_event(CalcContext *ctx, UiEvent event)
{
    if (ctx == NULL) return;

    switch (event.key)
    {
        case UI_KEY_DIGIT:
            calc_digit(ctx, event.digit);
            break;
        case UI_KEY_DOT:
            calc_decimal(ctx);
            break;
        case UI_KEY_SIGN:
            calc_toggle_sign(ctx);
            break;
        case UI_KEY_CLEAR:
            calc_clear(ctx);
            break;
        case UI_KEY_BACKSPACE:
            calc_backspace(ctx);
            break;
        case UI_KEY_ADD:
            calc_select_operator(ctx, CALC_OP_ADD);
            break;
        case UI_KEY_SUB:
            if (ctx->state == CALC_STATE_OPERAND1 && ctx->input.length == 0)
            {
                calc_toggle_sign(ctx);
            }
            else
            {
                calc_select_operator(ctx, CALC_OP_SUB);
            }
            break;
        case UI_KEY_MUL:
            calc_select_operator(ctx, CALC_OP_MUL);
            break;
        case UI_KEY_DIV:
            calc_select_operator(ctx, CALC_OP_DIV);
            break;
        case UI_KEY_EQUALS:
            calc_equals(ctx);
            break;
        default:
            break;
    }
}
