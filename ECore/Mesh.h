#pragma once
#include <vector>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <iterator>
#include <wrl.h>
#include "errors.h"
#include "Structs.h"
#include "EInputManager.h"
#include "Primitives.h"
#include "ECore.h"
class NullObject:public Objects{
public:
	TransformStruct* t;
	CORE_EXP NullObject(short id);
	std::vector<ObjectProperties*> objProp;
	CORE_EXP std::vector<ObjectProperties*>* GetProperties();
	CORE_EXP ~NullObject();
	CORE_EXP void inPlayMode();
	CORE_EXP void InitializePlayMode();
	CORE_EXP void DeInitializePlayMode();
};
class Renderable {
public:
	CORE_EXP virtual void Draw() {};
	CORE_EXP virtual void UpdateBuffers() {};
};
class Mesh :public Objects,public Renderable {
public:
	PathToFile* objectPath;
	std::vector<ObjectProperties*> ObjProperties;
	CB::PerObjectData Transformation;
	std::vector<Vertex> vertices; // = { {0.0f,0.0f, 255, 0, 0, 0 }, { 0.5f,-0.5f,0,255,0,0 }, { -0.5f,-0.5f,0,0,255,0 } };
	std::vector<unsigned int> index;
	UINT strides = sizeof(Vertex);
	UINT offset = 0u;
	//Primitives::Material* Mat;
	TransformStruct* Trans;
	Microsoft::WRL::ComPtr<ID3D11Device>& pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext;
	CORE_EXP Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext,short id,std::vector<Vertex> vertice, std::vector<unsigned int> indi,PathToFile* ptf);
	CORE_EXP std::vector<ObjectProperties*>* GetProperties();
};
class Prefab :public Objects, public Renderable
{
private:
	bool Moving();
public:
	short id;
	std::vector<Vertex> vertices; // = { {0.0f,0.0f, 255, 0, 0, 0 }, { 0.5f,-0.5f,0,255,0,0 }, { -0.5f,-0.5f,0,0,255,0 } };
	std::vector<unsigned int> index;
	CORE_EXP Prefab(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext, std::vector<Vertex> vertice, std::vector<unsigned int> indi, short id, float rgba[3], short globalID, std::vector<NormalPerObject>nor = {});
	CORE_EXP Prefab(Mesh& mesh, short ObjectId);
	CORE_EXP ~Prefab();
	bool isMoving;
	CORE_EXP void Draw();
	CORE_EXP void UpdateBuffers();
	CORE_EXP void UpdateSubResource();
	CORE_EXP void inPlayMode();
	CB::PerObjectData Transformation;
	CORE_EXP bool operator<(const Prefab& secondObj)const;
	CORE_EXP bool operator==(const Prefab& secondObj)const;
	CORE_EXP std::vector<Vertex> GetVertices();
	CORE_EXP void DeInitializePlayMode();
protected:
	UINT strides = sizeof(Vertex);
	UINT offset = 0u;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	//TODO: Move Shader to SCENE class
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	std::vector<NormalPerObject> n;
public:
	TransformStruct* Trans;
	std::vector<NormalPerObject> rn; //Transformed normals
	std::vector<ObjectProperties*> ObjProperties;
	//Primitives::Material* Mat;
public:
	PathToFile* objectPath;
	CORE_EXP void ReCalculatePosition();
	CORE_EXP void InitializePlayMode();
	CORE_EXP void Highlight();
	CORE_EXP void Restore();
	CORE_EXP std::string Serialize();
	float color[3];
	float last_color[3];
public:
	//
	//
	CORE_EXP std::vector<ObjectProperties*>* GetProperties();
};


