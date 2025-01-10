#include <Keyboard/Structs.hpp>

namespace TabGraph::Keyboard {
State GetState();
bool GetKeyState(const ScanCode& a_Scancode);
Modifiers GetModifiers();
Keycode GetKeycode(const ScanCode& a_Scancode);
ScanCode GetScancode(const Keycode& a_Keycode);
}
