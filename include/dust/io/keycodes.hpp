#ifndef _DUST_IO_KEYCODES_H_
#define _DUST_IO_KEYCODES_H_

#include "dust/core/types.hpp"

namespace dust {

// Key states
enum class KeyState : u32 {
    // Key Up state (release + Up)
    Up,
    // key press state (last 1 frame)
    Press,
    // key Down state (press + Down)
    Down,
    // key release state (last 1 frame)
    Release,
};
// Button state (same as keystate)
using ButtonState = KeyState;

enum class Key : u32 {
// The unknown key
    Unknown            = (u32)-1, // max
 
    // From 0-32 these are system-reserved
    
    // Printable keys

    Space              = 32,
    Apostrophe         = 39,  // '
    Comma              = 44,  // ,
    Minus              = 45,  // -
    Period             = 46,  // .
    Slash              = 47,  // /
    Key_0              = 48,
    Key_1              = 49,
    Key_2              = 50,
    Key_3              = 51,
    Key_4              = 52,
    Key_5              = 53,
    Key_6              = 54,
    Key_7              = 55,
    Key_8              = 56,
    Key_9              = 57,
    Semicolon          = 59,  // ;
    Equal              = 61,  // =
    A                  = 65,
    B                  = 66,
    C                  = 67,
    D                  = 68,
    E                  = 69,
    F                  = 70,
    G                  = 71,
    H                  = 72,
    I                  = 73,
    J                  = 74,
    K                  = 75,
    L                  = 76,
    M                  = 77,
    N                  = 78,
    O                  = 79,
    P                  = 80,
    Q                  = 81,
    R                  = 82,
    S                  = 83,
    T                  = 84,
    U                  = 85,
    V                  = 86,
    W                  = 87,
    X                  = 88,
    Y                  = 89,
    Z                  = 90,
    LeftBracket        = 91,  // [
    BackSlash          = 92,  /* \ */
    RightBracket       = 93,  // ]
    GraveAccent        = 96,  // `
    OEM_1              = 161, // non-US #1
    OEM_2              = 162, // non-US #2
 
/* Function keys */
    Escape             = 256,
    Enter              = 257, /* return, enter */
    Tab                = 258,
    Backspace          = 259, /* left delete */
    Insert             = 260, 
    Delete             = 261, /*SUppr, right delete*/
// arrows
    Right              = 262,
    Left               = 263,
    Down               = 264,
    Up                 = 265,
// stranges keys
    PageUp             = 266,
    PageDown           = 267,
    Home               = 268,
    End                = 269,
    CapsLock           = 280,
    ScrollLock         = 281,
    NumLock            = 282,
    PrintScreen        = 283,
    Pause              = 284,
// functions keys (to F25 lezgooo)
    F1                 = 290,
    F2                 = 291,
    F3                 = 292,
    F4                 = 293,
    F5                 = 294,
    F6                 = 295,
    F7                 = 296,
    F8                 = 297,
    F9                 = 298,
    F10                = 299,
    F11                = 300,
    F12                = 301,
    F13                = 302,
    F14                = 303,
    F15                = 304,
    F16                = 305,
    F17                = 306,
    F18                = 307,
    F19                = 308,
    F20                = 309,
    F21                = 310,
    F22                = 311,
    F23                = 312,
    F24                = 313,
    F25                = 314,

// keypad
    KeyPad_0               = 320,
    KeyPad_1               = 321,
    KeyPad_2               = 322,
    KeyPad_3               = 323,
    KeyPad_4               = 324,
    KeyPad_5               = 325,
    KeyPad_6               = 326,
    KeyPad_7               = 327,
    KeyPad_8               = 328,
    KeyPad_9               = 329,
    KeyPadDecimal          = 330,
    KeyPadDivide           = 331,
    KeyPadMultiply         = 332,
    KeyPadSubstract        = 333,
    KeyPadAdd              = 334,
    KeyPadEnter            = 335,
    KeyPadEqual            = 336,
    LeftShift              = 340,
    LeftControl            = 341,
    LeftAlt                = 342,
    LeftSuper              = 343,
    RightShift             = 344,
    RightControl           = 345,
    RightAlt               = 346,
    RightSuper             = 347,
    Menu                   = 348,
 
    Last                   = Menu,
};

// MODS
enum class Mod : u32 {
    Shift           = 0x0001,
    Control         = 0x0002,
    Alt             = 0x0004,
    Super           = 0x0008,
    CapsLock        = 0x0010,
    NumLock         = 0x0020,
    Count           = 6      // how many mods we handle
};
// extern Mod operator|(const Mod& first, const Mod& second);
// extern Mod operator&(const Mod& first, const Mod& second);

// MOUSE 
enum class MButton : u32 {
    Button_1         = 0,
    Button_2         = 1,
    Button_3         = 2,
    Button_4         = 3,
    Button_5         = 4,
    Button_6         = 5,
    Button_7         = 6,
    Button_8         = 7,
    Last             = Button_8,
    Left             = Button_1,
    Right            = Button_2,
    Middle           = Button_3,
};

}

#endif //_DUST_IO_KEYCODES_H_