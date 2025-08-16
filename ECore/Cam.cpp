#include "pch.h"
#include "Cam.h"
#include "EInputManager.h"
#include "MACROS.h"
using namespace DirectX;
Camera::CameraProp::CameraProp(Objects* obj) :ObjectProperties(obj) {

}
Camera::Camera(Microsoft::WRL::ComPtr<ID3D11Device> pDevice) : Objects(++Objects::count, "Camera")//should be changed
{
	SerializationName = "Camera";
	PosNrot = new CameraProp(this);
	CamProperties.push_back(PosNrot);
	viewmatrix = XMMatrixLookAtLH(
		XMVectorSet(PosNrot->postion[0], PosNrot->postion[1], PosNrot->postion[2], 1.0f),
		XMVectorSet(focus[0], focus[1], focus[2], 1.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 1.7777777777f, 0.1f, 100.0f);
	viewXprojection.viewMat = CB::ConvertMatrixToFloat4x4(viewmatrix * projection);

	D3D11_BUFFER_DESC CbuffDesc;
	CbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CbuffDesc.ByteWidth = sizeof(CB::PerObjectData);
	CbuffDesc.Usage = D3D11_USAGE_DEFAULT;
	CbuffDesc.MiscFlags = 0;
	CbuffDesc.StructureByteStride = 0;
	CbuffDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA Csbr;
	Csbr.pSysMem = &viewXprojection;

	(pDevice->CreateBuffer(&CbuffDesc, &Csbr, &pFrameConstantBuffer));
}
void __cdecl Camera::calculateProjection(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext, CB::PerFrameData* ViewMatrix) {
	pContext->UpdateSubresource(pFrameConstantBuffer.Get(), 0, nullptr, ViewMatrix, 0, 0);
	pContext->GSSetConstantBuffers(1, 1, pFrameConstantBuffer.GetAddressOf());
}
XMVECTOR Camera::ProjectRayCast() {
	float xNDC = (2.0f * InputManager::GetInputDevices()->Ms->MouseX / 1066)-1.0f; //TODO:Change this
	float yNDC = (1.0f - 2.0f * InputManager::GetInputDevices()->Ms->MouseY / 600);//TODO: Change this
	XMVECTOR rayNDC = XMVectorSet(xNDC, yNDC, 1.0f, 1.0f);
	XMMATRIX invViewProj = XMMatrixInverse(nullptr,XMLoadFloat4x4(&viewXprojection.viewMat));
	XMVECTOR worldRayEnd = XMVector4Transform(rayNDC, XMMatrixTranspose(invViewProj));
	worldRayEnd /=XMVectorGetW(worldRayEnd);
	XMVECTOR rayOrigin = XMVectorSet(PosNrot->postion[0], PosNrot->postion[1], PosNrot->postion[2],0.0f);
	XMVECTOR rayDir = XMVector3Normalize(worldRayEnd - rayOrigin);
	return rayDir;
}
CB::PerFrameData __cdecl Camera::GetViewMatrix() {
	//////////////////////////
	if (PosNrot->rotation[1] > 360) {
		PosNrot->rotation[1] = 0;
	}
	if (PosNrot->rotation[1] < 0) {
		PosNrot->rotation[1] = 359;
	}
	PosNrot->rotation[0] = std::clamp(PosNrot->rotation[0], -89.0f, 89.0f);
	//////////////////////////
	XMVECTOR pos = XMVectorSet(PosNrot->postion[0], PosNrot->postion[1], PosNrot->postion[2], 1.0f);
	XMMATRIX Rotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(PosNrot->rotation[0]), XMConvertToRadians(PosNrot->rotation[1]), 0.0f);
	PosNrot->forwardDirection = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	PosNrot->forwardDirection = XMVector3TransformCoord(PosNrot->forwardDirection, Rotation);

	viewmatrix = XMMatrixTranspose(XMMatrixLookAtLH(
		pos,
		pos + PosNrot->forwardDirection,
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XMConvertToRadians(PosNrot->FoV), PosNrot->Ratio, 0.1f, 100.0f));

	viewXprojection.viewMat = CB::ConvertMatrixToFloat4x4(projection*viewmatrix);
	return viewXprojection;
}
void Camera::SetFOVnAspectRatio(float FOV, float ratio) {
	if (FOV >= 0)
		PosNrot->FoV = FOV;
	if (ratio != -1)
		PosNrot->Ratio = ratio;
}
void Camera::CameraProp::show() {
	ImGui::DragFloat2("Camera Rotation", rotation, 0.1f);
	ImGui::DragFloat3("Camera Position", postion, 0.1f);
	ImGui::DragFloat("Fov", &FoV, 1.0f, 30.f, 270);
	ImGui::Text(std::to_string(InputManager::GetInputDevices()->Ms->MouseX).c_str());
	ImGui::Text(std::to_string(InputManager::GetInputDevices()->Ms->MouseY).c_str());
}
ObjectProperties* Camera::DebugCamera::GetPropertyRef() {
	return this;
}
const std::type_info& Camera::DebugCamera::GetPropertyType() {
	return typeid(DebugCamera);
}
void Camera::DebugCamera::SetCameraProp(CameraProp* prop) {
	Iprop = prop;
}
std::string Camera::CameraProp::GetPropertyClassName() { return "CameraProp"; }
void Camera::DebugCamera::DebugCamMovement() {
	auto x = InputManager::GetInputDevices();
	if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive() && !ImGui::IsAnyItemFocused()) {
		if (InputManager::GetInputDevices()->Ms->isRightButtonHeld) {
			Iprop->rotation[0] += ((float)(InputManager::GetInputDevices()->Ms->deltaY)) / 2;
			Iprop->rotation[1] += ((float)(InputManager::GetInputDevices()->Ms->deltaX)) / 2;
		}
		/*
		if (Mouse::isLeftButtonHeld) {
			XMFLOAT3 rightDir, upDir;
			XMFLOAT3 forwardDir;
			XMStoreFloat3(&forwardDir, Iprop->forwardDirection);
			XMFLOAT3 worldUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
			XMStoreFloat3(&rightDir, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&forwardDir), XMLoadFloat3(&worldUp))));
			XMStoreFloat3(&upDir, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&rightDir), XMLoadFloat3(&forwardDir))));
			Iprop->postion[0] += ((float)(Mouse::deltaX)) * rightDir.x*0.025f;
			Iprop->postion[1] += ((float)(-Mouse::deltaX)) * rightDir.y*0.025f;
			Iprop->postion[2] += ((float)(Mouse::deltaX)) * rightDir.z*0.025f;
			Iprop->postion[0] += ((float)(Mouse::deltaY)) * upDir.x*0.025f;
			Iprop->postion[1] += ((float)(Mouse::deltaY)) * upDir.y*0.025f;
			Iprop->postion[2] += ((float)(Mouse::deltaY)) * upDir.z*0.025f;
		}
		if (Mouse::isMouseWheelScrolling) {
			XMFLOAT3 forwardDir;
			XMStoreFloat3(&forwardDir, Iprop->forwardDirection);
			Iprop->postion[0] += ((float)(Mouse::WheelDelta) / 120) *forwardDir.x*0.4f;
			Iprop->postion[1] += ((float)(Mouse::WheelDelta) / 120) *forwardDir.y*0.4f;
			Iprop->postion[2] += ((float)(Mouse::WheelDelta) / 120) *forwardDir.z*0.4f;
		}*/
	}
}
std::string Camera::CameraProp::Serialize() {
	return Serialization::SerializeProperty(*this);
}