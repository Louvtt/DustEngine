#ifndef _DUST_IO_KEYCODES_H_
#define _DUST_IO_KEYCODES_H_

// to convert into namespaced constexpr values?

// Key states
// Key up state (release + up)
#define DUST_STATE_UP      0
// key press state (last 1 frame)
#define DUST_STATE_PRESS   1 // 1 frame
// key down state (press + down)
#define DUST_STATE_DOWN    2
// key release state (last 1 frame)
#define DUST_STATE_RELEASE 3 // 1 frame

// The unknown key
#define DUST_KEY_UNKNOWN            -1
 
// Printable keys
#define DUST_KEY_SPACE              32
#define DUST_KEY_APOSTROPHE         39  // '
#define DUST_KEY_COMMA              44  // ,
#define DUST_KEY_MINUS              45  // -
#define DUST_KEY_PERIOD             46  // .
#define DUST_KEY_SLASH              47  // /
#define DUST_KEY_0                  48
#define DUST_KEY_1                  49
#define DUST_KEY_2                  50
#define DUST_KEY_3                  51
#define DUST_KEY_4                  52
#define DUST_KEY_5                  53
#define DUST_KEY_6                  54
#define DUST_KEY_7                  55
#define DUST_KEY_8                  56
#define DUST_KEY_9                  57
#define DUST_KEY_SEMICOLON          59  // ;
#define DUST_KEY_EQUAL              61  // =
#define DUST_KEY_A                  65
#define DUST_KEY_B                  66
#define DUST_KEY_C                  67
#define DUST_KEY_D                  68
#define DUST_KEY_E                  69
#define DUST_KEY_F                  70
#define DUST_KEY_G                  71
#define DUST_KEY_H                  72
#define DUST_KEY_I                  73
#define DUST_KEY_J                  74
#define DUST_KEY_K                  75
#define DUST_KEY_L                  76
#define DUST_KEY_M                  77
#define DUST_KEY_N                  78
#define DUST_KEY_O                  79
#define DUST_KEY_P                  80
#define DUST_KEY_Q                  81
#define DUST_KEY_R                  82
#define DUST_KEY_S                  83
#define DUST_KEY_T                  84
#define DUST_KEY_U                  85
#define DUST_KEY_V                  86
#define DUST_KEY_W                  87
#define DUST_KEY_X                  88
#define DUST_KEY_Y                  89
#define DUST_KEY_Z                  90
#define DUST_KEY_LEFT_BRACKET       91  // [
#define DUST_KEY_BACKSLASH          92  /* \ */
#define DUST_KEY_RIGHT_BRACKET      93  // ]
#define DUST_KEY_GRAVE_ACCENT       96  // `
#define DUST_KEY_OEM_1              161 // non-US #1
#define DUST_KEY_OEM_2              162 // non-US #2
 
/* Function keys */
#define DUST_KEY_ESCAPE             256
#define DUST_KEY_ENTER              257 /* return, enter */
#define DUST_KEY_TAB                258
#define DUST_KEY_BACKSPACE          259 /* left delete */
#define DUST_KEY_INSERT             260 
#define DUST_KEY_DELETE             261 /*Suppr, right delete*/
// arrows
#define DUST_KEY_RIGHT              262
#define DUST_KEY_LEFT               263
#define DUST_KEY_DOWN               264
#define DUST_KEY_UP                 265
// stranges keys
#define DUST_KEY_PAGE_UP            266
#define DUST_KEY_PAGE_DOWN          267
#define DUST_KEY_HOME               268
#define DUST_KEY_END                269
#define DUST_KEY_CAPS_LOCK          280
#define DUST_KEY_SCROLL_LOCK        281
#define DUST_KEY_NUM_LOCK           282
#define DUST_KEY_PRINT_SCREEN       283
#define DUST_KEY_PAUSE              284
// functions keys (to F25 lezgooo)
#define DUST_KEY_F1                 290
#define DUST_KEY_F2                 291
#define DUST_KEY_F3                 292
#define DUST_KEY_F4                 293
#define DUST_KEY_F5                 294
#define DUST_KEY_F6                 295
#define DUST_KEY_F7                 296
#define DUST_KEY_F8                 297
#define DUST_KEY_F9                 298
#define DUST_KEY_F10                299
#define DUST_KEY_F11                300
#define DUST_KEY_F12                301
#define DUST_KEY_F13                302
#define DUST_KEY_F14                303
#define DUST_KEY_F15                304
#define DUST_KEY_F16                305
#define DUST_KEY_F17                306
#define DUST_KEY_F18                307
#define DUST_KEY_F19                308
#define DUST_KEY_F20                309
#define DUST_KEY_F21                310
#define DUST_KEY_F22                311
#define DUST_KEY_F23                312
#define DUST_KEY_F24                313
#define DUST_KEY_F25                314

// keypad
#define DUST_KEY_KP_0               320
#define DUST_KEY_KP_1               321
#define DUST_KEY_KP_2               322
#define DUST_KEY_KP_3               323
#define DUST_KEY_KP_4               324
#define DUST_KEY_KP_5               325
#define DUST_KEY_KP_6               326
#define DUST_KEY_KP_7               327
#define DUST_KEY_KP_8               328
#define DUST_KEY_KP_9               329
#define DUST_KEY_KP_DECIMAL         330
#define DUST_KEY_KP_DIVIDE          331
#define DUST_KEY_KP_MULTIPLY        332
#define DUST_KEY_KP_SUBTRACT        333
#define DUST_KEY_KP_ADD             334
#define DUST_KEY_KP_ENTER           335
#define DUST_KEY_KP_EQUAL           336
#define DUST_KEY_LEFT_SHIFT         340
#define DUST_KEY_LEFT_CONTROL       341
#define DUST_KEY_LEFT_ALT           342
#define DUST_KEY_LEFT_SUPER         343
#define DUST_KEY_RIGHT_SHIFT        344
#define DUST_KEY_RIGHT_CONTROL      345
#define DUST_KEY_RIGHT_ALT          346
#define DUST_KEY_RIGHT_SUPER        347
#define DUST_KEY_MENU               348
 
#define DUST_KEY_LAST               DUST_KEY_MENU
 
// MODS
#define DUST_MOD_SHIFT           0x0001
#define DUST_MOD_CONTROL         0x0002
#define DUST_MOD_ALT             0x0004
#define DUST_MOD_SUPER           0x0008
#define DUST_MOD_CAPS_LOCK       0x0010
#define DUST_MOD_NUM_LOCK        0x0020
#define DUST_MOD_COUNT           6      // how many mods we handle

// MOUSE 
#define DUST_MBUTTON_1         0
#define DUST_MBUTTON_2         1
#define DUST_MBUTTON_3         2
#define DUST_MBUTTON_4         3
#define DUST_MBUTTON_5         4
#define DUST_MBUTTON_6         5
#define DUST_MBUTTON_7         6
#define DUST_MBUTTON_8         7
#define DUST_MBUTTON_LAST      DUST_MBUTTON_8
#define DUST_MBUTTON_LEFT      DUST_MBUTTON_1
#define DUST_MBUTTON_RIGHT     DUST_MBUTTON_2
#define DUST_MBUTTON_MIDDLE    DUST_MBUTTON_3

#endif //_DUST_IO_KEYCODES_H_