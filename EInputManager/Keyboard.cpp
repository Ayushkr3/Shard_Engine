#include "pch.h"
#include "Keyboard.h"
char Keyboard::keyPressed =0;
std::unique_ptr<DirectX::Keyboard> Keyboard::dxKeyboard;
std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker> Keyboard::state;
void Keyboard::Init() {
	dxKeyboard = std::make_unique<DirectX::Keyboard>();
	state = std::make_unique<DirectX::Keyboard::KeyboardStateTracker>();
	rwK.dwFlags = 0;
	rwK.hwndTarget = 0;
	rwK.usUsage = 0x06;
	rwK.usUsagePage = 0x01;
	RegisterRawInputDevices(&rwK, 1, sizeof(rwK));
}
void Keyboard::ForwardMessage(UINT message,LPARAM lparam,WPARAM wparam) {
	dxKeyboard->ProcessMessage(message, wparam, lparam);
}
void Keyboard::Read() {
	/*if (raw->data.keyboard.Flags == RI_KEY_MAKE) {
		keyPressed = (char)raw->data.keyboard.VKey;
		USHORT check = raw->data.keyboard.Message;
	}
	else {
		keyPressed = 0;
	}*/
	auto rState = dxKeyboard->GetState();
	state->Update(rState);
	return;
}