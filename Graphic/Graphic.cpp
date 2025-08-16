#include "Graphic.h"
#include "Factory.h"
#include <d3dcompiler.h>
#pragma comment(lib, "dxguid.lib")
//////////////////////
#include "EUI.h"
/////////////////////
#pragma comment(lib,"D3DCompiler.lib")
#pragma comment(lib,"d3d11.lib")

Graphic::Graphic(HWND hwnd){
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.Height = 0;
	scd.BufferDesc.Width = 0;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Flags = 0;
	scd.OutputWindow = hwnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = true;
	CHECK_ERROR(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, &scd, &pSwap, &pDevice, nullptr,&pContext));

	D3DFactory::Init(pDevice, pContext);
	
	ID3D11Resource* pBackBuffer = nullptr;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	pDevice->CreateRenderTargetView(pBackBuffer, nullptr, pTarget.GetAddressOf());
	pBackBuffer->Release();
	NVPhysx::Init();
	//UI::InitalizeUI(pDevice, pContext);
	pSc = new Scene(pDevice,pContext);

	DXGI_SWAP_CHAIN_DESC DESC;
	pSwap->GetDesc(&DESC);
	D3D11_VIEWPORT vp;
	vp.Width = DESC.BufferDesc.Width;
	vp.Height = DESC.BufferDesc.Height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);
#define DEPTH_BIAS_D32_FLOAT(d) (d/(1/pow(2,23)))
	D3D11_RASTERIZER_DESC rasDesc;
#ifdef WIREFRAME_ENABLED
	rasDesc.FillMode = D3D11_FILL_WIREFRAME;
#else
	rasDesc.FillMode = D3D11_FILL_SOLID;
#endif // WIREFRAME_ENABLED

	rasDesc.CullMode = D3D11_CULL_BACK;
	rasDesc.FrontCounterClockwise = true;
	rasDesc.DepthClipEnable = true;
	rasDesc.DepthBias = 0;
	rasDesc.SlopeScaledDepthBias = 0;
	rasDesc.DepthBiasClamp = 0;
	rasDesc.ScissorEnable = FALSE;
	rasDesc.MultisampleEnable = false;  //should be off
	rasDesc.AntialiasedLineEnable = true;
	CHECK_ERROR(pDevice->CreateRasterizerState(&rasDesc, &WireFrame));

	D3D11_RASTERIZER_DESC rasDescS;
	rasDescS.FillMode = D3D11_FILL_SOLID;
	rasDescS.CullMode = D3D11_CULL_BACK;
	rasDescS.FrontCounterClockwise = true;
	rasDescS.DepthClipEnable = true;
	rasDescS.DepthBias = 0;
	rasDescS.SlopeScaledDepthBias = 0;
	rasDescS.DepthBiasClamp = 0;
	rasDescS.ScissorEnable = FALSE;
	rasDescS.MultisampleEnable = false;
	rasDescS.AntialiasedLineEnable = false;
	CHECK_ERROR(pDevice->CreateRasterizerState(&rasDescS, &Solid));
	Microsoft::WRL::ComPtr<ID3D10Blob> pWireShad;
	CHECK_ERROR(D3DReadFileToBlob(L"SolidColWireFrame.cso", &pWireShad));
	pDevice->CreatePixelShader(pWireShad->GetBufferPointer(), pWireShad->GetBufferSize(), nullptr, &pWireFrameSolid);

	Microsoft::WRL::ComPtr<ID3D10Blob> pDSBlob;
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;

	CHECK_ERROR(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));
	pContext->OMSetDepthStencilState(pDSState.Get(),0);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
#ifdef ImGUI_ENABLED
	descDepth.Width = DESC.BufferDesc.Width;
	descDepth.Height = DESC.BufferDesc.Height;
#else
	descDepth.Width = 784;
	descDepth.Height = 561;
#endif // ImGUI_ENABLED
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL; 
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	CHECK_ERROR(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));


	D3D11_DEPTH_STENCIL_VIEW_DESC stencil_view_desc;
	stencil_view_desc.Flags = 0;
	stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
	stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	stencil_view_desc.Texture2D.MipSlice = 0u;

	CHECK_ERROR(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &stencil_view_desc, &pDs));
	pContext->OMSetRenderTargets(1, pTarget.GetAddressOf(),pDs.Get());
	pContext->RSSetState(WireFrame.Get());
	IPC::SetScene((void*)pSc);
	//UI::Resize(DESC.BufferDesc.Width, DESC.BufferDesc.Height,pContext);
}
Graphic::~Graphic(){
	NVPhysx::Destroy();
	//delete pSc;
	//if (Globals::isFullscreen) {
	//	ImGui_ImplDX11_Shutdown();
	//	ImGui_ImplWin32_Shutdown();
	//	ImGui::DestroyContext();
	//}
}
void Graphic::Resize(short sizeX,short sizeY) {
	pTarget.Reset();
	pDs.Reset();
	
	auto hr = DXGetErrorString(pSwap->ResizeBuffers(1, sizeX, sizeY, DXGI_FORMAT_UNKNOWN,0));
	ID3D11Resource* pBackBuffer = nullptr;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	pDevice->CreateRenderTargetView(pBackBuffer, nullptr, pTarget.GetAddressOf());
	pBackBuffer->Release();


	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
	CHECK_ERROR(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));
	pContext->OMSetDepthStencilState(pDSState.Get(), 0);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = sizeX;
	descDepth.Height = sizeY;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);


	D3D11_DEPTH_STENCIL_VIEW_DESC stencil_view_desc;
	stencil_view_desc.Flags = 0;
	stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
	stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	stencil_view_desc.Texture2D.MipSlice = 0u;

	CHECK_ERROR(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &stencil_view_desc, &pDs));
	pContext->OMSetRenderTargets(1,pTarget.GetAddressOf(), pDs.Get());
	D3D11_VIEWPORT vp;
	vp.Width = descDepth.Width;
	vp.Height = descDepth.Height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);
	//UI::Resize(sizeX, sizeY,pContext);
}
void Graphic::EndFrame() {
	if (Globals::isFullscreen) {
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	pSwap->Present(1u,0);
}
void Graphic::ClearBuffer(float rgba[4]) {
	pContext->ClearRenderTargetView(pTarget.Get(),rgba);
	pContext->ClearDepthStencilView(pDs.Get(), D3D11_CLEAR_DEPTH , 1.0f, 0);
	if (Globals::isFullscreen) {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
}
void Graphic::TestFrames() {
	pContext->PSGetShader(&pLastShader,nullptr,0);
	pSc->Render();
#ifdef WIREFRAME_ENABLED
	pContext->PSSetShader(pWireFrameSolid.Get(), nullptr, 0);
	pContext->RSSetState(Solid.Get());
	pSc->RenderWireFrame();
#endif 
	if (*Globals::inPlayMode) {
		pSc->PlayMode();
	}
	//UI::RenderUI(pDevice,pContext);
	pContext->PSSetShader(pLastShader.Get(), nullptr, 0);  //Remove this maybe
}
Scene* Graphic::GetCurrentScene() {
	
	return pSc;
}