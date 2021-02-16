#pragma once
#include "framework.h"
#include "Vector3D.h"

HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);
HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);



//#include <d3d11.h>
////#include <d3dcompiler.h>
//#include <DirectXMath.h>

using namespace DirectX;

class Shader
{
public:

protected:
	
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		VS = nullptr; // 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		PS = nullptr; // ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	GS = nullptr; // ジオメトリシェーダ
	Microsoft::WRL::ComPtr<ID3D11HullShader>		HS = nullptr; //　ハルシェーダ
	Microsoft::WRL::ComPtr<ID3D11DomainShader>		DS = nullptr;//　ドメインネームシェーダ


	Microsoft::WRL::ComPtr<ID3D11InputLayout>		VertexLayout = nullptr;

	HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);


public:
	Shader() {}
	virtual ~Shader() {}

	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName, LPCSTR HSName, LPCSTR DSName);

	void Activate();	//動作
	void Inactivate();	//停止
};


class ShaderEx :public Shader
{
private:


	HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);
	HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);
	HRESULT create_ds_from_cso(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader);
	HRESULT create_hs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader);
	HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);


public:
	ShaderEx() {}
	virtual ~ShaderEx() {}
	bool Create(WCHAR* vsfilename, WCHAR* psfilename);
	bool Create(WCHAR* vsfilename, WCHAR* gsfilename, WCHAR* psfilename);
	bool Create(WCHAR* vsfilename, WCHAR* dsfilename, WCHAR* hsfilename, WCHAR* psfilename);
	bool Create(WCHAR* vsfilename, WCHAR* dsfilename, WCHAR* hsfilename, WCHAR* gsfilename, WCHAR* psfilename);

};


//----------------------------------------------
//
//	頂点データ構造体定義
//
//----------------------------------------------

//struct VERTEX
//{
//	XMFLOAT3 Pos;	//位置
//	XMFLOAT3 Normal;//法線
//	XMFLOAT2 Tex;	//UV座標
//	XMFLOAT4 Color;	//頂点色
//};