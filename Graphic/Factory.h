#pragma once
#include "Phys.h"
#include "Primitives.h"
#include "Global.h"
#include <wrl.h>
#include <d3d11.h>
class ObjectPropertiesFactory {
public:
	static HMODULE Userlib;
	static void Init();
	static void DeInit();
	static void InitModule();
};
class D3DFactory {
public:
	static void Init(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext);
	static void DeInit();
};
class ImguiContextFactory {
public:
	static void Init(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext,ImGuiContext* ctx);
};
