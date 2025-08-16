#include "pch.h"
#include "Global.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui.h"

bool* Globals::inPlayMode = nullptr;
float* Globals::dT = 0;
bool Globals::isFullscreen = false;
std::multimap<std::string,std::function<ObjectProperties*(Objects*)>> ObjectProperties::GlobalPropertiesPool = {};
ObjectProperties::ObjectProperties(Objects* obj) :associatedObj(obj){

}
TransformStruct::TransformStruct(Objects* obj) : ObjectProperties(obj) {
	if (obj != nullptr&&obj->Inheritence.inheritedFrom!=nullptr) {
		obj->Inheritence.AbsoluteTrans->position[0] = position[0] + obj->Inheritence.InheritedTrans->position[0];
		obj->Inheritence.AbsoluteTrans->position[1] = position[1] + obj->Inheritence.InheritedTrans->position[1];
		obj->Inheritence.AbsoluteTrans->position[2] = position[2] + obj->Inheritence.InheritedTrans->position[2];
	}
}
static TransformStruct* t = new TransformStruct(nullptr);
void ObjectProperties::PushToObjectPropertyPool(std::string name, std::function<ObjectProperties*(Objects*)> f, std::multimap<std::string, std::function<ObjectProperties*(Objects*)>>& GlobalPropertiesPoolL = ObjectProperties::GlobalPropertiesPool) {
	GlobalPropertiesPoolL.insert({name,f});
}
Objects::Objects(Objects& O):Id(O.Id),ObjName(O.ObjName) {
	if (O.Inheritence.inheritedFrom!=nullptr) {
		Inheritence.InheritedTrans = O.Inheritence.InheritedTrans;
		Inheritence.AbsoluteTrans = O.Inheritence.AbsoluteTrans;
		Inheritence.inheritedFrom = O.Inheritence.inheritedFrom;
		Inheritence.InheritedObj = O.Inheritence.InheritedObj;
	}
	else {
		Inheritence.InheritedTrans = new TransformStruct(nullptr);
		Inheritence.AbsoluteTrans = new TransformStruct(nullptr);
	}
	(Inheritence.InheritedTrans)->position[0] = 0;
	(Inheritence.InheritedTrans)->position[1] = 0;
	(Inheritence.InheritedTrans)->position[2] = 0;
	(Inheritence.AbsoluteTrans)->position[0] = 0;
	(Inheritence.AbsoluteTrans)->position[1] = 0;
	(Inheritence.AbsoluteTrans)->position[2] = 0;
	//Inheritence.InheritedObj = O.Inheritence.InheritedObj;
	//Inheritence.inheritedFrom = O.Inheritence.inheritedFrom;
}
Objects::Objects(short id, std::string ObjName) :Id(id), ObjName(ObjName) {
	Inheritence.InheritedTrans = new TransformStruct(nullptr);
	Inheritence.AbsoluteTrans = new TransformStruct(nullptr);
	(Inheritence.InheritedTrans)->position[0] = 0;
	(Inheritence.InheritedTrans)->position[1] = 0;
	(Inheritence.InheritedTrans)->position[2] = 0;
	(Inheritence.AbsoluteTrans)->position[0] = 0;
	(Inheritence.AbsoluteTrans)->position[1] = 0;
	(Inheritence.AbsoluteTrans)->position[2] = 0;
};

