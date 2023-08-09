#include "action_layer.h"
#include "quantum/pointing_device.h"         // pointing_device_get_report
#include "config.h"
#include "ino.h"

// Layers
enum {LY_MAC   = 0,
      LY_LINUX,
      LY_WIN,
      LY_RAISE = 14,
      LY_LOWER
};

#define xxx  KC_NO
#define ____ KC_TRANSPARENT

#define ID_MS_BTN_L 0               // Mouse buttons
#define ID_MS_BTN_R 1
#define ID_MS_BTN_M 2
#define KC_MSL F(ID_MS_BTN_L)
#define KC_MSR F(ID_MS_BTN_R)
#define KC_MSM F(ID_MS_BTN_M)

#define RAISE MO(LY_RAISE)          // RAISE Key
#define LOWER MO(LY_LOWER)          // LOWER Key (Not assigned)

#define HSCRL KC_LSFT               // Enable horizontal scroll during ring-rotation holding this key

// Switch IME with tapping Raise/Lower, thanks to http://leopardgecko.hatenablog.com/entry/2017/11/09/124103
enum user_macro {
    //                                  Hold      Tap
    UM_L_EIS,                       //  Lower     EISU,MUHENKAN(JP)
    UM_R_KAN,                       //  Raise     KANA,HENKAN(JP)
    UM_L_SPC,                       //  Lower     Space
    UM_R_SPC,                       //  Raise     Space
    UM_HSCRL                        //  Enable horizontal scroll
};
#define L_EISU MACROTAP(UM_L_EIS)   // Lower/EISU
#define R_KANA MACROTAP(UM_R_KAN)   // Raise/KANA
#define L_SPC  MACROTAP(UM_L_SPC)   // Lower/Space
#define R_SPC  MACROTAP(UM_R_SPC)   // Raise/Space

#define US_EISU  LSFT(KC_CAPS)      // EISU on US layout

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    switch(id) {
#if 0
        case UM_L_EIS:              // Hold:Lower   Tap:EISU,MUHENKAN
            return MACRO_TAP_HOLD_LAYER( record, MACRO(T(MHEN), T(LANG2), END), LY_LOWER );
        case UM_R_KAN:              // Hold:Raise   Tap:KANA,HENKAN
            return MACRO_TAP_HOLD_LAYER( record, MACRO(T(HENK), T(LANG1), END), LY_RAISE );

        case UM_L_SPC:              // Hold:Lower   Tap:Space
            return MACRO_TAP_HOLD_LAYER( record, MACRO(T(MHEN), T(LANG2), END), LY_LOWER );
        case UM_R_SPC:              // Hold:Raise   Tap:Space
            return MACRO_TAP_HOLD_LAYER( record, MACRO(T(HENK), T(LANG1), END), LY_RAISE );
#endif

        case UM_HSCRL:
            enable_horizontal_scroll = true;
            return MACRO_NONE;
        default:
            return MACRO_NONE;
    };
    return MACRO_NONE;
}

enum my_keycodes {
    S_MSM = SAFE_RANGE,       // Shift + Mouse Middle Button (for Orbit on Fusion360)
};

#define PROCESS_OVERRIDE_BEHAVIOR   (false)
#define PROCESS_USUAL_BEHAVIOR      (true)
bool process_record_user(uint16_t keycode, keyrecord_t *record)
{
    const uint8_t mask_middle_button = MOUSE_BTN3;
    report_mouse_t mouse_report = pointing_device_get_report();


    switch (keycode) {
        case S_MSM:
            if (record->event.pressed) {
                add_key(KC_LSHIFT);
                send_keyboard_report();

                mouse_report.buttons |= mask_middle_button;
                pointing_device_set_report(mouse_report);
                // pointing_device_send();
            }
            else {
                del_key(KC_LSHIFT);
                send_keyboard_report();

                mouse_report.buttons &= ~mask_middle_button;
                pointing_device_set_report(mouse_report);
                // pointing_device_send();
            }

            return PROCESS_OVERRIDE_BEHAVIOR;
            break;

        default:
            break;
    }

    return PROCESS_USUAL_BEHAVIOR;
}


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/*
    C-L0   C-L1   C-L2   C-L3   C-L4   C-L5   C-L6   C-L7              C-R0   C-R1   C-R2   C-R3   C-R4   C-R5   C-R6   C-R7
  -------+------+------+------+------+------+------+------+          +------+------+------+------+------+------+------+------+
  |   `  |   1  |   2  |   3  |   4  |   5  |   6  |      |          |      |   7  |   8  |   9  |   0  |   -  |   =  |  bs  | R0
  -------+------+------+------+------+------+------+------+          +------+------+------+------+------+------+------+------+
  |  tab |   q  |   w  |   e  |   r  |   t  |      |      |          |   y  |   u  |   i  |   o  |   p  |  [   |   ]  |   \  | R1
  -------+------+------+------+------+------+------+------+          +------+------+------+------+------+------+------+------+
  | caps |   a  |   s  |   d  |   f  |   g  |      |      |          |   h  |   j  |   k  |   l  |   ;  |   '  | enter|      | R2
  -------+------+------+------+------+------+------+------+          +------+------+------+------+------+------+------+------+
  |      |shift |   z  |   x  |   c  |   v  |   b  |      |          |   b  |   n  |   m  |   ,  |   .  |   /  |  up  |  del | R3
  -------+------+------+------+------+------+------+------+          +------+------+------+------+------+------+------+------+
  |  esc | ctrl |  win |  cmd | space| MS-M | MS-L |      |          | MB-R | space| ctrl | RAISE| LOWER| left | down | right| R4
  |      |      |      |  alt |      |      |      |      |          |      |      |      |      |      |      |      |      |
  -------+------+------+------+------+------+------+------+          +------+------+------+------+------+------+------+------+
 */

