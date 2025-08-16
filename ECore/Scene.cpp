#include "pch.h"
#include "Scene.h"
#include "Shapes.h"
#include "ObjParser.h"
#include "EUI.h"
short Scene::currentOBJID = 10;
using namespace physx;
std::vector<short> Scene::globalCurrentOBJID = {};
std::vector<Serialization::ObjectBlocks>* Scene::ObjectsBlocks=nullptr;
Scene::Scene(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext)
	:cam(pDevice), pContext(pContext), pDevice(pDevice)

{
	pDumbDevice = pDevice.Get();
	pDumbContext = pContext.Get();
	NVPhysx::CreateNewScene(physScene);
	LoadSkyBox();
	cam.CamProperties.push_back(&dc);
	dc.SetCameraProp(cam.PosNrot);
	//-------------------------------------//
	//-------------------------------------//
	AllObject.push_back(&cam);
}
Scene::~Scene() {
	physScene->release();
}
void Scene::Render() {
	dc.DebugCamMovement();
	cam.calculateProjection(pContext, &(cam.GetViewMatrix()));
	//TODO: Fix proper lighting
	pSkyBox->Draw();
	for (auto& obj : AllObject) {
		
		if (dynamic_cast<Renderable*>(obj) != nullptr) {
			Renderable* red = dynamic_cast<Renderable*>(obj);
			red->UpdateBuffers();
			red->Draw();
		}
	}
}
void Scene::RenderWireFrame() {
	for (auto& Triangle : Triangles) {
		Triangle->Draw();
#ifdef WIREFRAME_ENABLED
		Triangle->UpdateBuffers();
#endif 
		//Triangle->UpdateBuffers();
		//CContoller->Update(Triangle);
	}
}

void Scene::PlayMode()
{
	physScene->simulate(PxReal(1.0f / 60.0f));
	for (auto& Obj : AllObject) {
		Obj->inPlayMode();	//Call every object properties which want to act in play Mode
	}
	auto n = physScene->fetchResults(true);
	//CContoller->Update();
}
void Scene::InitalizePlayMode()
{
	inPlayMode = true;
	//CContoller = std::make_unique<ColliderController>();
	for (int i = 0; i < AllObject.size();i++) {
		Prefab* Triangle = dynamic_cast<Prefab*>(AllObject[i]);
		AllObject[i]->InitializePlayMode();
		if (!Triangle)continue;
		for (auto& prop : Triangle->ObjProperties) {
			if (auto* rb = dynamic_cast<NVPhysx::RigidBody*>(prop)) {
				physScene->addActor(*rb->GetCurrentActor());
			}
		}
		//CContoller->AddTriangle(Triangle);
		//CContoller->InitalizePosition(Triangle);  //Call initalization of all object which want to initalize in before play mode
	}
	//CContoller->ConstructTree();
}

