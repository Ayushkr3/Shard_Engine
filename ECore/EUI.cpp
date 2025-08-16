#include "pch.h"
#include "EUI.h"
#include <d3dcompiler.h>
#pragma comment(lib,"D3DCompiler.lib")
using namespace UI;
Microsoft::WRL::ComPtr<ID3D10Blob> BaseUI::pVSBlob;
Microsoft::WRL::ComPtr<ID3D10Blob> BaseUI::pPSBlob;
Microsoft::WRL::ComPtr<ID3D11InputLayout> BaseUI::pIL;
Microsoft::WRL::ComPtr<ID3D11VertexShader>BaseUI::pVS;
Microsoft::WRL::ComPtr<ID3D11PixelShader>BaseUI::pPS;
Microsoft::WRL::ComPtr<ID3D11Buffer> BaseUI::VertexBuff;
Microsoft::WRL::ComPtr<ID3D11Buffer> BaseUI::indexBuff;
Microsoft::WRL::ComPtr<ID3D11DepthStencilState> BaseUI::pDSState;
Microsoft::WRL::ComPtr<ID3D11Buffer> BaseUI::orthoBuffer;
Microsoft::WRL::ComPtr<ID3D11Buffer> BaseUI::UIInstance;
Microsoft::WRL::ComPtr<ID3D11Device> BaseUI::pDevice;
Microsoft::WRL::ComPtr<ID3D11DeviceContext> BaseUI::pContext;
std::unordered_map<int, UI::InstanceData*>BaseUI::AllInstanceData;
DirectX::XMMATRIX BaseUI::ortho;
short BaseUI::Count = 0;
short BaseUI::sizeX;
short BaseUI::sizeY;
std::queue<short> BaseUI::availableIndexs;
std::vector<UI::InstanceData> BaseUI::privateDat;

BaseUI::BaseUI(Objects* obj):Container(obj) {

}
BaseUI::~BaseUI() {
	availableIndexs.push(InterID);
}
void BaseUI::Action() {

}
std::vector<ObjectProperties*>* Button::GetProperties() {
	return &properties;
}
short BaseUI::GetNewIndex() {
	if (!availableIndexs.empty()) {
		short id =  availableIndexs.front();
		availableIndexs.pop();
		return id;
	}
	BaseUI::Count++;
	return BaseUI::Count-1;
}
void BaseUI::ResetInstanceBuffer(){
	UIInstance.Reset();
	D3D11_BUFFER_DESC InstanceBuff;
	InstanceBuff.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InstanceBuff.Usage = D3D11_USAGE_DYNAMIC;
	InstanceBuff.ByteWidth = (sizeof(UI::InstanceData) * BaseUI::Count);
	InstanceBuff.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	InstanceBuff.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sbr;
	sbr.pSysMem = privateDat.data();
	HRESULT hr = (pDevice)->CreateBuffer(&InstanceBuff,&sbr, &UIInstance); //All element buffer
}