[LY_MAC] = LAYOUT(
  KC_GRV,   KC_1,    KC_2,    KC_3,  KC_4,    KC_5,   KC_6,  xxx,    xxx,    KC_7,   KC_8,    KC_9,    KC_0,    KC_MINUS, KC_EQUAL, KC_BSPC, \
  KC_TAB,   KC_Q,    KC_W,    KC_E,  KC_R,    KC_T,   xxx,   xxx,    KC_Y,   KC_U,   KC_I,    KC_O,    KC_P,    KC_LBRC,  KC_RBRC,  KC_BSLS, \
  US_EISU,  KC_A,    KC_S,    KC_D,  KC_F,    KC_G,   xxx,   xxx,    KC_H,   KC_J,   KC_K,    KC_L,    KC_SCLN, KC_QUOT,  KC_ENT,   xxx,     \
  xxx,     KC_LSFT,  KC_Z,    KC_X,  KC_C,    KC_V,   KC_B,  xxx,    KC_B,   KC_N,   KC_M,    KC_COMM, KC_DOT,  KC_SLSH,  KC_UP,    KC_DEL,  \
  KC_ESC,  KC_LALT,  S_MSM, KC_LWIN, KC_SPC, KC_MSM, KC_MSL, xxx,    KC_MSR, KC_SPC, KC_RCTL, RAISE,   LOWER,   KC_LEFT,  KC_DOWN,  KC_RIGHT \
  ),

[LY_LINUX] = LAYOUT(
  ____,    ____,     ____,    ____,  ____,    ____,   ____,   ____,   ____,   ____,   ____,    ____,    ____,    ____,     ____,     ____,    \
  ____,    ____,     ____,    ____,  ____,    ____,   ____,   ____,   ____,   ____,   ____,    ____,    ____,    ____,     ____,     ____,    \
  ____,    ____,     ____,    ____,  ____,    ____,   ____,   ____,   ____,   ____,   ____,    ____,    ____,    ____,     ____,     ____,    \
  ____,    ____,     ____,    ____,  ____,    ____,   ____,   ____,   ____,   ____,   ____,    ____,    ____,    ____,     ____,     ____,    \
  ____,   KC_LWIN,   ____, KC_LALT,  ____,    ____,   ____,   ____,   ____,   ____,   ____,    ____,    ____,    ____,     ____,     ____     \
  ),

[LY_WIN] = LAYOUT(
  ____,    ____,     ____,    ____,  ____,    ____,   ____,   ____,   ____,   ____,   ____,    ____,    ____,    ____,     ____,     ____,    \
  ____,    ____,     ____,    ____,  ____,    ____,   ____,   ____,   ____,   ____,   ____,    ____,    ____,    ____,     ____,     ____,    \
  ____,    ____,     ____,    ____,  ____,    ____,   ____,   ____,   ____,   ____,   ____,    ____,    ____,    ____,     ____,     ____,    \
  ____,    ____,     ____,    ____,  ____,    ____,   ____,   ____,   ____,   ____,   ____,    ____,    ____,    ____,     ____,     ____,    \
  ____,   KC_LWIN,   ____, KC_LALT,  ____,    ____,   ____,   ____,   ____,   ____,   ____,    ____,    ____,    ____,     ____,     ____     \
  ),

