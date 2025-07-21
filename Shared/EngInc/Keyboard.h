#pragma once
#include <Windows.h>
#include "MACROS.h"
#include "dxtkInc/Keyboard.h"

class Keyboard{
private:
	UINT cbSize = 16;
public:
	static std::unique_ptr<DirectX::Keyboard> dxKeyboard;
	static std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker> state;
	static INPUT_API char keyPressed;
	static INPUT_API void ForwardMessage(UINT message, LPARAM lparam, WPARAM wparam);
	
	PRAWINPUT data;
	RAWINPUTDEVICE rwK;
	INPUT_API void Init();
	INPUT_API void Read();
};