Button::Button(short id):Objects(id,"Button"),BaseUI(this) {
	properties.push_back(&Container);
	BaseUI::AllInstanceData[Count] = &this->Container.InsDat;
	BaseUI::privateDat.push_back(this->Container.InsDat);
	InterID = GetNewIndex();
	BaseUI::ResetInstanceBuffer();
}
void UI::InitalizeUI(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext) {
	BaseUI::pDevice = pDevice;
	BaseUI::pContext = pContext;
	HRESULT hr;
	D3D11_BUFFER_DESC IndexBufferDesc;
	D3D11_BUFFER_DESC VertexBufferDesc;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	VertexBufferDesc.ByteWidth = (BaseUI::enclosingQuad.size() * sizeof(UIVertex));
	VertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	VertexBufferDesc.StructureByteStride = sizeof(UIVertex);
	VertexBufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sbr;
	sbr.pSysMem = BaseUI::enclosingQuad.data();

	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IndexBufferDesc.CPUAccessFlags = 0u;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.ByteWidth = (BaseUI::indices.size() * sizeof(unsigned int));
	IndexBufferDesc.StructureByteStride = sizeof(unsigned int);
	D3D11_SUBRESOURCE_DATA index_subr;
	index_subr.pSysMem = BaseUI::indices.data();
	hr = (pDevice->CreateBuffer(&VertexBufferDesc, &sbr, &BaseUI::VertexBuff));
	hr = (pDevice->CreateBuffer(&IndexBufferDesc, &index_subr, &BaseUI::indexBuff));

	//------------------------------Shaders-------------------------------------------//
	D3DReadFileToBlob(L"G:\\shard\\x64\\Debug\\UIVs.cso",&BaseUI::pVSBlob);
	D3DReadFileToBlob(L"G:\\shard\\x64\\Debug\\UIPs.cso", &BaseUI::pPSBlob);
	hr = pDevice->CreateVertexShader(BaseUI::pVSBlob->GetBufferPointer(), BaseUI::pVSBlob->GetBufferSize(),nullptr,&BaseUI::pVS);
	hr = pDevice->CreatePixelShader(BaseUI::pPSBlob->GetBufferPointer(), BaseUI::pPSBlob->GetBufferSize(), nullptr, &BaseUI::pPS);
	//-------------------------------------------------------------------------------//
	//------------------------------Constant Buffer----------------------------------//
	float aspect = static_cast<float>(BaseUI::sizeX) / BaseUI::sizeY;
	BaseUI::ortho = DirectX::XMMatrixOrthographicLH(2 * aspect, 2.0f, 0.0f, 1.0f);
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.ByteWidth = (sizeof(BaseUI::ortho));
	cb.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	cb.StructureByteStride = (sizeof(BaseUI::ortho));
	cb.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA c_subr;
	c_subr.pSysMem = &BaseUI::ortho;
	pDevice->CreateBuffer(&cb,&c_subr,&BaseUI::orthoBuffer);
	//-------------------------------------------------------------------------------//
	D3D11_INPUT_ELEMENT_DESC IL[] = {
		{ "Position",0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Texcoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "XY",0,DXGI_FORMAT_R32G32_FLOAT,1,0u,D3D11_INPUT_PER_INSTANCE_DATA,1 },//last is instance step rate
		{ "SXY",0,DXGI_FORMAT_R32G32_FLOAT,1,8u,D3D11_INPUT_PER_INSTANCE_DATA,1 },
		//{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 0, 20u, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = pDevice->CreateInputLayout(IL, std::size(IL),BaseUI::pVSBlob->GetBufferPointer(),BaseUI::pVSBlob->GetBufferSize(),&BaseUI::pIL);

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	
	pDevice->CreateDepthStencilState(&dsDesc, &BaseUI::pDSState);
}
void UI::RenderUI(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext) {
	if (BaseUI::Count > 0) {
		UINT strides[2] = { sizeof(UIVertex), sizeof(UI::InstanceData) };
		UINT offsets[2] = { 0, 0 };
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pBIL;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pOldState;
		D3D11_PRIMITIVE_TOPOLOGY topo;
		pContext->OMGetDepthStencilState(&pOldState,nullptr);
		pContext->OMSetDepthStencilState(BaseUI::pDSState.Get(),0);
		pContext->IAGetInputLayout(&pBIL); //Get Last Layout
		pContext->IAGetPrimitiveTopology(&topo); //Get Last topo
		ID3D11Buffer* buffers[2] = { BaseUI::VertexBuff.Get(), BaseUI::UIInstance.Get() };
		pContext->IASetVertexBuffers(0, 2, buffers,strides,offsets);
		pContext->VSSetShader(BaseUI::pVS.Get(), nullptr, 0);
		pContext->VSSetConstantBuffers(0, 1, BaseUI::orthoBuffer.GetAddressOf());
		pContext->PSSetShader(BaseUI::pPS.Get(), nullptr, 0);
		pContext->HSSetShader(nullptr, 0, 0);
		pContext->GSSetShader(nullptr, 0, 0);
		pContext->DSSetShader(nullptr, 0, 0);
		pContext->IASetInputLayout(BaseUI::pIL.Get());
		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pContext->DrawInstanced(BaseUI::enclosingQuad.size(), BaseUI::Count, 0, 0);
		pContext->IASetInputLayout(pBIL.Get()); //Set last IL
		pContext->IASetPrimitiveTopology(topo); //Set last topo
		pContext->OMSetDepthStencilState(pOldState.Get(), 0); //set last state
	}
}
void UI::Resize(short sizeX, short sizeY, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext) {
	BaseUI::sizeX = sizeX;
	BaseUI::sizeY = sizeY;
	float aspect = static_cast<float>(BaseUI::sizeX) / BaseUI::sizeY;
	BaseUI::ortho = DirectX::XMMatrixOrthographicLH(2 * aspect, 2.0f, 0.0f, 1.0f);
	//update buffer;
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hr = pContext->Map(BaseUI::orthoBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &BaseUI::ortho, sizeof(BaseUI::ortho));
	pContext->Unmap(BaseUI::orthoBuffer.Get(), 0);
}

//Way to fragile
//Kill me
void BaseUI::UpdatePortion(short ID,void* DataToReplaceWith) {
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hr = pContext->Map(BaseUI::UIInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapped);
	//Based on ID update the section inside the vector
	UI::InstanceData* ptr = (UI::InstanceData*)mapped.pData;
	ptr = ptr + ID;
	memcpy(ptr, DataToReplaceWith, sizeof(UIContainer::InsDat));
	pContext->Unmap(BaseUI::UIInstance.Get(), 0);
}
void Button::Update() {
	//check for input blah blah
	if (Container.isContainerMoving()) {
		UpdatePortion(InterID,&Container.InsDat); //Inside All element Buffer just update this section
	}
}