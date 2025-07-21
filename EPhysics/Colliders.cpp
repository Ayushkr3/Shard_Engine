#include "pch.h" //Should be included at top :'( wasted 5 hours
#include "Phys.h"
using namespace physx;
//#include "Colliders.h"
//#include "EPhysics.h"
//#include "ImGui/imgui.h"
////SAT algorithm
//using namespace DirectX;
//using namespace Collision;
//BoxCollider::BoxCollider(TransformStruct*& trans,std::vector<Vertex>& vertices, std::vector<NormalPerObject> Normals,Objects*& obj,ImGuiContext * ctx) :vert(vertices), ObjectsNormals{Normals},cxt(ctx), objTransform(trans),ObjectProperties(obj),phys(nullptr){
//	for (auto& o : *obj->GetProperties()) {
//		if (dynamic_cast<Physics_Body*>(o) != nullptr) {
//			Physics_Body* phy = (dynamic_cast<Physics_Body*>(o));
//			phys = phy;
//		}
//	}
//	CalcAxis(Normals);
//	ImGui::SetCurrentContext(ctx);
//}
//Collision::Projection BoxCollider::CalcProjection(DirectX::XMFLOAT3 normals) {
//	//std::vector<Projection> allProj;
//	Projection pro;
//		pro.mini = FLT_MAX;
//		pro.maxi = -FLT_MAX;
//		auto vec = DirectX::XMLoadFloat3(&(normals));
//		for (unsigned int k = 0; k < vert.size(); k++) {
//			float maxi = -FLT_MAX;
//			float mini = FLT_MAX;
//			auto pos = XMFLOAT3(vert[k].position.x, vert[k].position.y, vert[k].position.z);
//			float proj = XMVectorGetX(XMVector3Dot(vec, XMLoadFloat3(&pos)));
//			maxi = max(proj, maxi);
//			mini = min(proj, mini);
//			pro.maxi = max(maxi, pro.maxi);
//			pro.mini = min(mini, pro.mini);
//		}
//	return pro;
//}
//bool BoxCollider::Comp(DirectX::XMFLOAT3& v1, DirectX::XMFLOAT3& v2) {
//	if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
//		return true;
//	return false;
//}
//void BoxCollider::CalcAxis(std::vector<NormalPerObject>& Nor) {
//	if (Nor.empty()) {
//		return;
//	}
//	for (unsigned int i = 0; i < (Nor).size(); i++) {
//		auto nor = DirectX::XMFLOAT3(Nor[i].x, Nor[i].y, Nor[i].z);
//		axis1.normals.push_back(nor);
//	}
//}
//void BoxCollider::UpdateBuffer(std::vector<Vertex>& vertices,std::vector<NormalPerObject>& n) {
//	vert = vertices;
//	for (unsigned int i = 0; i < n.size(); i++) {
//			auto nor = DirectX::XMFLOAT3((n)[i].x, n[i].y, n[i].z);
//			axis1.normals[i] = nor;
//		//auto vec = DirectX::XMLoadFloat3(&nor);
//	}
//	//axis1.normals = std::vector<XMFLOAT3>(std::set<XMFLOAT3>(temp.begin(),temp.end()).begin(),std::set<XMFLOAT3>(temp.begin(), temp.end()).end());
//}
//////////////////////----Narrow Phase-----/////////////////////////////
//bool BoxCollider::CheckCollision(BoxCollider secondObj) {
//	bool isColliding = true;
//	std::vector<Collision::Projection> allproj;
//	DirectX::XMFLOAT3 collaxis;
//	float minover = std::numeric_limits<float>::infinity();
//	for (unsigned int i = 0; i < axis1.normals.size();i++) {
//		Projection p1;
//		Projection p2;
//		p1 = CalcProjection(axis1.normals[i]);
//		p2 = secondObj.CalcProjection(axis1.normals[i]);
//		if (IsOverlap(p1.mini, p1.maxi, p2.mini, p2.maxi)) {
//			
//			float overlap = min(p1.maxi, p2.maxi) - max(p1.mini, p2.mini);
//			isColliding = isColliding && true;
//			if (minover > overlap) {
//				collaxis = axis1.normals[i];
//				minover = overlap;
//			}
//		}
//		else {
//			isColliding = isColliding && false;
//		}
//	}
//	XMVECTOR vert1 = XMVECTOR(XMLoadFloat3(&XMFLOAT3(vert[0].position.x,vert[0].position.y, vert[0].position.z)));
//	XMVECTOR vert2 = XMVECTOR(XMLoadFloat3(&XMFLOAT3(secondObj.vert[0].position.x, secondObj.vert[0].position.y, secondObj.vert[0].position.z)));
//	XMVECTOR diff = XMVectorSubtract(vert1, vert2);
//	auto angle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(diff), XMLoadFloat3(&collaxis)));
//	if (XMVectorGetX(XMVector3Dot(XMVector3Normalize(diff), XMLoadFloat3(&collaxis))) < 0) {
//		collaxis.x = -collaxis.x;
//		collaxis.y = -collaxis.y;
//		collaxis.z = -collaxis.z;
//	}
//	isColliding ? lastCollaxis = collaxis :lastCollaxis = DirectX::XMFLOAT3(0,0,0);
//	if (isColliding && !isStaticObject) {
//		minover = std::round(minover*1000000.0f)/1000000.0f;
//		objTransform->position[0] = objTransform->position[0] + (collaxis.x*minover);
//		objTransform->position[1] = objTransform->position[1] + (collaxis.y*minover);
//		objTransform->position[2] = objTransform->position[2] + (collaxis.z*minover);
//		lastCollaxis = collaxis;
//	}
//	 //p1 = CalcProjection(axis1.normals);
//	 //p2 = secondObj.CalcProjection(axis1.normals); // potential to remove self projection
//	// isColliding = CheckCollisionSAT(p1, p2);
//	 //get the projection of 1 object on 2 object on axis --- Done p1 p2
//	 //p1 the projection of itself on its normal
//	 //compare p2 with p1 of the other object 
//	 return isColliding;
//}
//void BoxCollider::ResolveCollision() {
//	if (!isStaticObject) {
//		if(lastCollaxis.x == 0 && lastCollaxis.y == 1.0f&&lastCollaxis.z == 0)
//		phys->AddForce(0, 10.0f+9.81f, 0);
//		else {
//			phys->AddForce(lastCollaxis.x, lastCollaxis.y*phys->acceleration[1],lastCollaxis.z);
//		}
//	}
//}
//bool BoxCollider::CheckCollisionSAT(const std::vector<Projection>& cubeProjections, const std::vector<Projection>& planeProjections)
//{
//	for (unsigned int i = 0; i < cubeProjections.size();i++) {
//			if (!IsOverlap(cubeProjections[i].mini, cubeProjections[i].maxi, planeProjections[i].mini, planeProjections[i].maxi)) {
//				return false;
//			}
//	}
//	return true;
//}
//
//bool  BoxCollider::IsOverlap(float minA, float maxA, float minB, float maxB)
//{
//	return (minA <= maxB && maxA >= minB);
//}
//void BoxCollider::show() {
//	if (ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_CollapsingHeader)) {
//		ImGui::Checkbox("Static Object", &isStaticObject);
//	//	ImGui::Text(("Collision" + std::to_string(isColliding)).c_str());
//	}
//}
//////////////////////--------------------//////////////////////
//ObjectProperties* BoxCollider::GetPropertyRef() {
//	return this;
//}
//BoxCollider* CreateBoxCollider(Objects*& obj,TransformStruct*& trans, std::vector<Vertex>& vertices, std::vector<NormalPerObject> Normals)
//{
//	return new BoxCollider(trans,vertices,Normals,obj);
//}
NVPhysx::BaseCollider::BaseCollider() {

}
NVPhysx::Collider::BoxCollider::BoxCollider(Objects* obj):BaseCollider(),ObjectProperties(obj) {
}
void NVPhysx::Collider::BoxCollider::InitPlayMode() {
	try {
		if (isInitalized)
			return;
		if (rb == nullptr)
			throw 1;
		rb->InitPlayMode();
		if (this->Material == nullptr) {
			this->Material = physicsObj->createMaterial(0.5f, 0.5f, 0.5f);
		}
		if (this->shape == nullptr) {
			this->shape = physicsObj->createShape(PxBoxGeometry(1.0f, 1.0f, 1.0f), *this->Material);
		}
		if (rb->DynamicActor == rb->GetCurrentActor()) {
			rb->DynamicActor->attachShape(*this->shape);
		}
		if (rb->StaticActor == rb->GetCurrentActor()) {
			rb->StaticActor->attachShape(*this->shape);
		}
		isInitalized = true;
	}
	catch (int Error) {
		assert(Error);
	}
}
void NVPhysx::Collider::BoxCollider::show() {
	ImGuiContext* c = privateCtx;
	ImGui::SetCurrentContext(privateCtx);
	if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader)) {
		ImGui::Text("RigidBody"); ImGui::SameLine(); ImGui::Button(GetMemAddress((void*)rb).c_str());
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RigidBody")) {
				UpdateDependency((void*)payload->Data);
				ImGui::EndDragDropTarget();
			}
		}
		
	}
}
void NVPhysx::Collider::BoxCollider::UpdateDependency(const void* ptr) {
	RefrencePassing* ref = (RefrencePassing*)(ptr);
	if (ref->id == typeid(RigidBody)) {
		rb = (RigidBody*)(ref->ObjectPtr);
	}
}
void NVPhysx::Collider::BoxCollider::DeInitPlayMode() {
	isInitalized = false;
}
NVPhysx::Collider::PlaneCollider::PlaneCollider(Objects* obj):ObjectProperties(obj) {

}
const std::type_info & NVPhysx::Collider::PlaneCollider::GetPropertyType()
{
	return typeid(NVPhysx::Collider::PlaneCollider);
}
ObjectProperties * NVPhysx::Collider::PlaneCollider::GetPropertyRef()
{
	return this;
}
void NVPhysx::Collider::PlaneCollider::InitPlayMode() {
	try {
		if (isInitalized)
			return;
		if (rb == nullptr)
			throw 1;
		rb->InitPlayMode();
		if (this->Material == nullptr) {
			this->Material = physicsObj->createMaterial(0.5f, 0.5f, 0.5f);
		}
		if (this->shape == nullptr) {
			this->shape = physicsObj->createShape(PxBoxGeometry(1.0f, 0.001f, 1.0f), *this->Material);
		}
		if (rb->DynamicActor == rb->GetCurrentActor()) {
			rb->DynamicActor->attachShape(*this->shape);
		}
		if (rb->StaticActor == rb->GetCurrentActor()) {
			rb->StaticActor->attachShape(*this->shape);
		}
		isInitalized = true;
	}
	catch (int Error) {
		assert(Error);
	}
}
void NVPhysx::Collider::PlaneCollider::DeInitPlayMode()
{
	isInitalized = false;
}
void NVPhysx::Collider::PlaneCollider::show()
{
	ImGuiContext* c = privateCtx;
	ImGui::SetCurrentContext(privateCtx);
	if (ImGui::CollapsingHeader("Plane Collider", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader)) {
		ImGui::Text("RigidBody"); ImGui::SameLine(); ImGui::Button(GetMemAddress((void*)rb).c_str());
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RigidBody")) {
				UpdateDependency((void*)payload->Data);
			}
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Objects")) {
				RefrencePassing* ref = (RefrencePassing*)payload->Data;
				RefrencePassing ref2(nullptr, typeid(RigidBody));
				ObjectProperties::UpdateDependency((void*)ref, (void*)&ref2);
				if (ref2.ObjectPtr != nullptr) {
					UpdateDependency(&ref2);
				}
			}
			ImGui::EndDragDropTarget();
		}

	}
}
std::string NVPhysx::Collider::BoxCollider::Serialize() {
	return Serialization::SerializeProperty(*this);
}
void NVPhysx::Collider::BoxCollider::DeSerialize(std::string block, void* FixPropFunctor, void* Scene) {
	std::istringstream iss(block);
	std::string line;
	std::getline(iss, line);
	while (std::getline(iss, line)) {
		if (line == "{/Box Collider}")break;
		size_t off = line.find(":");
		std::string type = line.substr(0, off);
		switch (std::stoi(type))
		{
		case 0:
			off = line.find(":", off + 1);
			std::string enclosed = line.substr(off + 1, line.size() - 2);
			if (enclosed.front() == '<' && enclosed.back() == '>') {
				//call functor with this
				void* ref = nullptr;
				void(*func)(void*, void*, void*) = (void(*)(void*, void*, void*))(FixPropFunctor);
				func(Scene,(void*)&Serialization::CallerObject(associatedObj->Id,(void*)&enclosed),&ref);
				UpdateDependency(ref);
			}
			break;
		}
	}
	associatedObj->GetProperties()->push_back(this);
}
void NVPhysx::Collider::PlaneCollider::UpdateDependency(const void * ptr)
{
	RefrencePassing* ref = (RefrencePassing*)(ptr);
	if (ref->id == typeid(RigidBody)) {
		rb = (RigidBody*)(ref->ObjectPtr);
	}
}
//-------------------------------------------------------------------------------//
//---------------------------------Factroy functions----------------------------//
//-------------------------------------------------------------------------------//
ObjectProperties* CreateBoxCollider(Objects* obj) {
	return new NVPhysx::Collider::BoxCollider(obj);
}
ObjectProperties* CreatePlaneCollider(Objects* obj) {
	return new NVPhysx::Collider::PlaneCollider(obj);
}
ObjectProperties* NVPhysx::Collider::BoxCollider::GetPropertyRef() {
	return this;
}
void NVPhysx::Collider::BoxCollider::RegisterFactory(std::multimap<std::string, std::function<ObjectProperties*(Objects*)>>& GlobalPropertiesPoolL) {
	std::function<ObjectProperties*(Objects*)> f = CreateBoxCollider;
	ObjectProperties::PushToObjectPropertyPool("Box Collider",f,GlobalPropertiesPoolL);
}
void NVPhysx::Collider::PlaneCollider::RegisterFactory(std::multimap<std::string, std::function<ObjectProperties*(Objects*)>>& GlobalPropertiesPoolL)
{
	std::function<ObjectProperties*(Objects*)> f = CreatePlaneCollider;
	ObjectProperties::PushToObjectPropertyPool("Plane Collider", f, GlobalPropertiesPoolL);
}
const std::type_info& NVPhysx::Collider::BoxCollider::GetPropertyType() {
	return typeid(NVPhysx::Collider::BoxCollider);
}
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//