Objects::~Objects() {
	this->RemoveHeritence();
	//delete this->Inheritence.InheritedTrans;
}
DirectX::XMFLOAT4 TransformStruct::EulerToQuat(float yaw,float pitch,float roll) {
	yaw = DirectX::XMConvertToRadians(yaw);
	pitch = DirectX::XMConvertToRadians(pitch);
	roll = DirectX::XMConvertToRadians(roll);
	float cy = std::cos(yaw * 0.5f);
	float sy = std::sin(yaw * 0.5f);
	float cp = std::cos(pitch * 0.5f);
	float sp = std::sin(pitch * 0.5f);
	float cr = std::cos(roll * 0.5f);
	float sr = std::sin(roll * 0.5f);
	float x = sr * cp * cy - cr * sp * sy;
	float y = cr * sp * cy + sr * cp * sy;
	float z = cr * cp * sy - sr * sp * cy;
	float w = cr * cp * cy + sr * sp * sy;
	return DirectX::XMFLOAT4(x,y,z,w);
}
double normalizeAngle(double angle) {
	while (angle > 180.0) angle -= 360.0;
	while (angle < -180.0) angle += 360.0;
	return angle;
}
void TransformStruct::QuatToEuler(float x,float y ,float z, float w,float eulerAngles[3]) {

	double sinr_cosp = 2 * (w * x + y * z);
	double cosr_cosp = 1 - 2 * (x * x + y * y);
	eulerAngles[0] = std::atan2(sinr_cosp, cosr_cosp) * 180.0 / DirectX::XM_PI; // Convert to degrees

	double sinp = 2 * (w * y - z * x);
	if (std::abs(sinp) >= 1)
		eulerAngles[1] = std::copysign(90.0, sinp); // Gimbal lock, set to ±90 degrees
	else
		eulerAngles[1] = std::asin(sinp) * 180.0 / DirectX::XM_PI; // Convert to degrees

	double siny_cosp = 2 * (w * z + x * y);
	double cosy_cosp = 1 - 2 * (y * y + z * z);
	eulerAngles[2] = std::atan2(siny_cosp, cosy_cosp) * 180.0 / DirectX::XM_PI; // Convert to degrees

	eulerAngles[0] = normalizeAngle(eulerAngles[0]); // Roll
	eulerAngles[1] = normalizeAngle(eulerAngles[1]); // Pitch
	eulerAngles[2] = normalizeAngle(eulerAngles[2]); // Yaw
}
void TransformStruct::show() {
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader)) {
		ImGui::Button("Transform");
		if (ImGui::BeginDragDropSource()) {
			std::unique_ptr<RefrencePassing> ref = std::make_unique<RefrencePassing>((void*)this, typeid(*this));
			//RefrencePassing* ref = new RefrencePassing(this, typeid(*this));
			ImGui::SetDragDropPayload("Transform", ref.get(), sizeof(RefrencePassing));
			ImGui::EndDragDropSource();
		}
		
		if (ImGui::DragFloat3("Rotation",EulerRot, 0.1f))
		{
			isChangedExternally = true;
			associatedObj->Inheritence.AbsoluteTrans->isChangedExternally = true;
			auto vec = EulerToQuat(EulerRot[0], EulerRot[1], EulerRot[2]);
			rotation[0] = vec.x;
			rotation[1] = vec.y;
			rotation[2] = vec.z;
			rotation[3] = vec.w;
		}
		else {
			QuatToEuler(rotation[0], rotation[1], rotation[2], rotation[3], EulerRot);
			isChangedExternally = false;
			associatedObj->Inheritence.AbsoluteTrans->isChangedExternally = false;
		}
		if(ImGui::DragFloat3("Position", position, 0.1f))
		{
			isChangedExternally = true;
			associatedObj->Inheritence.AbsoluteTrans->isChangedExternally = true;
		}
		else {
			isChangedExternally = false;
			associatedObj->Inheritence.AbsoluteTrans->isChangedExternally = false;
		}
		ImGui::DragFloat3("Scale", Scale, 0.1f);
		
	}
}
ObjectProperties* TransformStruct::GetPropertyRef() {
	return this;
}
void TransformStruct::Update() {
	isMoving = false;
}
void Objects::SetInheritence(Objects*& o) {
	if (this->Inheritence.inheritedFrom == o)
		return; //already inherited by object
	this->Inheritence.InheritedTrans = o->Inheritence.AbsoluteTrans;
	this->Inheritence.inheritedFrom = o;
	TransformStruct* b = nullptr;
	for (auto& op : *this->GetProperties()) {
		if (dynamic_cast<TransformStruct*>(op) != nullptr) {
			b = (dynamic_cast<TransformStruct*>(op));
		}
	}
	o->Inheritence.InheritedObj.push_back(this);
	if (b != nullptr) {
		b->position[0] = b->position[0] - o->Inheritence.AbsoluteTrans->position[0];
		b->position[1] = b->position[1] - o->Inheritence.AbsoluteTrans->position[1];
		b->position[2] = b->position[2] - o->Inheritence.AbsoluteTrans->position[2];
	}
}
void ObjectProperties::UpdateDependency(const void* ptr,void* Destptr) {
	RefrencePassing* ref = (RefrencePassing*)ptr;
	Objects* obj = static_cast<Objects*>(ref->ObjectPtr);
	RefrencePassing* Dest = (RefrencePassing*)Destptr;
	for (auto& prop : *(obj->GetProperties())) {
		if (prop->GetPropertyType() == Dest->id) {
			Dest->ObjectPtr = prop;
			break;
		}
	}
}
void Objects::DeleteInheritence() {
	for (auto it = this->Inheritence.InheritedObj.begin(); it != this->Inheritence.InheritedObj.end(); ++it) {
		(*it)->Inheritence.inheritedFrom = nullptr;
	}
}
void Objects::RemoveHeritence(){
	if (this->Inheritence.inheritedFrom != nullptr) {
		for (auto it = this->Inheritence.inheritedFrom->Inheritence.InheritedObj.begin(); it != this->Inheritence.inheritedFrom->Inheritence.InheritedObj.end(); ++it) {
			if (*it == this) {
				
				this->Inheritence.inheritedFrom->Inheritence.InheritedObj.erase(it);
				break;
			}
		}
	}
	if (!this->Inheritence.InheritedTrans) {
		this->Inheritence.InheritedTrans = nullptr;
		this->Inheritence.InheritedTrans = new TransformStruct(nullptr);
	}
	if (this->Inheritence.AbsoluteTrans) {
		//delete this->Inheritence.AbsoluteTrans;
		this->Inheritence.AbsoluteTrans = nullptr;
		
	}
	//this->Inheritence.InheritedObj.clear();
	this->Inheritence.inheritedFrom = nullptr;
}
bool Objects::DeleteProperty(ObjectProperties* Property) {
	auto vec = *(this->GetProperties());
	for (auto it = vec.begin(); it != vec.end();it++) {
		if (*it == Property) {
			vec.erase(it);
			return true;
		}
	}
	return false;
}
std::string GetMemAddress(void* ptr) {
	std::ostringstream oss; oss << ptr; return oss.str().substr(oss.str().size()-7, 7);
}
const std::type_info& TransformStruct::GetPropertyType() {
	return typeid(TransformStruct);
}

