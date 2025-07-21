#pragma once
#include "Keyboard.h"
#include "Mouse.h"
#include "MACROS.h"
class InputManager {
public:
	struct InputDevices {
		Mouse* Ms;
		Keyboard* kb;
	};
private:
	static PRAWINPUT data;
	static UINT cbSize;
	static HANDLE hMapFile;
	static LPVOID ptrToMem;
	INPUT_API static void IPC(Mouse* Ms,Keyboard* kb);
public:
	//Dont use in IPC
	INPUT_API static Mouse Ms;
	//Dont use in IPC
	INPUT_API static Keyboard kb;
	INPUT_API static void Read(LPARAM lparam);
	INPUT_API static char GetLastKeyPress();
	INPUT_API static void Init();
	INPUT_API static void ClientInit();
	INPUT_API static void ClientDeInit();
	INPUT_API static const InputDevices* GetInputDevices();
	INPUT_API static void DeInit();
	INPUT_API static void Update();
	INPUT_API static std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker>& GetKeyboardState();
};