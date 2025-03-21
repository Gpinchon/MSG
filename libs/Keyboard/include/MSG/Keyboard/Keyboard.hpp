#include <MSG/Keyboard/Structs.hpp>

namespace MSG::Keyboard {
State GetState();
bool GetKeyState(const ScanCode& a_Scancode);
Modifiers GetModifiers();
KeyCode GetKeycode(const ScanCode& a_Scancode);
ScanCode GetScancode(const KeyCode& a_Keycode);
}
