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
	
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		VS = nullptr; // ���_�V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		PS = nullptr; // �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	GS = nullptr; // �W�I���g���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11HullShader>		HS = nullptr; //�@�n���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11DomainShader>		DS = nullptr;//�@�h���C���l�[���V�F�[�_


	Microsoft::WRL::ComPtr<ID3D11InputLayout>		VertexLayout = nullptr;

	HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);


public:
	Shader() {}
	virtual ~Shader() {}

	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName, LPCSTR HSName, LPCSTR DSName);

	void Activate();	//����
	void Inactivate();	//��~
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
//	���_�f�[�^�\���̒�`
//
//----------------------------------------------

//struct VERTEX
//{
//	XMFLOAT3 Pos;	//�ʒu
//	XMFLOAT3 Normal;//�@��
//	XMFLOAT2 Tex;	//UV���W
//	XMFLOAT4 Color;	//���_�F
//};