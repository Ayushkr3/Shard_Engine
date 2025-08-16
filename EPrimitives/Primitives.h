#pragma once
#include "EPrimitives.h"
#include "Global.h"
#include "WIC.h"
#include "Texture.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl.h>
#include <string>
#include <memory>
#include <map>
#include "Serialization.h"
#pragma comment(lib,"d3dcompiler.lib")
namespace Primitives {
	enum SHADER_CONSTANT_BUFFER_TYPE {
		DOMAIN_SHADER,PIXEL_SHADER
	};
	struct ConstantBufferData {
		
	};
	class ShaderConstantBuffer {
	public:
		PRIMITVE_API ShaderConstantBuffer() {};
		PRIMITVE_API ShaderConstantBuffer(ID3D11Device*& pID);
		Microsoft::WRL::ComPtr<ID3D11Buffer> CBuffer;
		virtual void bind(short index) {  };
	public:
		size_t size;
		std::unique_ptr<ConstantBufferData> Data;
	};
	class VertexShader {
		Microsoft::WRL::ComPtr<ID3D10Blob> pVSBlob;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pSReflector;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVS;
	public:
		std::unique_ptr<PathToFile> path;
		PRIMITVE_API VertexShader();
		PRIMITVE_API void CreateShader(ID3D11Device*& pIDevice);
		PRIMITVE_API void AssignShader(ID3D11VertexShader* VertexShader, Microsoft::WRL::ComPtr<ID3D10Blob>& ppBlob);
		PRIMITVE_API void BindShaderToPipeLine(ID3D11DeviceContext*& pIContext);
		PRIMITVE_API Microsoft::WRL::ComPtr<ID3D10Blob> GetBlob();
		PRIMITVE_API void ReleaseRef();
	};
	class PixelShader {
		Microsoft::WRL::ComPtr<ID3D10Blob> pPSBlob;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPS;
	public:
		ShaderConstantBuffer CB;
		std::vector<std::unique_ptr<Texture>> Textures; // list of all Textures need for shader
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pSReflector;
		std::unique_ptr<PathToFile> path;
		struct DefualtPixelShaderCB:public ConstantBufferData //Default PixelShader Constant Buffer
		{
			float rgba[4] = {0.0f,0.0f,1.0f,1.0f};
		};
		DefualtPixelShaderCB PixelShaderData;
		PRIMITVE_API PixelShader();
		//This will be called from UI
		PRIMITVE_API void CreateShader(ID3D11Device*& pIDevice,ID3D11DeviceContext*& pIContext);
		//This will be called from script if someone wants to call it
		PRIMITVE_API void CreateShader(std::unique_ptr<PathToFile>& ptf,ID3D11Device*& pIDevice, ID3D11DeviceContext*& pIContext);
		PRIMITVE_API void AssignShader(ID3D11PixelShader* PixelShader, Microsoft::WRL::ComPtr<ID3D10Blob>& ppBlob);
		PRIMITVE_API void BindShaderToPipeLine(ID3D11DeviceContext*& pIContext);
		PRIMITVE_API void SetAssociatedTexture(short Index,std::unique_ptr<PathToFile>& ptf);
		PRIMITVE_API void ReleaseRef();
	};
	class GeometryShader {
		Microsoft::WRL::ComPtr<ID3D10Blob> pGSBlob;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGS;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pSReflector;
		std::unordered_map<short,std::unique_ptr<ConstantBufferData>> buffersAssociated;
	public:
		//Create a List associated Constant buffer with each shader
		std::unique_ptr<PathToFile> path;
		//Called every frame to update buffer
		PRIMITVE_API void BindAssociatedBuffer(ConstantBufferData*& Data,short index);
		PRIMITVE_API GeometryShader();
		PRIMITVE_API void CreateShader(ID3D11Device*& pIDevice);
		PRIMITVE_API void AssignShader(ID3D11GeometryShader* GeometryShader, Microsoft::WRL::ComPtr<ID3D10Blob> ppBlob);
		PRIMITVE_API void BindShaderToPipeLine(ID3D11DeviceContext*& pIContext);
		PRIMITVE_API void ReleaseRef();
	};
	class HullShader {
		Microsoft::WRL::ComPtr<ID3D10Blob> pHSBlob;
		Microsoft::WRL::ComPtr<ID3D11HullShader> pHS;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pSReflector;
	public:
		std::unique_ptr<PathToFile> path;
		PRIMITVE_API void CreateShader(ID3D11Device*& pIDevice);
		PRIMITVE_API void AssignShader(ID3D11HullShader* HullShader, Microsoft::WRL::ComPtr<ID3D10Blob>& ppBlob);
		PRIMITVE_API void BindShaderToPipeLine(ID3D11DeviceContext*& pIContext);
		PRIMITVE_API void ReleaseRef();
		PRIMITVE_API HullShader();
	};
	class DomainShader {
		Microsoft::WRL::ComPtr<ID3D10Blob> pDSBlob;
		Microsoft::WRL::ComPtr<ID3D11DomainShader> pDS;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pSReflector;
		std::unordered_map<short, std::unique_ptr<ShaderConstantBuffer>> buffersAssociated;
	public:
		std::unique_ptr<PathToFile> path;
		PRIMITVE_API DomainShader();
		//Called every frame to update buffer
		PRIMITVE_API void BindAssociatedBuffer(void* Data, short index,ID3D11DeviceContext *pIC);
		PRIMITVE_API void CreateShader(ID3D11Device*& pIDevice);
		PRIMITVE_API void CreateShader(std::unique_ptr<PathToFile>& ptf, ID3D11Device*& pIDevice, ID3D11DeviceContext*& pIContext);
		PRIMITVE_API std::unique_ptr<ShaderConstantBuffer>& GetBuffer(short index);
		PRIMITVE_API void AssignShader(ID3D11DomainShader* DomainShader, Microsoft::WRL::ComPtr<ID3D10Blob>& ppBlob);
		PRIMITVE_API void BindShaderToPipeLine(ID3D11DeviceContext*& pIContext);
		PRIMITVE_API void ReleaseRef();
	};
	class ConstantBuffer {
	private:
		PixelShader* Ps;
		//Microsoft::WRL::ComPtr<ID3D11Buffer> CBuffer;
	public:
		PRIMITVE_API Microsoft::WRL::ComPtr<ID3D11Buffer>& CreateCBuffer(ID3D11Device*& pID,size_t size,ConstantBufferData*& Data);
		PRIMITVE_API ConstantBuffer(PixelShader* PS, ID3D11Device*& pID);
		PRIMITVE_API static void UpdateSubResource( void* Data, size_t sizeofData, Microsoft::WRL::ComPtr<ID3D11Buffer>& Buffer,ID3D11DeviceContext*& pIC);
		PRIMITVE_API void UpdatePS(ID3D11DeviceContext*& pIc);   //Update PixelShader
	};
	class Material :public ObjectProperties {
	private:
		//------------------------------------------------------------------//
		//-----------------------Defualt Shader-----------------------------//
		//------------------------------------------------------------------//
		static Microsoft::WRL::ComPtr<ID3D10Blob> pDVSBlob; // Default blob
		static Microsoft::WRL::ComPtr<ID3D10Blob> pDPSBlob; // Default blob
		static Microsoft::WRL::ComPtr<ID3D10Blob> pDHSBlob; // Default blob
		static Microsoft::WRL::ComPtr<ID3D10Blob> pDDSBlob; // Default blob
		static Microsoft::WRL::ComPtr<ID3D10Blob> pDGSBlob; // Default blob
		static ID3D11VertexShader* pDVS; //Default Shader
		static ID3D11PixelShader* pDPS; //Default Shader
		static ID3D11HullShader* pDHS; //Default Shader
		static ID3D11DomainShader* pDDS; //Default Shader
		static ID3D11GeometryShader* pDGS; //Default Shader