void Scene::DeleteObject(Objects* obj,bool isSmartPointer) {
	Prefab* t = dynamic_cast<Prefab*>(obj);
	if(t!=nullptr)
		globalCurrentOBJID.push_back(t->id); //local push back
	Objects::GlobalIdPool.push_back(obj->Id); //global push back
	obj->RemoveHeritence();
	obj->DeleteInheritence();
	AllObject.erase(LookUp(obj->Id, AllObject));
	if (t != nullptr) {
		for (auto& prop : t->ObjProperties) {
			if (dynamic_cast<NVPhysx::RigidBody*>(prop)) {
				auto p = dynamic_cast<NVPhysx::RigidBody*>(prop);
				if (p->DynamicActor != nullptr) {
					if (p->DynamicActor->getScene() != nullptr) {
						physScene->removeActor(*p->DynamicActor);
					}
				}
				if (p->StaticActor != nullptr) {
					if (p->StaticActor->getScene() != nullptr) {
						physScene->removeActor(*p->StaticActor);
					}
				}
				break;
			}
		}
		Triangles.erase(LookUp(t, Triangles));
	}
	//delete t;
	if (obj != nullptr) {
		if (!isSmartPointer) {
			delete obj;
		}
		obj = nullptr;
	}
	t = nullptr;
}
void Scene::DeInitalizePlayMode() {
	inPlayMode = false;
#ifdef WIN32_DEBUG
	for (int i = 0; i < DeleteAfterPlay.size(); i++) {
		Prefab* Triangle = dynamic_cast<Prefab*>(DeleteAfterPlay[i]);
		DeleteAfterPlay[i]->DeInitializePlayMode();
		if (!Triangle)continue;
		for (auto& prop : Triangle->ObjProperties) {
			if (auto* rb = dynamic_cast<NVPhysx::RigidBody*>(prop)) {
				physScene->removeActor(*rb->GetCurrentActor());
			}
		}
		DeleteObject(DeleteAfterPlay[i], true);
	}
	DeleteAfterPlay.clear();
#endif
	for (int i = 0; i < AllObject.size(); i++) {
		Prefab* Triangle = dynamic_cast<Prefab*>(AllObject[i]);
		AllObject[i]->DeInitializePlayMode();
		if (!Triangle)continue;
		for (auto& prop : Triangle->ObjProperties) {
			if (auto* rb = dynamic_cast<NVPhysx::RigidBody*>(prop)) {
				physScene->removeActor(*rb->GetCurrentActor());
			}
		}
	}
 
}
std::vector<Objects*>::iterator Scene::LookUp(short id, std::vector<Objects*>& vec) {
	for (auto it = vec.begin(); it != vec.end(); it++) {
		if ((*it)->Id == id)
			return it;
	}
	return vec.end();
}
std::vector<Prefab*>::iterator Scene::LookUp(Prefab* Tri, std::vector<Prefab*>& vec) {
	for (auto it = vec.begin(); it != vec.end(); it++) {
		if ((*it)->id == Tri->id)
			return it;
	}
	return vec.end();
}
//kill me
// way to fragile
void Scene::SceneThunk(void* TScene,void* pathForDeps,void* OutPtr) {
	Scene* sc = (Scene*)TScene;
	std::string* path = (std::string*)(pathForDeps);
	sc->FixRecursivePropDeps(path, OutPtr);
}
void Scene::FixRecursivePropDeps(void* CallerStruct,void* ptr) {
	void(*func)(void*, void*, void*) = Scene::SceneThunk;
	void** xtr = (void**)ptr;
	Serialization::CallerObject* data = (Serialization::CallerObject*)CallerStruct;
	std::string* path = (std::string*)data->Data;
	int off = path->find(":");
	int objectnum = std::stoi(path->substr(1, off-1)); //object number where the property should exist
	int closing = path->find(">");
	std::string classname = path->substr(off + 1, closing-3);
	//if properties belong to same object but created only linking pending
	//if properties belong to same object but not yet created
	//if properties belong to other object not yet created
	std::vector<Objects*> copyOfall(AllObject);
	std::sort(copyOfall.begin(), copyOfall.end(),
		[](const Objects* a, const Objects* b) {
		return a->Id < b->Id; 
	});
	auto it = std::lower_bound(copyOfall.begin(), copyOfall.end(), objectnum,
		[](const Objects* obj,int value) {
		return obj->Id<value;
	});
	//if the object already exist in list
	//should only go when the object property need other object property which is already exist
	if (it != copyOfall.end() && (*it)->Id != data->callerId) {
		Objects* foundObj = *it;
		bool found = false;
		for (auto& it : *foundObj->GetProperties()) {
			if (it->GetPropertyClassName() == classname) {
				RefrencePassing ref((void*)it, it->GetPropertyType());
				*xtr = (void*)&ref;
				found = true;
				break;
			}
		}
	}
	// we need to create new object or property exist in same object
	else {
		//object want property from same object
		if (data->callerId == objectnum) {
		//get the object
			bool found = false;
			Objects* foundObj = *it;

		//make properties and
		//look for property if it exist bind it
			for (auto& it : *foundObj->GetProperties()) {
				if (it->GetPropertyClassName() == classname) {
					RefrencePassing ref((void*)it, it->GetPropertyType());
					*xtr = (void*)&ref;
					found = true;
					break;
				}
			}
			if(found)
				return;
		//recursivly fix its dependency
		//this should only reach when property is in same object but not initalized yet?
		for (int i = 0; i < ObjectsBlocks->size(); i++) {
			if ((*ObjectsBlocks)[i].Id == objectnum) {
				auto OB = &(*ObjectsBlocks)[i];
				for (int j = 0; i < OB->propBlocks.size();j++) {
					if (OB->propBlocks[j].PropertyNames == classname) {
						Serialization::DeSerializeObject(foundObj, OB->propBlocks[j], func, this);
						auto jit = OB->propBlocks.begin() + j;
						OB->propBlocks.erase(jit);
						break;
					}
				}
				break;
			}
		}
		for (auto& it : *foundObj->GetProperties()) {
			if (it->GetPropertyClassName() == classname) {
				RefrencePassing ref((void*)it, it->GetPropertyType());
				*xtr = (void*)&ref;
				found = true;
				break;
			}
		}
		return;
		}
		//Transitive dependency
		else {
			///////////////////////////////////
			Serialization::ObjectBlocks* OB = nullptr;
			auto it = ObjectsBlocks->begin();
			for (int i = 0; i < ObjectsBlocks->size(); i++) {
				if ((*ObjectsBlocks)[i].Id == objectnum) {
					OB = &(*ObjectsBlocks)[i];
					it = it + i;
					break;
				}
			}
			if (OB != nullptr) {
				Objects* o = ParseObject(*OB);
				ObjectsBlocks->erase(it);

				//Made new Object should be fine
				for (auto& it : *o->GetProperties()) {
					if (it->GetPropertyClassName() == classname) {
						RefrencePassing ref((void*)it, it->GetPropertyType());
						*xtr = (void*)&ref;
						break;
					}
				}
			}
		}
	}
}
void Scene::LoadSkyBox() {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector <NormalPerObject> n;
	float color[3];
	indices.clear();
	vertices.clear();
	Primitives::BasicShapes::Circle::GetCircleofLongNLat(10, 10, vertices, indices);
	//Set position of sky box to Camera
	pSkyBox = std::make_unique<SkyBox>(pDumbDevice, pDumbContext, vertices, indices, currentOBJID++, color, ++Objects::count, n, &cam);
}
Objects* Scene::AddObject(Objects* ob,bool isRenderable) {
	short ObjectID;
	Objects* ret = nullptr;
	if (!Scene::globalCurrentOBJID.empty()) {
		ObjectID = Scene::globalCurrentOBJID[0];
		Scene::globalCurrentOBJID.erase(Scene::globalCurrentOBJID.begin());
	}
	else {
		ObjectID = Scene::currentOBJID++;
	}
	if (isRenderable) {
		Mesh* mesh = dynamic_cast<Mesh*>(ob);
		Prefab* obj = new Prefab(*mesh, ObjectID);
		Triangles.push_back(obj);
		AllObject.push_back(obj);
		ret = obj;
#ifdef WIN32_DEBUG
		if (inPlayMode) {
			DeleteAfterPlay.push_back(obj);
		}
#endif
	}
	else {
		AllObject.push_back(ob);
#ifdef WIN32_DEBUG
		if (inPlayMode) {
			DeleteAfterPlay.push_back(ob);
		}
#endif
	}
	return ret;
}
Objects* Scene::AddObject(Objects* ob, short LocalId,bool isRenderable) {
	Objects* ret = nullptr;
	if (isRenderable) {
		Mesh* mesh = dynamic_cast<Mesh*>(ob);
		Prefab* obj = new Prefab(*mesh, LocalId);
		Triangles.push_back(obj);
		AllObject.push_back(obj);
		ret = obj;
#ifdef WIN32_DEBUG
		if (inPlayMode) {
			DeleteAfterPlay.push_back(obj);
		}
#endif
	}
	else {
		AllObject.push_back(ob);
#ifdef WIN32_DEBUG
		if (inPlayMode) {
			DeleteAfterPlay.push_back(ob);
		}
#endif
	}
	return ret;
}
short Scene::GetCurrentID() {
	short globalID;
	if (!Objects::GlobalIdPool.empty()) {
		globalID = Objects::GlobalIdPool[0];
		Objects::GlobalIdPool.erase(Objects::GlobalIdPool.begin());
	}
	else {
		globalID = ++Objects::count;
	}
	return globalID;
}
Objects* Scene::FixInheritenceOfObjWhileParsing(Serialization::ObjectBlocks& OB) {
	if (OB.InheritedId != -1) {
		auto o = LookUp(OB.InheritedId, AllObject);
		if (o != AllObject.end())return *o;
		for (auto it = ObjectsBlocks->begin(); it != ObjectsBlocks->end();++it) {
			if (it->Id == OB.InheritedId) {
				auto obj = *it; 
				ObjectsBlocks->erase(it);
				return ParseObject(obj);  
			}
		}
	}
	return nullptr;
}
Objects* Scene::ParseObject(Serialization::ObjectBlocks& OB) {
	void(*func)(void*, void*, void*) = Scene::SceneThunk;
	if (OB.ClassName == "Camera") {
		Objects* inheritedObject = FixInheritenceOfObjWhileParsing(OB);
		auto it = LookUp(OB.Id, AllObject);
		if (inheritedObject) {
			(*it)->SetInheritence(inheritedObject);
		}
	}
	if (OB.ClassName == "Prefab") {
		std::istringstream stream(OB.blockBuffer);
		std::string line;
		std::getline(stream, line);
		std::getline(stream, line); //0
		auto path = line;
		int off = line.find(":");
		std::getline(stream, line);//1
		int off2 = line.find(":");

		PathToFile* ptf = new PathToFile(path.substr(off + 1, path.size() - 1), line.substr(off + 1, line.size() - 1));
		GetFile(ptf->Path);
		std::vector<Vertex> v;
		std::vector<unsigned int> indi;

		for (unsigned int i = 0; i < Vertexformated.size(); i++) {
			v.emplace_back(Vertexformated[i]);
			indi.push_back(i);
		}
		Objects* inheritedObject = FixInheritenceOfObjWhileParsing(OB);
		Mesh* newMesh = new Mesh(pDevice, pContext,OB.Id,v,indi,ptf);
		Prefab* newPrefab = (Prefab*)AddObject(newMesh, true);
		if (inheritedObject) {
			newPrefab->SetInheritence(inheritedObject);
		}
		for (int i = 0; i < OB.propBlocks.size();i++) {
			Serialization::DeSerializeObject(newPrefab,OB.propBlocks[i],func,this);
			OB.propBlocks.erase(OB.propBlocks.begin()+i);
		}
		dynamic_cast<Primitives::Material*>(newPrefab->ObjProperties[1])->CreateCBuffer(0, sizeof(CB::PerObjectData), Primitives::DOMAIN_SHADER);
		newPrefab->UpdateSubResource();
		return newPrefab;
	}
	if (OB.ClassName == "NObject") {
		NullObject* no = new NullObject(OB.Id);
		Objects* inheritedObject = FixInheritenceOfObjWhileParsing(OB);
		if (inheritedObject) {
			no->SetInheritence(inheritedObject);
		}
		AddObject(no, false);
		for (auto& it : OB.propBlocks) {
			Serialization::DeSerializeObject(no, it,func,this);
		}
		return no;
	}
	
	return nullptr;
}
void Scene::LoadScene() {
	CONSOLE_PRINT("Start");
	AllObject[0]->RemoveHeritence();
	while (AllObject.size()!=1) {
		auto it = AllObject.begin() + 1;
		DeleteObject(*it);
	}
	Serialization::MetaDataBlock MB = Serialization::LoadMetaData();
	Objects::count = MB.LastGlobalID;
	Objects::GlobalIdPool = MB.LeftOverIds;
	auto localBlock = Serialization::ReadFromFile();
	ObjectsBlocks = &localBlock;
	for (auto it = ObjectsBlocks->begin(); it != ObjectsBlocks->end(); ) {
		Objects* o = ParseObject(*it);
		/*if (o != nullptr)
			AllObject.push_back(o);*/
		it = ObjectsBlocks->erase(it); 
	}
	ObjectsBlocks->clear();
	ObjectsBlocks = nullptr;
	CONSOLE_PRINT("End");
}
void Scene::SaveScene() {
	std::string buffer;
	for (auto &it : AllObject) {
		buffer +=Serialization::SerializeObject(*it);
	}
	Serialization::SaveToFile(buffer);
	Serialization::MetaDataBlock MB;
	MB.LastGlobalID = Objects::count;
	MB.LeftOverIds = Objects::GlobalIdPool;

	Serialization::SaveMetaData(MB);
}