#pragma once
#include <Windows.h>
#include "dxtkInc/Mouse.h"
#include "MACROS.h"
class Mouse {
public:
	bool isLeftButtonHeld = false;
	bool LeftButtonDown = false;
	bool LeftButtonUp = false;
	bool isRightButtonHeld = false;
	bool RightButtonDown = false;
	bool RightButtonUp = false;
	short deltaX = 0;
	short deltaY = 0;
	bool isMouseWheelScrolling = false;
	short WheelDelta = 0;
	short MouseX=0;
	short MouseY=0;
	RAWINPUTDEVICE rwM;
	INPUT_API void Init();
	INPUT_API void Read(RAWINPUT* raw);
	INPUT_API void ResetDelta();
};