		static ID3D11Device* pIDevice;   //Internal Device
		static ID3D11DeviceContext* pIContext; //Internal Context
		//---------------------------------------------------------------------//
	private:
		std::unique_ptr<ConstantBuffer> pCB;
	public:
		static ImGuiContext* privateCtx;
		PRIMITVE_API void CreateCBuffer(short index, size_t size, SHADER_CONSTANT_BUFFER_TYPE shadertype);
		PRIMITVE_API static void SetImguiCtx(ImGuiContext* Ctx);
		VertexShader vs;
		PixelShader ps;
		HullShader hs;
		DomainShader ds;
		GeometryShader gs;
		REFLECT_BEGIN_ADDR(Material)
			REFLECT_ADDR(vs.path->Path),
			REFLECT_ADDR(vs.path->FileName),
			REFLECT_ADDR(ps.path->Path),
			REFLECT_ADDR(ps.path->FileName),
			REFLECT_ADDR(hs.path->Path),
			REFLECT_ADDR(hs.path->FileName),
			REFLECT_ADDR(ds.path->Path),
			REFLECT_ADDR(ds.path->FileName),
			REFLECT_ADDR(gs.path->Path),
			REFLECT_ADDR(gs.path->FileName)
		REFLECT_END_ADDR()
		PRIMITVE_API static void InitalizeD3D11Factory(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext);
		PRIMITVE_API static void DeInitalizeD3D11Factory();
		PRIMITVE_API void show();
		PRIMITVE_API void UpdateDependency(const void* ptr);
		PRIMITVE_API Material(Objects* obj);
		PRIMITVE_API void BindAllShaders();
		PRIMITVE_API ObjectProperties* GetPropertyRef();
		PRIMITVE_API const std::type_info& GetPropertyType();
		PRIMITVE_API std::string Serialize();
		PRIMITVE_API std::string GetPropertyClassName() { return "Material"; }
		PRIMITVE_API void DeSerialize(std::string block);
		PRIMITVE_API static void RegisterFactory(std::multimap<std::string, std::function<ObjectProperties*(Objects*)>>& GlobalPropertiesPoolL = ObjectProperties::GlobalPropertiesPool);
	};
}
PRIMITVE_API ObjectProperties* CreateMaterial(Objects* obj);