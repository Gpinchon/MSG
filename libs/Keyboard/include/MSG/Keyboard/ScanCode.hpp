#pragma once

#include <cstdint>

namespace MSG::Keyboard {
/// @brief shamelessly stolen from SDL_scancode.h
enum class ScanCode : uint16_t {
    Unknown            = 0,
    KeyA               = 4,
    KeyB               = 5,
    KeyC               = 6,
    KeyD               = 7,
    KeyE               = 8,
    KeyF               = 9,
    KeyG               = 10,
    KeyH               = 11,
    KeyI               = 12,
    KeyJ               = 13,
    KeyK               = 14,
    KeyL               = 15,
    KeyM               = 16,
    KeyN               = 17,
    KeyO               = 18,
    KeyP               = 19,
    KeyQ               = 20,
    KeyR               = 21,
    KeyS               = 22,
    KeyT               = 23,
    KeyU               = 24,
    KeyV               = 25,
    KeyW               = 26,
    KeyX               = 27,
    KeyY               = 28,
    KeyZ               = 29,
    Key1               = 30,
    Key2               = 31,
    Key3               = 32,
    Key4               = 33,
    Key5               = 34,
    Key6               = 35,
    Key7               = 36,
    Key8               = 37,
    Key9               = 38,
    Key0               = 39,
    Return             = 40,
    Escape             = 41,
    Backspace          = 42,
    Tab                = 43,
    Space              = 44,
    Minus              = 45,
    Equals             = 46,
    LeftBracket        = 47,
    RightBracket       = 48,
    Backslash          = 49,
    Nonushash          = 50,
    SemiColon          = 51,
    Apostrophe         = 52,
    Grave              = 53,
    Comma              = 54,
    Period             = 55,
    Slash              = 56,
    CapsLock           = 57,
    F1                 = 58,
    F2                 = 59,
    F3                 = 60,
    F4                 = 61,
    F5                 = 62,
    F6                 = 63,
    F7                 = 64,
    F8                 = 65,
    F9                 = 66,
    F10                = 67,
    F11                = 68,
    F12                = 69,
    PrintScreen        = 70,
    ScrollLock         = 71,
    Pause              = 72,
    Insert             = 73,
    Home               = 74,
    PageUp             = 75,
    Delete             = 76,
    End                = 77,
    PageDown           = 78,
    Right              = 79,
    Left               = 80,
    Down               = 81,
    Up                 = 82,
    NumLockClear       = 83,
    KpDivide           = 84,
    KpMultiply         = 85,
    KpMinus            = 86,
    KpPlus             = 87,
    KpEnter            = 88,
    Kp1                = 89,
    Kp2                = 90,
    Kp3                = 91,
    Kp4                = 92,
    Kp5                = 93,
    Kp6                = 94,
    Kp7                = 95,
    Kp8                = 96,
    Kp9                = 97,
    Kp0                = 98,
    KpPeriod           = 99,
    NonusBackslash     = 100,
    Application        = 101,
    Power              = 102,
    KpEquals           = 103,
    F13                = 104,
    F14                = 105,
    F15                = 106,
    F16                = 107,
    F17                = 108,
    F18                = 109,
    F19                = 110,
    F20                = 111,
    F21                = 112,
    F22                = 113,
    F23                = 114,
    F24                = 115,
    Execute            = 116,
    Help               = 117,
    Menu               = 118,
    Select             = 119,
    Stop               = 120,
    Again              = 121,
    Undo               = 122,
    Cut                = 123,
    Copy               = 124,
    Paste              = 125,
    Find               = 126,
    Mute               = 127,
    VolumeUp           = 128,
    VolumeDown         = 129,
    KpComma            = 133,
    KpEqualsas400      = 134,
    International1     = 135,
    International2     = 136,
    International3     = 137,
    International4     = 138,
    International5     = 139,
    International6     = 140,
    International7     = 141,
    International8     = 142,
    International9     = 143,
    Lang1              = 144,
    Lang2              = 145,
    Lang3              = 146,
    Lang4              = 147,
    Lang5              = 148,
    Lang6              = 149,
    Lang7              = 150,
    Lang8              = 151,
    Lang9              = 152,
    AltErase           = 153,
    SysReq             = 154,
    Cancel             = 155,
    Clear              = 156,
    Prior              = 157,
    Return2            = 158,
    Separator          = 159,
    Out                = 160,
    Oper               = 161,
    ClearAgain         = 162,
    Crsel              = 163,
    Exsel              = 164,
    Kp00               = 176,
    Kp000              = 177,
    ThousandsSeparator = 178,
    DecimalSeparator   = 179,
    CurrencyUnit       = 180,
    CurrencySubunit    = 181,
    KpLeftParen        = 182,
    KpRightParen       = 183,
    KpLeftBrace        = 184,
    KpRightBrace       = 185,
    KpTab              = 186,
    KpBackspace        = 187,
    KpA                = 188,
    KpB                = 189,
    KpC                = 190,
    KpD                = 191,
    KpE                = 192,
    KpF                = 193,
    KpXor              = 194,
    KpPower            = 195,
    KpPercent          = 196,
    KpLess             = 197,
    KpGreater          = 198,
    KpAmpersand        = 199,
    KpDblampersand     = 200,
    KpVerticalbar      = 201,
    KpDblverticalbar   = 202,
    KpColon            = 203,
    KpHash             = 204,
    KpSpace            = 205,
    KpAt               = 206,
    KpExclam           = 207,
    KpMemstore         = 208,
    KpMemrecall        = 209,
    KpMemclear         = 210,
    KpMemadd           = 211,
    KpMemsubtract      = 212,
    KpMemmultiply      = 213,
    KpMemdivide        = 214,
    KpPlusminus        = 215,
    KpClear            = 216,
    KpClearentry       = 217,
    KpBinary           = 218,
    KpOctal            = 219,
    KpDecimal          = 220,
    KpHexadecimal      = 221,
    LCtrl              = 224,
    LShift             = 225,
    LAlt               = 226,
    LGui               = 227,
    RCtrl              = 228,
    RShift             = 229,
    RAlt               = 230,
    RGui               = 231,
    Mode               = 257,
    AudioNext          = 258,
    AudioPrev          = 259,
    AudioStop          = 260,
    AudioPlay          = 261,
    AudioMute          = 262,
    MediaSelect        = 263,
    Www                = 264,
    Mail               = 265,
    Calculator         = 266,
    Computer           = 267,
    AcSearch           = 268,
    AcHome             = 269,
    AcBack             = 270,
    AcForward          = 271,
    AcStop             = 272,
    AcRefresh          = 273,
    AcBookmarks        = 274,
    BrightnessDown     = 275,
    BrightnessUp       = 276,
    DisplaySwitch      = 277,
    KbdIllumToggle     = 278,
    KbdIllumDown       = 279,
    KbdIllumUp         = 280,
    Eject              = 281,
    Sleep              = 282,
    App1               = 283,
    App2               = 284,
    AudioRewind        = 285,
    AudioFastForward   = 286,
    SoftLeft           = 287,
    SoftRight          = 288,
    Call               = 289,
    EndCall            = 290,
    MaxValue           = 512
};
}