/* Raise
     C-L0   C-L1   C-L2   C-L3   C-L4   C-L5   C-L6   C-L7             C-R0   C-R1   C-R2   C-R3   C-R4   C-R5   C-R6   C-R7
   +-------------------------------------------------------+         +-------------------------------------------------------+
   | reset|  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |      |         |      |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |      | R0
   +-------------------------------------------------------+         +-------------------------------------------------------+
   |      |      |      |      |      |      |      |      |         |      |      |      |      |      |      |      |      | R1
   +-------------------------------------------------------+         +-------------------------------------------------------+
   |      |      |      |      |      |      |      |      |         |      |      |      |      |      |      |      |      | R2
   +-------------------------------------------------------+         +-------------------------------------------------------+
   |      |      |      |      |      |      |      |      |         |      |      |      |      |      |      |      |      | R3
   +-------------------------------------------------------+         +-------------------------------------------------------+
   |      |      |      |      |      |      |      |      |         |      |      |      | RAISE|      |      |      |      | R4
   +-------------------------------------------------------+         +-------------------------------------------------------+
 */

[LY_RAISE] = LAYOUT(
  RESET,   KC_F1,    KC_F2,   KC_F3, KC_F4,  KC_F5,  KC_F6,  xxx,    xxx,    KC_F7,  KC_F8,   KC_F9,   KC_F10,  KC_F11,   KC_F12,   xxx,     \
  xxx,     xxx,      xxx,     xxx,   xxx,    xxx,    xxx,    xxx,    xxx,    xxx,    xxx,     xxx,     xxx,     xxx,      xxx,      xxx,     \
  xxx,     xxx,      xxx,     xxx,   xxx,    xxx,    xxx,    xxx,    xxx,    xxx,    xxx,     xxx,     xxx,     xxx,      xxx,      xxx,     \
  xxx,     xxx,      xxx,     xxx,   xxx,    xxx,    xxx,    xxx,    xxx,    xxx,    xxx,     xxx,     xxx,     xxx,      xxx,      xxx,     \
  xxx,     xxx,      xxx,     xxx,   xxx,    xxx,    xxx,    xxx,    xxx,    xxx,    xxx,    ____,     xxx,     xxx,      xxx,      xxx      \
  ),

/* Lower
     C-L0   C-L1   C-L2   C-L3   C-L4   C-L5   C-L6   C-L7             C-R0   C-R1   C-R2   C-R3   C-R4   C-R5   C-R6   C-R7
   +-------------------------------------------------------+         +-------------------------------------------------------+
   |      |      |      |      |      |      |      |      |         |      |      |      |      |      |      |      |      | R0
   +-------------------------------------------------------+         +-------------------------------------------------------+
   |      |      |      |      |      |      |      |      |         |      |      |      |      |      |      |      |      | R1
   +-------------------------------------------------------+         +-------------------------------------------------------+
   |      |      |      |      |      |      |      |      |         |      |      |      |      |      |      |      |      | R2
   +-------------------------------------------------------+         +-------------------------------------------------------+
   |      |      |      |      |      |      |      |      |         |      |      |      |      |      | home | pgup |  end | R3
   +-------------------------------------------------------+         +-------------------------------------------------------+
   |      |      |      |      |      |      |      |      |         |      |      |      |      | LOWER|      | pgdn |      | R4
   +-------------------------------------------------------+         +-------------------------------------------------------+
 */

[LY_LOWER] = LAYOUT(
  xxx,     xxx,      xxx,     xxx,   xxx,    xxx,    xxx,    xxx,    xxx,    xxx,    xxx,     xxx,     xxx,     xxx,      xxx,      xxx,     \
  xxx,     xxx,      xxx,     xxx,   xxx,    xxx,    xxx,    xxx,    xxx,    xxx,    xxx,     xxx,     xxx,     xxx,      xxx,      xxx,     \
  xxx,     xxx,      xxx,     xxx,   xxx,    xxx,    xxx,    xxx,    xxx,    xxx,    xxx,     xxx,     xxx,     xxx,      xxx,      xxx,     \
  xxx,     xxx,      xxx,     xxx,   xxx,    xxx,    xxx,    xxx,    xxx,    xxx,    xxx,     xxx,     xxx,     KC_HOME,  KC_PGUP,  KC_END,  \
  xxx,     xxx,      xxx,     xxx,   xxx,    xxx,    xxx,    xxx,    xxx,    xxx,    xxx,     xxx,    ____,     xxx,      KC_PGDN,  xxx      \
  )
};
