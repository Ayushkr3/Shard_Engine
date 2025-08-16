#pragma once
#include <d3d11.h>
#include "Global.h"
#include "ECore.h"
#include "UIProperties.h"
namespace UI {
	CORE_EXP void InitalizeUI(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext);
	CORE_EXP void RenderUI(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext);
	CORE_EXP void Resize(short sizeX, short sizeY, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext);
	struct UIVertex {
		struct {
			float x;
			float y;
			float z;
		}position;
		struct {
			float u;
			float v;
		}UV;
		constexpr UIVertex(float x, float y, float z, float u, float v) :position{x,y,z},UV{u, v} {};
	};
	class BaseUI {
	protected:
		static std::vector<UI::InstanceData> privateDat;
		short InterID; //Internal UI id
	public:
		UI::UIContainer Container;
	public:
		static constexpr std::array<uint16_t, 6> indices = { 0, 2,1, 2,3,0 };
		static constexpr std::array<UIVertex,6> enclosingQuad = { 
			UIVertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f),
			UIVertex(0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
			UIVertex(0.5f,  0.5f, 0.0f, 1.0f, 1.0f),
			UIVertex(0.5f,  0.5f, 0.0f, 1.0f, 1.0f),
			UIVertex(-0.5f,  0.5f, 0.0f, 0.0f, 1.0f),
			UIVertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f)
		};
		CORE_EXP virtual void Action();
		CORE_EXP BaseUI(Objects* obj);
		CORE_EXP ~BaseUI();
		static void ResetInstanceBuffer();
		static Microsoft::WRL::ComPtr<ID3D10Blob> pVSBlob;
		static Microsoft::WRL::ComPtr<ID3D10Blob> pPSBlob;
		static Microsoft::WRL::ComPtr<ID3D11InputLayout> pIL;
		static Microsoft::WRL::ComPtr<ID3D11VertexShader>pVS;
		static Microsoft::WRL::ComPtr<ID3D11PixelShader>pPS;
		static Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuff;
		static Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuff;
		static Microsoft::WRL::ComPtr<ID3D11Buffer> orthoBuffer; //Constant buffer
		static Microsoft::WRL::ComPtr<ID3D11Buffer> UIInstance; //Instance Buffer
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
		static Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
		static DirectX::XMMATRIX ortho;
		static std::unordered_map<int,UI::InstanceData*>AllInstanceData;
		static short sizeX;
		static short sizeY;
		static short Count;
		static std::queue<short> availableIndexs;
		static short GetNewIndex();
		CORE_EXP void UpdatePortion(short ID,void* DataToReplaceWith);
	};
	class Button : public Objects, public BaseUI {
	public:
		//CORE_EXP void Action();
		CORE_EXP Button(short id);
		std::vector<ObjectProperties*> properties;
		CORE_EXP std::vector<ObjectProperties*>* GetProperties();
		CORE_EXP void Update();
		CORE_EXP ~Button();
	};
};