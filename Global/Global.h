#pragma once
#include <vector>
#include <array>
#include <string>
#include "ImGui/imgui.h"
#include <functional>
#include <map>
#include <any>
#include <sstream>
#include <Windows.h>
#include <string>
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>
#include <iomanip>
#pragma warning(disable : 4244)
static std::string WorkingDirectory = "D:\\program\\Eng";
struct ObjectProperties;
struct TransformStruct;
inline ImGuiContext* Globalctx = nullptr;

#define RECURSIVE_SERIALIZATION_CALL(functor,pp_to_ref)void(*func)(void*, void*, void*) = (void(*)(void*, void*, void*))(functor);func(Scene, (void*)&Serialization::CallerObject(associatedObj->Id, (void*)&enclosed), pp_to_ref);

#define REFLECT_BEGIN_ADDR(CLASS_NAME) \
    auto reflect_addr() { \
        using ThisClass = CLASS_NAME; \
		int i = 0;\
        return std::tuple(
#define REFLECT_ADDR(VAR) \
    std::make_pair(i++,(void*)&VAR)

#define REFLECT_END_ADDR() \
        ); \
    }
class Objects {
public:
	bool operator<(const Objects& other)const {
		return Id < other.Id;
	}
	bool operator>(const Objects& other)const {
		return Id > other.Id;
	}
	bool operator==(const Objects& other)const {
		return Id == other.Id;
	}
	bool operator==(const int id)const {
		return Id == id;
	}
	virtual void Highlight() {}; // If item is selected then do stuff
	virtual void Restore() {}; //If item is unselected restore previous state
	inline static std::vector<short> GlobalIdPool;
	inline static int count = 0;
	struct Inherits {
		Objects* inheritedFrom = nullptr;
		std::vector<Objects*>InheritedObj;//list of all inherited objects;
		TransformStruct* InheritedTrans;
		TransformStruct* AbsoluteTrans;
	}Inheritence;
	Objects(Objects & O);
	Objects(short id, std::string ObjName);
	~Objects();
	short Id;
	std::string ObjName;
	std::string SerializationName;
	void SetInheritence(Objects*& o);
	void RemoveHeritence();
	void DeleteInheritence();
	virtual std::vector<ObjectProperties*>* GetProperties() = 0;
	virtual std::string Serialize() { return ""; };
	virtual void inPlayMode() {};
	virtual void InitializePlayMode() {};
	virtual void DeInitializePlayMode() {};
	//Only use this when want to query properties multiple time otherwise its just O(n)
	virtual std::unordered_map<std::string, ObjectProperties*> GetPropertiesQuick();

};
struct ObjectProperties {
	static void PushToObjectPropertyPool(std::string name,std::function<ObjectProperties*(Objects*)> f, std::multimap<std::string, std::function<ObjectProperties*(Objects*)>>& GlobalPropertiesPoolL);
	static std::multimap<std::string,std::function<ObjectProperties*(Objects*)>> GlobalPropertiesPool; //Implement this
	Objects* associatedObj;
	ObjectProperties(Objects* obj);
	ObjectProperties() {} //Should never used directly
	//Called at start of play mode
	virtual void InitPlayMode() {};
	virtual void DeInitPlayMode() {};
	virtual void show() = 0;
	//Called everyframe in playmode
	virtual void inPlayMode() {};
	virtual void UpdateDependency(const void*& ptr) {};
	void UpdateDependency(const void* ptr,void* Destptr);
	virtual ObjectProperties* GetPropertyRef() = 0;
	virtual const std::type_info& GetPropertyType() = 0;
	virtual std::string Serialize() { 
		std::ostringstream bytes;
		std::string s;
		s = s + "{/" + GetPropertyClassName() + "}" + "\n";
		bytes << std::setw(5) << std::setfill('0') << (s.size());
		std::string buffer = "{" + GetPropertyClassName() + ":" + bytes.str() + "}" + "\n";
		s = buffer + s;
		return s; 
	};
	virtual void DeSerialize(std::string block,void* FixPropFunctor,void* Scene) {};
	virtual std::string GetPropertyClassName() = 0;
};
struct TransformStruct :public ObjectProperties {
	TransformStruct(Objects* obj);
private:
	DirectX::XMFLOAT4 EulerToQuat(float yaw,float pitch,float roll);
public:
	void QuatToEuler(float x,float y,float z,float w, float eulerAngles[3]);
	bool isChangedExternally;
	float EulerRot[3] = {0,0,0};
public:
	ObjectProperties* GetPropertyRef();
	const std::type_info& GetPropertyType();
	bool isMoving;
	void Update();
	float rotation[4] = { 0,0,0,1.0f };
	float position[3] = { 0,0,0 };
	float Scale[3] = { 1,1,1 };
	void show();
	std::string GetPropertyClassName() { return "Transform"; };
};
class Globals {
public:
	static bool* inPlayMode;
	static float* dT;
	static bool isFullscreen;
};
struct Vertex {
	struct {
		float x;
		float y;
		float z;
	}position;
	struct {
		float u;
		float v;
	}texcoord;
	struct {
		float x;
		float y;
		float z;
	}Normal;
	Vertex() = default;
	Vertex(std::vector<float> val) :position{ val[0],val[1],val[2] }, texcoord{ val[3],val[4] }, Normal{ val[5],val[6],val[7] } {};
	Vertex(std::initializer_list<float> val) {
		auto it = val.begin();
		position = { *(it++), *(it++), *(it++) };
		texcoord = { *(it++), *(it++) };
		Normal = { *(it++), *(it++), *(it++) };
	};
};
struct NormalPerObject {
	float x;
	float y;
	float z;
	NormalPerObject(float x, float y, float z) :x(x), y(y), z(z) {}
};
struct RefrencePassing {
	void* ObjectPtr;
	const std::type_info& id;
	RefrencePassing(void* ObjectPtr,const std::type_info& id) :ObjectPtr(ObjectPtr),id(id){};
	RefrencePassing():id(typeid(nullptr)){};
	RefrencePassing(const RefrencePassing& ref) :ObjectPtr(ref.ObjectPtr),id(ref.id) {};
};
struct PathToFile {
	std::string Path;
	std::string FileName = "Default";
	PathToFile() {};
	PathToFile(std::string path,std::string filename ):Path(path),FileName(filename){}
};
std::string GetMemAddress(void* ptr);
//---------------------------------Console Stuff------------------------------//
//----------------------------------------------------------------------------//
class DebugConsole {
private:
	struct SharedData {
		std::queue<std::string>* Message_q;             ///-------------------------------------------------------------///
		std::mutex* mute;								///---|  Main exe    |------|  dll    |----|  dll   |-----------///
	};													///--------|-------------------|--------------|-----------------///
	static SharedData* Data;							///---|Console class|----|Console class|---|Console class|------///
	static std::mutex* mute;							///--------|-------------------|----------------|---------------///
	static std::condition_variable cv;                  ///________--------------------------------------_______________///
	static bool Pushed;									///____________________________|________________________________///
	static bool isFinished;								///_______________________|Shared Queue & mutex|________________///
	static std::thread DebugThread;						//Absolute dogshit CPU usage increased by 15%
	static void RunThread();
	static std::queue<std::string>* Message_q;
	//-----------Shared Paging------------// 
	static HANDLE hMapFile;
	static LPVOID ptrToMem;
	//------------------------------------//
public:
	static DebugConsole* GetInstance();
	static void InitDebugConsole();
	static void DestroyConsole();
	static void Log(std::string Message);
};
#define CONSOLE_PRINT(str) DebugConsole::GetInstance()->Log(str)
//---------------------------------------------------------------------------//

