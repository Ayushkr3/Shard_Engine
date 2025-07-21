#include "pch.h"
#include "EInputManager.h"
#include <stdlib.h>
#include <thread>
Keyboard InputManager::kb;
Mouse InputManager::Ms;
PRAWINPUT InputManager::data;
UINT InputManager::cbSize;
HANDLE InputManager::hMapFile;
LPVOID InputManager::ptrToMem;
void InputManager::Init() {
	kb.Init();
	Ms.Init();
	IPC(&Ms, &kb);
}
void InputManager::Read(LPARAM lparam) {
	GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &cbSize, sizeof(RAWINPUTHEADER));
	LPBYTE lpbyte = new BYTE[cbSize];

	GetRawInputData((HRAWINPUT)lparam,RID_INPUT,lpbyte,&cbSize, sizeof(RAWINPUTHEADER));

	RAWINPUT* raw = (RAWINPUT*)lpbyte;
	/*if (raw->header.dwType == RIM_TYPEKEYBOARD) {
		kb.Read(raw);
	}*/
	//kb.Read(raw);
	if (raw->header.dwType == RIM_TYPEMOUSE) {
		Ms.Read(raw);
	}
}
void InputManager::Update() {
	kb.Read();
}
char InputManager::GetLastKeyPress()
{
	return kb.keyPressed;
}
std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker>& InputManager::GetKeyboardState() {
	return kb.state;
}
void InputManager::IPC(Mouse* Ms, Keyboard* kb) {
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, sizeof(InputDevices), L"IP");
	ptrToMem = MapViewOfFile(hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(InputDevices));
	InputDevices* Data = new InputDevices;
	Data->Ms = Ms;
	Data->kb = kb;
	CopyMemory(ptrToMem, Data, sizeof(InputDevices));
}
void InputManager::DeInit() {
	if (ptrToMem != nullptr) {
		UnmapViewOfFile(ptrToMem);
	}
	if (hMapFile) {
		CloseHandle(hMapFile);
	}
}
void InputManager::ClientInit() {
	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, L"IP");
	if (hMapFile == nullptr)
		return;
	if (ptrToMem == nullptr) {
		ptrToMem = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(InputDevices));
	}
}
void InputManager::ClientDeInit() {
	if (ptrToMem != nullptr) {
		UnmapViewOfFile(ptrToMem);
	}
	
}
const InputManager::InputDevices* InputManager::GetInputDevices() {
	return (InputDevices*)ptrToMem;
}