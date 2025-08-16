#include "pch.h"
#include "ImGui/imgui.h"
#include "Mesh.h"
using namespace CB;
using namespace DirectX;
Prefab::Prefab(Mesh& mesh,short ObjectId):Objects(mesh),id(ObjectId) {
	SerializationName = "Prefab";
	HRESULT hr;
	pDevice = mesh.pDevice;
	pContext = mesh.pContext;
	vertices = mesh.vertices;
	index = mesh.index;
	color[0] = 1.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	Trans = new TransformStruct(this);
	//Mat = mesh.Mat;
	objectPath = mesh.objectPath;
	ObjProperties = mesh.ObjProperties;
	ObjProperties[0] = (Trans);
	Transformation = mesh.Transformation;
	
	D3D11_BUFFER_DESC IndexBufferDesc;
	D3D11_BUFFER_DESC VertexBuffer;
	VertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBuffer.Usage = D3D11_USAGE_DYNAMIC;
	VertexBuffer.ByteWidth = (vertices.size() * sizeof(Vertex));
	VertexBuffer.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	VertexBuffer.StructureByteStride = sizeof(Vertex);
	VertexBuffer.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sbr;
	sbr.pSysMem = vertices.data();

	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IndexBufferDesc.CPUAccessFlags = 0u;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.ByteWidth = (index.size() * sizeof(unsigned int));
	IndexBufferDesc.StructureByteStride = sizeof(unsigned int);
	D3D11_SUBRESOURCE_DATA index_subr;
	index_subr.pSysMem = index.data();
	UINT strides = sizeof(Vertex);
	UINT offset = 0u;
	hr = (pDevice->CreateBuffer(&VertexBuffer, &sbr, &pVertexBuffer));
	hr = (pDevice->CreateBuffer(&IndexBufferDesc, &index_subr, &pIndexBuffer));
	pContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &strides, &offset);
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	Transformation.Rotation = ConvertMatrixToFloat4x4(XMMatrixRotationRollPitchYaw(XMConvertToRadians(Trans->rotation[0]), XMConvertToRadians(Trans->rotation[1]), XMConvertToRadians(Trans->rotation[2])));
	Transformation.Translation = ConvertMatrixToFloat4x4(XMMatrixTranslation(Trans->position[0] + (Inheritence.InheritedTrans)->position[0], Trans->position[1] + (Inheritence.InheritedTrans)->position[1], Trans->position[2] + (Inheritence.InheritedTrans)->position[2]));
	Transformation.Scale = ConvertMatrixToFloat4x4(XMMatrixScaling(Trans->Scale[0], Trans->Scale[1], Trans->Scale[2]));
	dynamic_cast<Primitives::Material*>(ObjProperties[1])->ds.BindAssociatedBuffer((void*)&Transformation, 0, pContext.Get());
	///////////////////////////////////////////////////////////

	Microsoft::WRL::ComPtr<ID3D11InputLayout>pIL;
	D3D11_INPUT_ELEMENT_DESC IL[] = {
		{ "Position",0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Texcoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0, DXGI_FORMAT_R32G32B32_FLOAT,0,20u,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	hr = pDevice->CreateInputLayout(IL, std::size(IL), (dynamic_cast<Primitives::Material*>(ObjProperties[1])->vs.GetBlob())->GetBufferPointer(), (dynamic_cast<Primitives::Material*>(ObjProperties[1])->vs.GetBlob())->GetBufferSize(), &pIL);
	pContext->IASetInputLayout(pIL.Get());
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
}
Prefab::Prefab(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext, std::vector<Vertex> vertice, std::vector<unsigned int>indi, short id, float rgba[3], short globalID, std::vector<NormalPerObject>nor) :
	pContext(pContext), pDevice(pDevice), vertices(
		vertice
	), index(indi
	), Objects(globalID, "Object"), id(id), color{ rgba[0],rgba[1],rgba[2] }, n{ nor }, last_color{ rgba[0],rgba[1],rgba[2] }, rn{ nor }
{
	IPC::GetInstance();
	SerializationName = "Prefab";
	Objects* obj = dynamic_cast<Objects*>(this);
	Trans = new TransformStruct(obj);
	Primitives::Material* Mat = new Primitives::Material(obj);
	ObjProperties.push_back(Trans);
	ObjProperties.push_back(Mat);

	D3D11_BUFFER_DESC IndexBufferDesc;
	D3D11_BUFFER_DESC VertexBuffer;
	VertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBuffer.Usage = D3D11_USAGE_DYNAMIC;
	VertexBuffer.ByteWidth = (vertices.size() * sizeof(Vertex));
	VertexBuffer.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	VertexBuffer.StructureByteStride = sizeof(Vertex);
	VertexBuffer.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sbr;
	sbr.pSysMem = vertices.data();

	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IndexBufferDesc.CPUAccessFlags = 0u;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.ByteWidth = (index.size() * sizeof(unsigned int));
	IndexBufferDesc.StructureByteStride = sizeof(unsigned int);
	D3D11_SUBRESOURCE_DATA index_subr;
	index_subr.pSysMem = index.data();
	UINT strides = sizeof(Vertex);
	UINT offset = 0u;
	pContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &strides, &offset);
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	Transformation.Rotation = ConvertMatrixToFloat4x4(XMMatrixRotationRollPitchYaw(XMConvertToRadians(Trans->rotation[0]), XMConvertToRadians(Trans->rotation[1]), XMConvertToRadians(Trans->rotation[2])));
	Transformation.Translation = ConvertMatrixToFloat4x4(XMMatrixTranslation(Trans->position[0] + (Inheritence.InheritedTrans)->position[0], Trans->position[1] + (Inheritence.InheritedTrans)->position[1], Trans->position[2] + (Inheritence.InheritedTrans)->position[2]));
	Transformation.Scale = ConvertMatrixToFloat4x4(XMMatrixScaling(Trans->Scale[0], Trans->Scale[1], Trans->Scale[2]));
	Mat->CreateCBuffer(0, sizeof(PerObjectData), Primitives::DOMAIN_SHADER);
	dynamic_cast<Primitives::Material*>(ObjProperties[1])->ds.BindAssociatedBuffer((void*)&Transformation, 0, pContext.Get());
	//pContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &strides, &offset);

	(pDevice->CreateBuffer(&VertexBuffer, &sbr, &pVertexBuffer));
	(pDevice->CreateBuffer(&IndexBufferDesc, &index_subr, &pIndexBuffer));
	///////////////////////////////////////////////////////////

	Microsoft::WRL::ComPtr<ID3D11InputLayout>pIL;
	D3D11_INPUT_ELEMENT_DESC IL[] = {
		{ "Position",0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Texcoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0, DXGI_FORMAT_R32G32B32_FLOAT,0,20u,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	pDevice->CreateInputLayout(IL, std::size(IL), (Mat->vs.GetBlob())->GetBufferPointer(), (Mat->vs.GetBlob())->GetBufferSize(), &pIL);
	pContext->IASetInputLayout(pIL.Get());
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
}
void Prefab::Draw() {
	dynamic_cast<Primitives::Material*>(ObjProperties[1])->BindAllShaders();
	pContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &strides, &offset);
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//Have to bind other wise it will not be rendered ,before every draw call
	//pCB->BindToVSshader(pContext);
	pContext->DrawIndexed(index.size(), 0, 0);
	//pContext->DrawIndexed(vertices.size(), 0);
}
void Prefab::UpdateBuffers() {
	if (Moving()) {
		Inheritence.AbsoluteTrans->position[0] = Trans->position[0] + (Inheritence.InheritedTrans)->position[0];
		Inheritence.AbsoluteTrans->position[1] = Trans->position[1] + (Inheritence.InheritedTrans)->position[1];
		Inheritence.AbsoluteTrans->position[2] = Trans->position[2] + (Inheritence.InheritedTrans)->position[2];
		Inheritence.AbsoluteTrans->rotation[0] = Trans->rotation[0] + (Inheritence.InheritedTrans)->rotation[0];
		Inheritence.AbsoluteTrans->rotation[1] = Trans->rotation[1] + (Inheritence.InheritedTrans)->rotation[1];
		Inheritence.AbsoluteTrans->rotation[2] = Trans->rotation[2] + (Inheritence.InheritedTrans)->rotation[2];
		Transformation.Rotation = ConvertMatrixToFloat4x4(XMMatrixRotationQuaternion(XMVectorSet(Trans->rotation[0], Trans->rotation[1], Trans->rotation[2], Trans->rotation[3])));
		Transformation.Translation = ConvertMatrixToFloat4x4(XMMatrixTranslation(Inheritence.AbsoluteTrans->position[0], Inheritence.AbsoluteTrans->position[1], Inheritence.AbsoluteTrans->position[2]));
		Transformation.Scale = ConvertMatrixToFloat4x4(XMMatrixScaling(Trans->Scale[0], Trans->Scale[1], Trans->Scale[2]));

		dynamic_cast<Primitives::Material*>(ObjProperties[1])->ds.BindAssociatedBuffer((void*)&Transformation, 0, pContext.Get());
	}
}
void Prefab::UpdateSubResource() {
	dynamic_cast<Primitives::Material*>(ObjProperties[1])->ds.BindAssociatedBuffer((void*)&Transformation, 0, pContext.Get());
}
void Prefab::inPlayMode() {
	Trans->Update();
	for (auto& o : ObjProperties) {
		o->inPlayMode();
	}
}
void Prefab::ReCalculatePosition() {
	//pCB->Transform(Trans, rn);
}
void Prefab::InitializePlayMode()
{
	for (auto& o : ObjProperties) {
		o->InitPlayMode();
	}
	//pCB->Transform(Trans, rn);
}
void Prefab::Highlight() {
	color[0] = 0.0f;
	color[1] = 1.0f;
}
void Prefab::Restore()
{
	color[0] = last_color[0];
	color[1] = last_color[1];
}
std::vector<Vertex> Prefab::GetVertices() {
	return vertices;
}
void Prefab::DeInitializePlayMode()
{
	for (auto& o : ObjProperties) {
		o->DeInitPlayMode();
	}
}
bool Prefab::operator<(const Prefab& secondObj) const {
	return (id < secondObj.id) ? id : secondObj.id;
}
bool Prefab::operator==(const Prefab& secondObj) const {
	return (id == secondObj.id) ? true : false;
}
std::vector<ObjectProperties*>* Prefab::GetProperties() {
	return &ObjProperties;
}
Prefab::~Prefab() {
	//delete Mat;
	auto vec = *GetProperties();
	for (auto& it : vec) {
		delete it;
		it = nullptr;
	}
}

std::vector<ObjectProperties*>* Mesh::GetProperties() {
	return &ObjProperties;
}
Mesh::Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext,short id, std::vector<Vertex> vertice, std::vector<unsigned int> indi,PathToFile* ptf):Objects(id,"Object"),pDevice(pDevice),pContext(pContext) {
	SerializationName = "Mesh";
	Objects* obj = dynamic_cast<Objects*>(this);
	Primitives::Material* Mat = new Primitives::Material(obj);
	ObjProperties.push_back(Trans);
	ObjProperties.push_back(Mat);
	vertices = vertice;
	index = indi;
	objectPath = ptf;
	//Transformation.Rotation = ConvertMatrixToFloat4x4(XMMatrixRotationRollPitchYaw(XMConvertToRadians(Trans->rotation[0]), XMConvertToRadians(Trans->rotation[1]), XMConvertToRadians(Trans->rotation[2])));
	//Transformation.Translation = ConvertMatrixToFloat4x4(XMMatrixTranslation(Trans->position[0] + (Inheritence.InheritedTrans)->position[0], Trans->position[1] + (Inheritence.InheritedTrans)->position[1], Trans->position[2] + (Inheritence.InheritedTrans)->position[2]));
	//Transformation.Scale = ConvertMatrixToFloat4x4(XMMatrixScaling(Trans->Scale[0], Trans->Scale[1], Trans->Scale[2]));

	Mat->CreateCBuffer(0, sizeof(PerObjectData), Primitives::DOMAIN_SHADER);
	///////////////////////////////////////////////////////////
}
std::string Prefab::Serialize() {
	std::string s;
	s += "0:"+objectPath->Path+"\n";
	s += "1:" + objectPath->FileName + "\n";
	return s;
}
NullObject::NullObject(short id):Objects(id,"Object"){
	SerializationName = "NObject";
	t = new TransformStruct(this);
	objProp.push_back(t);
}
std::vector<ObjectProperties*>* NullObject::GetProperties() {
	return &objProp;
}
NullObject::~NullObject() {
	auto vec = *(GetProperties());
	for (auto& it : vec) {
		delete it;
		it = nullptr;
	}
}
bool Prefab::Moving() {
	return ((Inheritence.AbsoluteTrans->position[0]-(Trans->position[0] + (Inheritence.InheritedTrans)->position[0])!=0)|
		(Inheritence.AbsoluteTrans->position[1]-(Trans->position[1] + (Inheritence.InheritedTrans)->position[1]) !=0)|
		(Inheritence.AbsoluteTrans->position[2]-(Trans->position[2] + (Inheritence.InheritedTrans)->position[2] )!=0)|
		(Inheritence.AbsoluteTrans->rotation[0]-(Trans->rotation[0] + (Inheritence.InheritedTrans)->rotation[0] )!=0)|
		(Inheritence.AbsoluteTrans->rotation[1]-(Trans->rotation[1] + (Inheritence.InheritedTrans)->rotation[1] )!=0)|
		(Inheritence.AbsoluteTrans->rotation[2]-(Trans->rotation[2] + (Inheritence.InheritedTrans)->rotation[2])!=0)|
		(Inheritence.AbsoluteTrans->Scale[0] - (Trans->Scale[0] + (Inheritence.InheritedTrans)->Scale[0]) != 0) |
		(Inheritence.AbsoluteTrans->Scale[1] - (Trans->Scale[1] + (Inheritence.InheritedTrans)->Scale[1]) != 0) |
		(Inheritence.AbsoluteTrans->Scale[2] - (Trans->Scale[2] + (Inheritence.InheritedTrans)->Scale[2]) != 0));
}
void NullObject::inPlayMode() {
	Inheritence.AbsoluteTrans->position[0] = t->position[0] + (Inheritence.InheritedTrans)->position[0];
	Inheritence.AbsoluteTrans->position[1] = t->position[1] + (Inheritence.InheritedTrans)->position[1];
	Inheritence.AbsoluteTrans->position[2] = t->position[2] + (Inheritence.InheritedTrans)->position[2];
	for (auto& o : objProp) {
		o->inPlayMode();
	}
}
void NullObject::InitializePlayMode() {
	for (auto& o : objProp) {
		o->InitPlayMode();
	}
}
void NullObject::DeInitializePlayMode() {
	for (auto& o : objProp) {
		o->DeInitPlayMode();
	}
}