//-----------------------------Hot Reloading---------------------------------//

class HotReloading {
private:
	static HANDLE hMapFile;
	
public:
	static LPVOID ptrToMem;
	struct SharedData {
		std::multimap<std::string, std::function<ObjectProperties*(Objects*)>>* GlobalPropertiesPool;
		ImGuiContext* ctx;
	};
	static SharedData* Data;
	static std::multimap<std::string, std::function<ObjectProperties*(Objects*)>>* PropertiesPool;
	static std::multimap<std::string, std::function<ObjectProperties*(Objects*)>> InternalPool;
	static ImGuiContext* IMctx;
	static void Init(std::multimap<std::string, std::function<ObjectProperties*(Objects*)>>* GlobalPropertiesPool = &ObjectProperties::GlobalPropertiesPool,ImGuiContext* ctx = ImGui::GetCurrentContext());
	static void DeInit();
	static void GetInstance();
	static void RegClass(std::string ClassName, std::function<ObjectProperties*(Objects*)>ptrToFactoryFunction);
	static void NukeClass();
};
//---------------------------------------------------------------------------//


//-----------------------------All Shared Data-------------------------------//
class IPC {
private:
public:
	struct SharedData {
		float time;
		ImGuiContext* ctx;
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
		void* Scene;
	};
	static void SetScene(void* Scene);
	static void SetD3D(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext);
	static HANDLE hMapFile;
	static LPVOID ptrToMem;
	static SharedData* Data;
	static void Init(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext, ImGuiContext* ctx = ImGui::GetCurrentContext());
	static void DeInit();
	static void GetInstance();
};
//------------------------------------------------------------------------------------------------------//
