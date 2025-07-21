#pragma once
#include <d3d11.h>
#include "NVPhysx/PxPhysicsAPI.h"
#include <wrl.h>
#include "SkyBox.h"
#include "Phys.h"
#include "Cam.h"
#include "Mesh.h"
#include "Global.h"
#include "ECore.h"
class Scene {
private:
	bool inPlayMode = false;
	std::vector<Objects*>::iterator LookUp(short id, std::vector<Objects*>& vec);
	std::vector<Prefab*>::iterator LookUp(Prefab* Tri, std::vector<Prefab*>& vec);
	std::unique_ptr<SkyBox> pSkyBox;
	void LoadSkyBox();
	Camera::DebugCamera dc;
	/*@first param-Internal
	/ @second param-Input
	/ @third param output*/
	CORE_EXP static void SceneThunk(void* TScene,void* pathForDeps,void* ptr);
	CORE_EXP void FixRecursivePropDeps(void* CallerStruct,void* ptr);
	//when object scene being loaded get the whole list
	//clear whole list after scene loaded
	static std::vector<Serialization::ObjectBlocks>* ObjectsBlocks;
	CORE_EXP Objects* AddObject(Objects* ob, short LocalId,bool isRenderable = true);
	CORE_EXP Objects* FixInheritenceOfObjWhileParsing(Serialization::ObjectBlocks& OB);
public:
	CORE_EXP void SaveScene();
	CORE_EXP void LoadScene();
	CORE_EXP Objects* AddObject(Objects* ob,bool isRenderable = true);
	CORE_EXP static short currentOBJID;
	CORE_EXP static std::vector<short> globalCurrentOBJID;
	CORE_EXP Scene(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext);
	CORE_EXP void Render();
	CORE_EXP void RenderWireFrame();
	CORE_EXP void PlayMode();
	CORE_EXP void InitalizePlayMode();
	CORE_EXP void DeInitalizePlayMode();
	CORE_EXP void DeleteObject(Objects* obj, bool isSmartPointer=false);
	CORE_EXP static short GetCurrentID();
	CORE_EXP Objects* ParseObject(Serialization::ObjectBlocks& OB);
public:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	ID3D11Device* pDumbDevice;
	ID3D11DeviceContext* pDumbContext;
	CORE_EXP ~Scene();
public:
	physx::PxScene* physScene;
	Camera cam;
	std::vector<Prefab*>Triangles;
	std::vector<Objects*>AllObject;
#ifdef WIN32_DEBUG
	std::vector<Objects*> DeleteAfterPlay;
#endif 
};