std::unordered_map<std::string, ObjectProperties*> Objects::GetPropertiesQuick() {
	std::unordered_map<std::string, ObjectProperties*> hashed;
	for (auto& it : (*GetProperties())) {
		hashed[it->GetPropertyClassName()] = it;
	}
	return hashed;
}

HANDLE IPC::hMapFile;
LPVOID IPC::ptrToMem;
IPC::SharedData* IPC::Data = nullptr;

//------------------------------------------------------------------------------------------//
void IPC::Init(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext, ImGuiContext* ctx) {
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, sizeof(SharedData), L"IPC");
	ptrToMem = MapViewOfFile(hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(SharedData));
	if (Data == nullptr)
		Data = new SharedData;
	Data->ctx = ctx;
	//Data->pDevice = pDevice;
	//Data->pContext = pContext;
	CopyMemory(ptrToMem, Data, sizeof(SharedData));
}
void IPC::GetInstance() {
	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, L"IPC");
	if (hMapFile == nullptr)
		return;
	if (ptrToMem == nullptr) {
		ptrToMem = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));
		IPC::Data = (SharedData*)(ptrToMem);
	}
	if (ImGui::GetCurrentContext() == nullptr)
		ImGui::SetCurrentContext(((SharedData*)(ptrToMem))->ctx);
}
void IPC::DeInit() {
	if (ptrToMem != nullptr) {
		UnmapViewOfFile(ptrToMem);
	}
	if (hMapFile) {
		CloseHandle(hMapFile);
	}
}
void IPC::SetScene(void* Scene) {
	((SharedData*)(ptrToMem))->Scene = Scene;
}
void IPC::SetD3D(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext) {
	((SharedData*)(ptrToMem))->pDevice = pDevice;
	((SharedData*)(ptrToMem))->pContext = pContext;
}
//------------------------------------------------------------------------------------------------//