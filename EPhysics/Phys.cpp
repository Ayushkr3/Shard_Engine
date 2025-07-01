#include "pch.h"
#include "Phys.h"
//#include <math.h>
using namespace physx;
ImGuiContext* NVPhysx::privateCtx = nullptr;
physx::PxDefaultAllocator NVPhysx::allocator;
physx::PxDefaultErrorCallback NVPhysx::errorCallback;
physx::PxFoundation* NVPhysx::foundation = nullptr;
physx::PxPhysics* NVPhysx::physicsObj = nullptr;
PxPvdSceneClient* pvdClient = nullptr;
physx::PxPvd* NVPhysx::pvd = nullptr;
physx::PxPvdTransport* NVPhysx::transport = nullptr;
void SharedVarInit(float*&dT, bool*& inPlayMode) {
	Globals::dT = dT;
	Globals::inPlayMode = inPlayMode;
}
//float Physics_Body::gravity = -9.81f;
//using namespace DirectX;
//void Physics_Body::inPlayMode() {
//	Update();
//	PxScene* s;
//}
//void Physics_Body::show()
//{
//	if (ImGui::CollapsingHeader("Physics Body")) {
//		ImGui::Checkbox("Affected by Gravity", &isAffectedByGravity);
//		ImGui::Text(("X velocity "+std::to_string(velocity[0])).c_str());
//		ImGui::Text(("Y velocity " + std::to_string(velocity[1])).c_str());
//		ImGui::Text(("Z velocity " + std::to_string(velocity[2])).c_str());
//		ImGui::Text(("X accel " + std::to_string(acceleration[0])).c_str());
//		ImGui::Text(("Y accel " + std::to_string(acceleration[1])).c_str());
//		ImGui::Text(("Z accel " + std::to_string(acceleration[2])).c_str());
//	}
//}
//Physics_Body::Physics_Body(TransformStruct*& t,Objects*& obj,float*& dt):t(t),dt(dt), ObjectProperties(obj) {
//	lastdT = new float;
//	DirectX::XMFLOAT3 gravityflo = DirectX::XMFLOAT3(0,gravity-10.0f,0);
//	gravityvec = DirectX::XMVECTOR(DirectX::XMLoadFloat3(&gravityflo));
//}
//void Physics_Body::ApplyDrag(std::array<float, 3>* acceleration, float velocity[3],std::array<float, 3>* last_velocity) {
//	auto f = XMFLOAT3(-abs(velocity[0])*velocity[0], -abs(velocity[1])*velocity[1] , -abs(velocity[2])* velocity[2]);
//	//XMStoreFloat3(&f, v);
//	(*acceleration)[0] = (*acceleration)[0]+f.x;
//	(*acceleration)[1] = (*acceleration)[1] + f.y;
//	(*acceleration)[2] = (*acceleration)[2] + f.z;
//	velocity[0] = ((((*acceleration)[0]) * (*dt)) + (*last_velocity)[0]);
//	velocity[1] = ((((*acceleration)[1]) * (*dt)) + (*last_velocity)[1]);
//	velocity[2] = ((((*acceleration)[2]) * (*dt)) + (*last_velocity)[2]);
//}
//void Physics_Body::Update() {
//	if (associatedObj-->Inheritence.inheritedFrom == nullptr)
//		return;
//	last_velocity = {velocity[0],velocity[1],velocity[2]}; //last velocity
//	DirectX::XMFLOAT3 res = DirectX::XMFLOAT3(0, 0, 0); //resultant vector
//	DirectX::XMVECTOR resultant = DirectX::XMVECTOR(DirectX::XMLoadFloat3(&res));//resultant vector
//	unsigned int i = 0;
//	while(i<pendingForces.size()){
//		DirectX::XMVECTOR temp = DirectX::XMVECTOR(DirectX::XMLoadFloat3(&(*pendingForces.cbegin())));
//		resultant = DirectX::XMVectorAdd(temp,resultant);
//		pendingForces.pop_back();
//	}
//	if (isAffectedByGravity) {
//		resultant = DirectX::XMVectorAdd(gravityvec, resultant);
//	}
//	//if (t->isChangedexternally) {
//	//	ResetVelocity();
//	//}
//	XMStoreFloat3(&res, resultant);
//	acceleration[0] = res.x;
//	acceleration[1] = res.y;
//	acceleration[2] = res.z;
//	velocity[0] = (((acceleration[0]) * (*dt))+last_velocity[0]);//instanteous velocity
//	velocity[1] = (((acceleration[1]) * (*dt)) + last_velocity[1]);//instanteous velocity
//	velocity[2] = (((acceleration[2]) * (*dt)) + last_velocity[2]);//instanteous velocity
//
//	ApplyDrag(&acceleration, velocity,&last_velocity);
//
//	if (acceleration[1] != 0)
//		t->position[1] += ((((velocity[1] * velocity[1]) - (last_velocity[1] * last_velocity[1])) / (2 * acceleration[1])));
//	if(acceleration[0]!=0)
//		t->position[0] += ((((velocity[0] * velocity[0]) - (last_velocity[0] * last_velocity[0])) / (2 * acceleration[0])));
//	(velocity[2] > 0) ? t->position[2] -= (velocity[2] * (*dt)) - ((res.z * (*dt)*(*dt)) / 2) : t->position[2] += (velocity[2] * (*dt)) + ((res.z * (*dt)*(*dt)) / 2);
//	
//	
//	//acceleration = {velocity[0]-last_velocity[0]/(*lastdT),velocity[1] - last_velocity[1] / (*lastdT),velocity[2] - last_velocity[2] / (*lastdT) };
//	
//	*lastdT = *dt;
//}
//void Physics_Body::ResetGravity() {
//	gravity = 0;
//}
//void Physics_Body::AddForce(float x,float y,float z) {
//	DirectX::XMFLOAT3 force = DirectX::XMFLOAT3(x, y, z);
//	pendingForces.push_back(force);
//}
//void Physics_Body::ResetVelocity() {
//	t->l_position[0] = t->position[0];
//	t->l_position[1] = t->position[1];
//	t->l_position[2] = t->position[2];
//	velocity[0] =0;
//	velocity[1] =0;
//	velocity[2] =0;
//	*lastdT =1;
//}
//ObjectProperties* Physics_Body::GetPropertyRef() {
//	return this;
//}
//Physics_Body* CreatePhysicsBody(TransformStruct* t,Objects*& obj)
//{
//	return new Physics_Body(t,obj);
//}
void NVPhysx::Init() {
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION,allocator,errorCallback);
	foundation->setErrorLevel(PxErrorCode::eDEBUG_INFO);
	pvd = PxCreatePvd(*NVPhysx::foundation);
	transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425,10000);
	pvd->connect(*transport, PxPvdInstrumentationFlag::eDEBUG);
	physicsObj = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true,pvd);
}
void NVPhysx::Destroy() {
	foundation->release();
	physicsObj->release();
	pvd->disconnect();
	pvd->release();
	transport->disconnect();
	transport->release();
}
void NVPhysx::CreateNewScene(PxScene*& Scene) {
	physx::PxSceneDesc* physcenedesc = new physx::PxSceneDesc(NVPhysx::physicsObj->getTolerancesScale());
	physcenedesc->gravity = PxVec3(0.0f, -9.81f, 0.0f);
	physcenedesc->broadPhaseType = PxBroadPhaseType::eSAP;
	physcenedesc->filterShader = PxDefaultSimulationFilterShader;
	physcenedesc->cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	Scene = physicsObj->createScene(*physcenedesc);
	Scene->getScenePvdClient()->setScenePvdFlags(PxPvdSceneFlag::eTRANSMIT_CONTACTS | PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES | PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS);
	delete physcenedesc;
}
NVPhysx::RigidBody::RigidBody(Objects* obj) :ObjectProperties(obj),trans(nulltrans){

}
ObjectProperties* NVPhysx::RigidBody::GetPropertyRef() {
	return this;
}
PxActor* NVPhysx::RigidBody::GetCurrentActor()
{
	if (isStaticObject == true && StaticActor != nullptr) {
		return StaticActor;
	}
	else if (!isStaticObject && DynamicActor != nullptr) {
		return DynamicActor;
	}
	else {
		nullptr;
	}
	return nullptr;
}
void NVPhysx::RigidBody::InitPlayMode() {
	if (isInitalized) {
		goto SKIP_CREATION;
	}
	if (trans == nullptr)
		assert("UnInitialized");
	if (DynamicActor == nullptr&&StaticActor == nullptr) {
		DynamicActor = physicsObj->createRigidDynamic(physx::PxTransform(physx::PxVec3(this->associatedObj->Inheritence.AbsoluteTrans->position[0], this->associatedObj->Inheritence.AbsoluteTrans->position[1], this->associatedObj->Inheritence.AbsoluteTrans->position[2])));
		StaticActor = physicsObj->createRigidStatic(physx::PxTransform(physx::PxVec3(this->associatedObj->Inheritence.AbsoluteTrans->position[0], this->associatedObj->Inheritence.AbsoluteTrans->position[1], this->associatedObj->Inheritence.AbsoluteTrans->position[2])));
		DynamicActor->setActorFlag(PxActorFlag::eVISUALIZATION, true);
		DynamicActor->userData = associatedObj;
		StaticActor->userData = associatedObj;
	}
	SKIP_CREATION:
	if (DynamicActor != nullptr) {
		DynamicActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !isAffectedbyGravity);
		DynamicActor->setGlobalPose(physx::PxTransform(physx::PxVec3(this->associatedObj->Inheritence.AbsoluteTrans->position[0], this->associatedObj->Inheritence.AbsoluteTrans->position[1], this->associatedObj->Inheritence.AbsoluteTrans->position[2]), PxQuat(trans->rotation[0], trans->rotation[1], trans->rotation[2],trans->rotation[3])));
		DynamicActor->setLinearVelocity(PxVec3(0.0f, 0.0f, 0.0f));
		DynamicActor->setAngularVelocity(PxVec3(0.0f, 0.0f, 0.0f));
	}
	if (StaticActor != nullptr) {
		StaticActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !isAffectedbyGravity);
		StaticActor->setGlobalPose(physx::PxTransform(physx::PxVec3(this->associatedObj->Inheritence.InheritedTrans->position[0]+trans->position[0], this->associatedObj->Inheritence.InheritedTrans->position[1] + trans->position[1], this->associatedObj->Inheritence.InheritedTrans->position[2] + trans->position[2]), PxQuat(trans->rotation[0], trans->rotation[1], trans->rotation[2], trans->rotation[3])));
	}
	isInitalized = true;
}
void NVPhysx::RigidBody::show() {
	ImGui::SetCurrentContext(privateCtx);
	if (ImGui::CollapsingHeader("RigiBody", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader)) {
		ImGui::Button("RigidBody");
		if (ImGui::BeginDragDropSource()) {
			std::unique_ptr<RefrencePassing> ref = std::make_unique<RefrencePassing>((void*)this, typeid(*this));
			//RefrencePassing* ref = new RefrencePassing(this, typeid(*this));
			ImGui::SetDragDropPayload("RigidBody", ref.get(), sizeof(RefrencePassing));
			ImGui::EndDragDropSource();
		}
		ImGui::Text("Transform"); ImGui::SameLine(); ImGui::Button(GetMemAddress((void*)trans).c_str());
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Transform")) {
				UpdateDependency(payload->Data);
				ImGui::EndDragDropTarget();
			}
		}
		if (ImGui::Checkbox("Affected by Gravity", &isAffectedbyGravity)) {
			if (*Globals::inPlayMode) {
				if (DynamicActor->getScene() != nullptr) {
					DynamicActor->wakeUp();
					DynamicActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !isAffectedbyGravity);
				}
			}
		}
		if (ImGui::Checkbox("Is static object", &isStaticObject)) {
			if (isStaticObject == true&& *Globals::inPlayMode) {
				PxScene* CurrentScene = DynamicActor->getScene();
				CurrentScene->removeActor(*DynamicActor);
				CurrentScene->addActor(*StaticActor);
				StaticActor->setGlobalPose(physx::PxTransform(physx::PxVec3(this->associatedObj->Inheritence.AbsoluteTrans->position[0], this->associatedObj->Inheritence.AbsoluteTrans->position[1], this->associatedObj->Inheritence.AbsoluteTrans->position[2]),PxQuat(trans->rotation[0], trans->rotation[1], trans->rotation[2],trans->rotation[3])));
			}
			else {
				if (StaticActor != nullptr&& *Globals::inPlayMode) {
				PxScene* CurrentScene = StaticActor->getScene();
				CurrentScene->removeActor(*StaticActor);
				CurrentScene->addActor(*DynamicActor);
				DynamicActor->setGlobalPose(physx::PxTransform(physx::PxVec3(this->associatedObj->Inheritence.AbsoluteTrans->position[0], this->associatedObj->Inheritence.AbsoluteTrans->position[1], this->associatedObj->Inheritence.AbsoluteTrans->position[2]), PxQuat(trans->rotation[0], trans->rotation[1], trans->rotation[2], trans->rotation[3])));
				}
			}
		}
		if (DynamicActor == nullptr && StaticActor == nullptr) {
			return;
		}
		if (DynamicActor->getScene() != nullptr) {
			ImGui::Text(("X " + std::to_string((DynamicActor->getGlobalPose()).p.x)).c_str());
			ImGui::Text(("Y " + std::to_string((DynamicActor->getGlobalPose()).p.y)).c_str());
			ImGui::Text(("Z " + std::to_string((DynamicActor->getGlobalPose()).p.z)).c_str());
			ImGui::Text(("X Rot" + std::to_string((DynamicActor->getGlobalPose()).q.x)).c_str());
			ImGui::Text(("Y Rot" + std::to_string((DynamicActor->getGlobalPose()).q.y)).c_str());
			ImGui::Text(("Z Rot" + std::to_string((DynamicActor->getGlobalPose()).q.z)).c_str());
			ImGui::Text(("W Rot" + std::to_string((DynamicActor->getGlobalPose()).q.w)).c_str());
		}
		else {
			ImGui::Text(("X " + std::to_string((StaticActor->getGlobalPose()).p.x)).c_str());
			ImGui::Text(("Y " + std::to_string((StaticActor->getGlobalPose()).p.y)).c_str());
			ImGui::Text(("Z " + std::to_string((StaticActor->getGlobalPose()).p.z)).c_str());
		}
	}
}
void NVPhysx::RigidBody::inPlayMode() {
	UpdatePhysics();
}
void NVPhysx::RigidBody::UpdatePhysics() {
	if (DynamicActor->getScene()!=nullptr) {
		if (trans->isChangedExternally == false) {
			physx::PxTransform updateTrans = DynamicActor->getGlobalPose();
			trans->position[0] = updateTrans.p.x - this->associatedObj->Inheritence.InheritedTrans->position[0];
			trans->position[1] = updateTrans.p.y - this->associatedObj->Inheritence.InheritedTrans->position[1];
			trans->position[2] = updateTrans.p.z - this->associatedObj->Inheritence.InheritedTrans->position[2];
			trans->rotation[0] = updateTrans.q.x;
			trans->rotation[1] = updateTrans.q.y;
			trans->rotation[2] = updateTrans.q.z;
			trans->rotation[3] = updateTrans.q.w;
		}
		else {
			DynamicActor->setGlobalPose(physx::PxTransform(physx::PxVec3(this->associatedObj->Inheritence.AbsoluteTrans->position[0],this->associatedObj->Inheritence.AbsoluteTrans->position[1], this->associatedObj->Inheritence.AbsoluteTrans->position[2]), PxQuat(trans->rotation[0], trans->rotation[1], trans->rotation[2], trans->rotation[3])));
		}
	}
}
std::string NVPhysx::RigidBody::Serialize() {
	return Serialization::SerializeProperty(*this);
}
void NVPhysx::RigidBody::DeSerialize(std::string block, void* FixPropFunctor, void* Scene) {
	std::istringstream iss(block);
	std::string line;
	std::getline(iss, line);
	while (std::getline(iss, line)) {
		size_t off = line.find(":");
		std::string type = line.substr(0, off);
		switch (std::stoi(type))
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			off = line.find(":", off + 1);
			std::string enclosed = line.substr(off + 1, line.size() - 2);
			if (enclosed.front() == '<' && enclosed.back() == '>') {
				//call functor with this
				void(*func)(void*, void*) = (void(*)(void*, void*))(FixPropFunctor);
				func(Scene, (void*)&Serialization::CallerObject(associatedObj->Id, (void*)&enclosed));
			}
			break;
		}
	}
}

//-------------------------------------------------------------------------------//
//---------------------------------Factroy functions----------------------------//
//-------------------------------------------------------------------------------//
ObjectProperties* CreateRigidBody(Objects* obj) {
	return new NVPhysx::RigidBody(obj);
}
void NVPhysx::RigidBody::RegisterFactory(std::multimap<std::string, std::function<ObjectProperties*(Objects*)>>& GlobalPropertiesPoolL) {
	std::function<ObjectProperties*(Objects*)> f = CreateRigidBody;
	ObjectProperties::PushToObjectPropertyPool("RigidBody",f, GlobalPropertiesPoolL);
}
//------------------------------------------------------------------------------//

//Updates inter properties dependency
//Drag and drop dependency
void NVPhysx::RigidBody::UpdateDependency(const void* ptr) {
	RefrencePassing* ref = (RefrencePassing*)ptr;
	if (ref->id == typeid(TransformStruct)) {
		trans = (TransformStruct*)ref->ObjectPtr;
	}
}
const std::type_info& NVPhysx::RigidBody::GetPropertyType() {
	return typeid(NVPhysx::RigidBody);
}
void NVPhysx::RigidBody::DeInitPlayMode() {
	isInitalized = false;
}