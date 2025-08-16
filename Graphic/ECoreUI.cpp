#include "ECoreUI.h"
#define CHECK_ERROR(hr) if(FAILED(hr)) throw error::error(hr,__LINE__)
Microsoft::WRL::ComPtr<ID3D11Device> UIElements::pUIDevice = nullptr;
ImGuiContext* UIElements::ctx = nullptr;
Microsoft::WRL::ComPtr<ID3D11DeviceContext> UIElements::pUIContext= nullptr;
Microsoft::WRL::ComPtr<IDXGISwapChain>      UIElements::pUISwapChain= nullptr;
Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  UIElements::pUIRenderTarget= nullptr;
DXGI_SWAP_CHAIN_DESC UIElements::UIscd;
ImGuiIO* UIElements::io = nullptr;
namespace  fs=std::filesystem ;
Scene* SceneManager::currentScene = nullptr;
bool PropertiesWindow::closable = true;
UIWindows::UIWindows(std::string className, HWND Phwnd, HINSTANCE hint,short x ,short y,short w,short b,int windowsN) {
	cHwnd = CreateWindowEx(
		0,                          // Optional window styles.
		className.c_str(),                     // Window class
		std::to_string(windowsN).c_str(),				  // Window text
		WS_CHILD | WS_VISIBLE|WS_BORDER,     // Window style

							   // Size and position
		x, y, w,b,

		Phwnd,				// Parent window    
		(HMENU)windowsN,			   // Menu
		hint,			  // Instance handle
		NULL			 // Additional application data
	);
}
 void UIElements::ContainerStart()
{
	if (Globals::isFullscreen) {
		ImGui::Begin("Container",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SetWindowFontScale(0.7f);
	}
}
void UIElements::ContainerEnd() {
	if (Globals::isFullscreen) {
		ImGui::End();
	}
}
UIElements::UIElements(std::string className, HWND Phwnd, HINSTANCE hint, short x, short y, short w, short b, int windowsN) {
	if (pUIDevice == nullptr) {
		ctx = (ImGui::CreateContext());
		UIElements::io = &ImGui::GetIO();
		((HotReloading::SharedData*)HotReloading::ptrToMem)->ctx = ctx;
		io->FontGlobalScale = 1.4f;
		io->WantCaptureMouse = true;
		io->WantCaptureKeyboard = true;
		io->WantTextInput = true;
		io->Fonts->AddFontDefault();
		static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 13.0f;
		io->Fonts->AddFontFromFileTTF("D:/program/vs/graphic/Graphic/font/fontawesome-webfont.ttf",13.0f,&config, icon_ranges);
		io->ConfigFlags = ImGuiConfigFlags_NavEnableSetMousePos;
		cHwnd = CreateWindowEx(
			0,                          // Optional window styles.
			className.c_str(),                     // Window class
			std::to_string(windowsN).c_str(),				  // Window text
			WS_CHILD |WS_VISIBLE |WS_CLIPSIBLINGS | WS_BORDER,     // Window style

								   // Size and position
			x, y, w, b,

			Phwnd,				// Parent window    
			(HMENU)windowsN,			   // Menu
			hint,			  // Instance handle
			NULL			 // Additional application data
		);
		UIscd.BufferCount = 1;
		UIscd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		UIscd.BufferDesc.Height = 0;
		UIscd.BufferDesc.Width = 0;
		UIscd.BufferDesc.RefreshRate.Numerator = 0;
		UIscd.BufferDesc.RefreshRate.Denominator = 0;
		UIscd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		UIscd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		UIscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		UIscd.Flags = 0;
		UIscd.OutputWindow = cHwnd;
		UIscd.SampleDesc.Count = 1;
		UIscd.SampleDesc.Quality = 0;
		UIscd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		UIscd.Windowed = true;
		CHECK_ERROR(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, &UIscd, &pUISwapChain, &pUIDevice, nullptr, &pUIContext));
		ID3D11Resource* pBackBuffer = nullptr;
		CHECK_ERROR(pUISwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));
		CHECK_ERROR(pUIDevice->CreateRenderTargetView(pBackBuffer, nullptr, pUIRenderTarget.GetAddressOf()));
		pBackBuffer->Release();
		ImGui_ImplWin32_Init(cHwnd);
		ImGui_ImplDX11_Init(pUIDevice.Get(), pUIContext.Get());
		ImguiContextFactory::Init(pUIDevice, pUIContext, ctx);
	}
}
void UIWindows::SetFullScreen(LPRECT rect) {
	SetWindowLongPtr(cHwnd, GWL_STYLE, WS_CHILD | WS_VISIBLE | WS_BORDER| WS_CLIPSIBLINGS);
	SetWindowPos(cHwnd, HWND_TOP, 0, 0, rect->right-rect->left, rect->bottom-rect->top, SWP_SHOWWINDOW);
}
void UIElements::UpdateUI() {
	if (Globals::isFullscreen) { return; }
		float rgba[4] = { 0.0f,0.0f,0.0f,1.0f };
		pUIContext->OMSetRenderTargets(1, pUIRenderTarget.GetAddressOf(), nullptr);
		pUIContext->ClearRenderTargetView(pUIRenderTarget.Get(), rgba);
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
}
UIElements::~UIElements() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
void UIElements::Swap() {
	if (Globals::isFullscreen) return;
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		pUISwapChain->Present(1u, 0);
}
void UIElements::SetSizenWidth(int w, int b)
{
	DXGI_MODE_DESC modeDesc;
	modeDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	modeDesc.Height = b-1;
	modeDesc.Width = w-1;
	modeDesc.RefreshRate.Numerator = 0;
	modeDesc.RefreshRate.Denominator = 0;
	modeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	ID3D11Resource* pBackBuffer = nullptr;
	pUISwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	pBackBuffer->Release();
	pUIRenderTarget->Release();
	pUISwapChain->ResizeBuffers(0, w - 1, b - 1, DXGI_FORMAT_B8G8R8A8_UNORM,0);
	pUISwapChain->ResizeTarget(&modeDesc);
	pUIDevice->CreateRenderTargetView(pBackBuffer, nullptr, pUIRenderTarget.GetAddressOf());
	pUIContext->OMSetRenderTargets(1, pUIRenderTarget.GetAddressOf(), nullptr);
	io->DisplaySize = ImVec2(static_cast<float>(w - 1), static_cast<float>(b - 1));
}
SceneManager::SceneManager(int posX, int posY, int widthX, int widthY):posX(posX),posY(posY),widthX(widthX),widthY(widthY)
{
}
void SceneManager::SetSizenWidth() {
	ImGui::Begin("Scene Objects");
	ImGui::SetWindowFontScale(0.7f);
	ImGui::SetWindowSize(ImVec2((float)widthX, (float)widthY));
	ImGui::SetWindowPos(ImVec2((float)posX, (float)posY));
	ImGui::End();
}
std::vector<Objects*>::iterator SceneManager::LookUp(short id, std::vector<Objects*>& vec ) {
	for (auto it = currentScene->AllObject.begin();it!= currentScene->AllObject.end(); it++) {
		if ((*it)->Id == id)
			return it;
	}
	return currentScene->AllObject.end();
}
std::vector<Prefab*>::iterator SceneManager::LookUp(Prefab* Tri, std::vector<Prefab*>& vec) {
	for (auto it = currentScene->Triangles.begin(); it != currentScene->Triangles.end(); it++) {
		if ((*it)->id == Tri->id)
			return it;
	}
	return currentScene->Triangles.end();
}
void SceneManager::RecursiveTree(Objects*& obj,int* selected) {
	if (obj->Inheritence.InheritedObj.empty()) {
		std::string selectableLabel = obj->ObjName + std::to_string(obj->Id);
		if (ImGui::Selectable(selectableLabel.c_str(), *selected == obj->Id)) {
			PropertiesWindow::Obj = obj;
			PropertiesWindow::closable = true;
			*selected = obj->Id;
		}
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
			ImGui::SetDragDropPayload("Objects", &obj, sizeof(obj));
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Objects")) {
				Objects* data = *static_cast<Objects**>(payload->Data);
				data->RemoveHeritence();
				data->SetInheritence(obj);
			}
			ImGui::EndDragDropTarget();
		}
		return;
	}
	else {
   		std::string selectableLabel = obj->ObjName + std::to_string(obj->Id);
		if(ImGui::TreeNodeEx(selectableLabel.c_str()/*,selected == obj->Id ? ImGuiTreeNodeFlags_Selected : 0*/)) {
			/*if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
				PropertiesWindow::Obj = obj;
				PropertiesWindow::closable = true;
				*selected = obj->Id;
			}*/
			if ((ImGui::IsItemToggledOpen()) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
				PropertiesWindow::Obj = obj;
				PropertiesWindow::closable = true;
				*selected = obj->Id;
			}
			else {

			}
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
				ImGui::SetDragDropPayload("Objects", &obj, sizeof(obj));
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Objects")) {
					Objects* data = *static_cast<Objects**>(payload->Data);
					data->RemoveHeritence();
					data->SetInheritence(obj);
				}
				ImGui::EndDragDropTarget();
			}
			for (int i = 0; i < obj->Inheritence.InheritedObj.size(); i++) {
				RecursiveTree(obj->Inheritence.InheritedObj[i],selected);
			}
			ImGui::TreePop();
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Objects")) {
				Objects* data = *static_cast<Objects**>(payload->Data);
				data->RemoveHeritence();
				data->SetInheritence(obj);
			}
			ImGui::EndDragDropTarget();
		}
	}
}
void SceneManager::Content() {
	if (currentScene == nullptr)
		return;
	static int selected = -1;
	if (!Globals::isFullscreen) {
		ImGui::Begin("Scene Objects", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	}
	else {
		if (!ImGui::CollapsingHeader("Scene Objects", nullptr))return;
	}
	if (ImGui::BeginPopupContextItem("Scene Menu")) {
		if (ImGui::BeginMenu("Add Object")) {
			ImGui::SetWindowFontScale(0.7f);
			if (ImGui::MenuItem("Null Object")) {
				NullObject* no = new NullObject(Scene::GetCurrentID());
				SceneManager::currentScene->AddObject(no, false);
			}
			ImGui::EndMenu();
			
		}
		ImGui::EndPopup();
	}
	static Objects* last_object = nullptr;
	for (int i = 0; i < currentScene->AllObject.size();i++) {
		if (currentScene->AllObject[i]->Inheritence.inheritedFrom != nullptr)
			continue;
		RecursiveTree(currentScene->AllObject[i], &selected);
		std::string selectableLabel = currentScene->AllObject[i]->ObjName + std::to_string(currentScene->AllObject[i]->Id);
		/*if (ImGui::Selectable(selectableLabel.c_str(), selected == currentScene->AllObject[i]->Id)) {
				PropertiesWindow::Obj = currentScene->AllObject[i];
				PropertiesWindow::closable = true;
				selected = currentScene->AllObject[i]->Id;
				currentScene->AllObject[i]->Highlight();
				if (last_object != currentScene->AllObject[i]) {
					if (last_object != nullptr)
						last_object->Restore();
					last_object = currentScene->AllObject[i];
				}
		}*/
		/*if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
			ImGui::SetDragDropPayload("Objects",&currentScene->AllObject[i],sizeof(currentScene->AllObject[i]));
			ImGui::EndDragDropSource();
		}*/
		/*if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Objects")) {
				Objects* data = *static_cast<Objects**>(payload->Data);
				currentScene->AllObject[i]->Inheritence.InheritedObj.push_back(data);
				data->SetInheritence(currentScene->AllObject[i]);
				ImGui::EndDragDropTarget();
			}
		}*/
		if (ImGui::BeginPopupContextItem(std::to_string(currentScene->AllObject[i]->Id).c_str())) {
			if (ImGui::Button("Delete")) {
				Prefab* t = dynamic_cast<Prefab*>(currentScene->AllObject[i]);
				if (Globals::inPlayMode) {}
					//currentScene->CContoller->deleteObject(t);
				if (last_object == t)
					last_object = nullptr;
				currentScene->DeleteObject(currentScene->AllObject[i]);
				
				//ImGui::CloseCurrentPopup();
				PropertiesWindow::Obj = nullptr;
			}
			ImGui::EndPopup();
		}
	}
	ImGui::Dummy(ImGui::GetContentRegionAvail());
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Objects")) {
			Objects* data = *static_cast<Objects**>(payload->Data);
			data->RemoveHeritence();
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
		ImGui::OpenPopup("Scene Menu");
	if (!Globals::isFullscreen) {
		ImGui::End();
	}
}
Objects* PropertiesWindow::Obj = nullptr;
PropertiesWindow::PropertiesWindow(int posX, int posY, int widthX, int widthY) :posX(posX), posY(posY), widthX(widthX), widthY(widthY)
{
}
void PropertiesWindow::SetSizenWidth() {
	ImGui::Begin("Properties",&closable);
	ImGui::SetWindowFontScale(0.7f);
	ImGui::SetWindowSize(ImVec2((float)widthX, (float)widthY));
	ImGui::SetWindowPos(ImVec2((float)posX, (float)posY));
	ImGui::End();
}
void PropertiesWindow::Content() {
	if (!Globals::isFullscreen) {
		ImGui::Begin("Properties", &closable, 0/*ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize*/);
	}
	else {
		if (closable) {
			ImGui::Begin("Properties", &closable, ImGuiWindowFlags_AlwaysAutoResize);
			if (closable == false) {
				ImGui::End();
				return;
			}
		}
		else {
			return;
		}
	}
	if (PropertiesWindow::Obj != nullptr) {
		for (auto& obj : *PropertiesWindow::Obj->GetProperties()) {
			obj->show();
		}
		if (ImGui::BeginPopupContextWindow("Change Properties", 1)) {
			if (ImGui::BeginMenu("Add Properties"))
			{
				ImGui::SetWindowFontScale(0.7f);
				for (auto& x : ObjectProperties::GlobalPropertiesPool) {
					if (ImGui::MenuItem(x.first.c_str()))
					{
						ObjectProperties* newProp = x.second(PropertiesWindow::Obj);
						bool found = false;
						for (auto& y : *PropertiesWindow::Obj->GetProperties()) {
							if (y->GetPropertyType() == newProp->GetPropertyType()) {
								found = true;
								break;
							}
						}
						found?delete newProp: PropertiesWindow::Obj->GetProperties()->push_back(newProp);
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
			ImGui::OpenPopup("Change Properties");
	}
	ImGui::Text((std::string(1,InputManager::GetLastKeyPress())).c_str());
	ImGui::End();
	return;
}

void Files::SetSizenWidth()
{
	ImGui::Begin("Files");
	ImGui::SetWindowFontScale(0.7f);
	ImGui::SetWindowSize(ImVec2((float)widthX, (float)widthY));
	ImGui::SetWindowPos(ImVec2((float)posX, (float)posY));
	ImGui::End();
}

Files::Files(int posX, int posY, int widthX, int widthY ,Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext): posX(posX), posY(posY), widthX(widthX), widthY(widthY),pDevice(pDevice),pContext(pContext)
{
	
}

void Files::Content()
{
	if (!Globals::isFullscreen) {
		ImGui::Begin("Files", nullptr);
	}
	else {
		if(!ImGui::CollapsingHeader("Files",0))return;
	}
	static fs::directory_entry fileSelected;
	if (working != WorkingDirectory && ImGui::Selectable(ICON_FA_FOLDER_O " ../")) {
		working = working.parent_path();
	}
	for (auto& it : fs::directory_iterator(working)) {
		if (it.is_directory()) {
			if (ImGui::Selectable((ICON_FA_FOLDER_O" "+ it.path().filename().string()).c_str(), fileSelected == it)) {
				fileSelected = it;
			}
		}
		else if (it.is_regular_file()) {
			if (ImGui::Selectable((ICON_FA_FILE_O" " + it.path().filename().string()).c_str(), fileSelected == it)) {
				fileSelected = it;
			}
			if (it.path().extension() == ".cso") {
				if (ImGui::BeginDragDropSource()) {
					std::unique_ptr<PathToFile> pf = std::make_unique<PathToFile>();
					pf.get()->Path = it.path().generic_string();
					pf.get()->FileName = it.path().filename().generic_string();
					std::unique_ptr<RefrencePassing> ref = std::make_unique<RefrencePassing>((void*)pf.get(), typeid(PathToFile));
					ImGui::SetDragDropPayload("Shader", ref.get(), sizeof(RefrencePassing));
					pf.release();
					ImGui::EndDragDropSource();
				}
			}
			else if (it.path().extension() == ".jpg"|| it.path().extension() ==".jpeg"|| it.path().extension() == ".dds") {
				if (ImGui::BeginDragDropSource()) {
					std::unique_ptr<PathToFile> pf = std::make_unique<PathToFile>();
					pf.get()->Path = it.path().generic_string();
					pf.get()->FileName = it.path().filename().generic_string();
					std::unique_ptr<RefrencePassing> ref = std::make_unique<RefrencePassing>((void*)pf.get(), typeid(PathToFile));
					ImGui::SetDragDropPayload("Texture", ref.get(), sizeof(RefrencePassing));
					pf.release();
					ImGui::EndDragDropSource();
				}
			}
			if (ImGui::BeginPopupContextItem(it.path().string().c_str())) {
				if (ImGui::Button("Add to scene")) {
					GetFile(it.path().string());
					std::vector<Vertex> newVert;
					std::vector<unsigned int> indi;
					std::vector<NormalPerObject> n;
					for (unsigned int i = 0; i < Vertexformated.size(); i++) {
						newVert.emplace_back(Vertexformated[i]);
						indi.push_back(i);
						if (i < ObjNormals.size())
							n.emplace_back(ObjNormals[i][0], ObjNormals[i][1], ObjNormals[i][2]);
					}
					PathToFile* ptf = new PathToFile(it.path().string(), it.path().filename().string());
					Mesh newMesh(pDevice, pContext, Scene::GetCurrentID() , newVert, indi,ptf);
					SceneManager::currentScene->AddObject(&newMesh);
					
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
	}
	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
		if (fileSelected.is_directory()) {
			working = fileSelected;
		}
	}
	if (!Globals::isFullscreen) {
		ImGui::End();
	}
}
ControlMenu::ControlMenu(int posX, int posY, int widthX, int widthY, std::unique_ptr<UIElements>& UIWindow, std::unique_ptr<UIWindows>& win, std::unique_ptr<Graphic>& pgfx) :posX(posX), posY(posY), widthX(widthX), widthY(widthY),win(win),UIWindow(UIWindow),pgfx(pgfx)
{
}
void ControlMenu::SetSizenWidth()
{
	ImGui::Begin("Control");
	ImGui::SetWindowFontScale(0.7f);
	ImGui::SetWindowSize(ImVec2((float)widthX, (float)widthY));
	ImGui::SetWindowPos(ImVec2((float)posX, (float)posY));
	ImGui::End();
}
void ControlMenu::Content() {
	if (!Globals::isFullscreen) {
		ImGui::Begin("Control", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	}
	else {
		ImGui::Begin("Control", nullptr,ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
	}
	
	if (ImGui::Button(ICON_FA_PLAY"Play/Pause")) {
		*Globals::inPlayMode = !(*Globals::inPlayMode);
		if (*Globals::inPlayMode) {
			SceneManager::currentScene->InitalizePlayMode();
		}
		else {
			SceneManager::currentScene->DeInitalizePlayMode();
			DebugConsole::Log("Exited PlayMode");
		}
	}
	ImGui::SameLine();
	ImGui::Text(("inPlayMode" + std::to_string(*Globals::inPlayMode)).c_str());
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_REPEAT"Reload Module")) {
		if(ObjectPropertiesFactory::Userlib==nullptr)
		ObjectPropertiesFactory::Userlib = LoadLibrary("Module_1.dll");
		if (ObjectPropertiesFactory::Userlib == nullptr)
			DebugConsole::Log("User module not loaded");
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_REPEAT"Discard Module")) {
		if (ObjectPropertiesFactory::Userlib != nullptr)
		FreeLibrary(ObjectPropertiesFactory::Userlib);
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_FLOPPY_O"Save Scene")) {
		SceneManager::currentScene->SaveScene();
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_REPEAT"Load Scene")) {
		PropertiesWindow::Obj = nullptr;
		SceneManager::currentScene->LoadScene();
	}
	ImGui::SameLine();
	if (ImGui::Button("FullScreen")) {
		RECT rect;
		Globals::isFullscreen = !Globals::isFullscreen;
		GetWindowRect(UIWindow->cHwnd,&rect);
		SetWindowLongPtr(UIWindow->cHwnd,GWL_STYLE,WS_CHILD| WS_BORDER);
		SetWindowPos(UIWindow->cHwnd, HWND_BOTTOM, 0, 0,0, 0, SWP_HIDEWINDOW);
		UIWindow->~UIElements();
		auto ctx = ImGui::CreateContext();
		auto io = &ImGui::GetIO();
		io->FontGlobalScale = 1.4f;
		io->WantCaptureMouse = true;
		io->WantCaptureKeyboard = true;
		io->WantTextInput = true;
		io->Fonts->AddFontDefault();
		static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 13.0f;
		io->Fonts->AddFontFromFileTTF("D:/program/vs/graphic/Graphic/font/fontawesome-webfont.ttf", 13.0f, &config, icon_ranges);
		io->ConfigFlags = ImGuiConfigFlags_NavEnableSetMousePos;
		((IPC::SharedData*)IPC::ptrToMem)->ctx = ctx;
		ImGui::SetCurrentContext(ctx);
		ImGui_ImplWin32_Init(win->cHwnd);
		ImGui_ImplDX11_Init(pgfx->pDevice.Get(), pgfx->pContext.Get());
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Container");
		ImGui::SetWindowFontScale(0.7f);
		ImGui::End();
		ImGui::Begin("Properties",&PropertiesWindow::closable);
		ImGui::SetWindowFontScale(0.7f);
		ImGui::End();
		ImGui::Begin("Control");
		ImGui::SetWindowFontScale(0.7f);
		ImGui::End();
		ImGui::EndFrame();
		win->SetFullScreen(&rect);
		pgfx->Resize(rect.right - rect.left, rect.bottom - rect.top);
		float ratio = (((float)(rect.right - rect.left)) / (float)((rect.bottom - rect.top)));
		pgfx->pSc->cam.SetFOVnAspectRatio(60, ratio);
		return;
	}
	ImGui::End();
}
//----------------------------------------------------------------------//
void ControlMenu::SetSizenWidth(UINT width, UINT height) {
	ImGui::Begin("Control");
	ImGui::SetWindowFontScale(0.7f);
	ImGui::SetWindowSize(ImVec2((float)width, (float)height));
	ImGui::SetWindowPos(ImVec2((float)posX, (float)posY));
	ImGui::End();
}

void SceneManager::SetSizenWidth(UINT width, UINT height) {
	ImGui::Begin("Scene Objects");
	ImGui::SetWindowFontScale(0.7f);
	ImGui::SetWindowSize(ImVec2((float)width, (float)height));
	ImGui::SetWindowPos(ImVec2((float)posX, (float)posY));
	ImGui::End();
}
void Files::SetSizenWidth(int width, int height) {
	ImGui::SetCurrentContext(UIElements::ctx);
	ImGui::NewFrame();
	ImGui::Begin("Files");
	ImGui::SetWindowFontScale(0.7f);
	/*posX = posX + width;
	widthX = widthX + width;
	widthY = widthY + height;*/
	ImGui::SetWindowSize(ImVec2(widthX, widthY));
	ImGui::SetWindowPos(ImVec2((float)posX, (float)posY));
	ImGui::End();
	ImGui::EndFrame();
}
void PropertiesWindow::SetSizenWidth(UINT width, UINT height) {
	ImGui::SetCurrentContext(UIElements::ctx);
	ImGui::NewFrame();
	ImGui::Begin("Properties",&closable);
	ImGui::SetWindowFontScale(0.7f);
	//posX += width / 2;
	/*widthX = widthX + width;
	widthY = widthY + height;*/
	ImGui::SetWindowSize(ImVec2(widthX, widthY));
	ImGui::SetWindowPos(ImVec2((float)posX, (float)posY));
	ImGui::End();
	ImGui::EndFrame();
}
//----------------------------------------------------------------------//