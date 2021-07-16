#define _CRT_SECURE_NO_WARNINGS
#include "Shader.h"

#include<memory>

#include<map>
#include <wrl.h>
#include <d3dcompiler.h>
#include <assert.h>
#include <stdlib.h>


HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{

	// UNIT.05
	struct set_of_vertex_shader_and_input_layout
	{
		set_of_vertex_shader_and_input_layout(ID3D11VertexShader* vertex_shader, ID3D11InputLayout* input_layout) : vertex_shader(vertex_shader), input_layout(input_layout) {}
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	};
	static std::map<std::string, set_of_vertex_shader_and_input_layout> cache;

	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*vertex_shader = it->second.vertex_shader.Get();
		(*vertex_shader)->AddRef();
		*input_layout = it->second.input_layout.Get();
		(*input_layout)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device->CreateInputLayout(input_element_desc, num_elements, cso_data.get(), cso_sz, input_layout);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// UNIT.05
	cache.insert(std::make_pair(cso_name, set_of_vertex_shader_and_input_layout(*vertex_shader, *input_layout)));

	return hr;
}

HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{

	// UNIT.05
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		//it->second.Attach(*pixel_shader);
		*pixel_shader = it->second.Get();
		(*pixel_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// UNIT.05
	cache.insert(std::make_pair(cso_name, *pixel_shader));


	return hr;
}

// 入力レイアウト


//------------------------------------------------
//	有効化
//------------------------------------------------
void Shader::Activate()
{
	ID3D11DeviceContext* device_context = Mo2System->DX11context.Get();

	// 入力レイアウト設定
	device_context->IASetInputLayout(VertexLayout.Get());

	device_context->VSSetShader(VS.Get(), NULL, 0);
	device_context->HSSetShader(HS.Get(), NULL, 0);
	device_context->DSSetShader(DS.Get(), NULL, 0);
	device_context->GSSetShader(GS.Get(), NULL, 0);
	device_context->PSSetShader(PS.Get(), NULL, 0);


}

void Shader::Inactivate()
{
	ID3D11DeviceContext* device_context = Mo2System->DX11context.Get();

	device_context->VSSetShader(NULL, NULL, 0);
	device_context->HSSetShader(NULL, NULL, 0);
	device_context->DSSetShader(NULL, NULL, 0);
	device_context->GSSetShader(NULL, NULL, 0);
	device_context->PSSetShader(NULL, NULL, 0);

}


HRESULT ShaderEx::create_vs_from_cso(
	ID3D11Device* device,
	const char* cso_name,
	ID3D11VertexShader** vertex_shader,
	ID3D11InputLayout** input_layout,
	D3D11_INPUT_ELEMENT_DESC* input_element_desc,
	UINT num_elements)
{

	struct set_of_vertex_shader_and_input_layout
	{
		set_of_vertex_shader_and_input_layout(ID3D11VertexShader* vertex_shader, ID3D11InputLayout* input_layout) : vertex_shader(vertex_shader), input_layout(input_layout) {}
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	};
	static std::map<std::string, set_of_vertex_shader_and_input_layout> cache;


	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*vertex_shader = it->second.vertex_shader.Get();
		(*vertex_shader)->AddRef();
		*input_layout = it->second.input_layout.Get();
		(*input_layout)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	errno_t error;
	error = fopen_s(&fp, cso_name, "rb");
	if (error != 0)assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
	assert(SUCCEEDED(hr));

	hr = device->CreateInputLayout(input_element_desc, num_elements, cso_data.get(), cso_sz, input_layout);
	assert(SUCCEEDED(hr));

	cache.insert(std::make_pair(cso_name, set_of_vertex_shader_and_input_layout(*vertex_shader, *input_layout)));

	return hr;
}

HRESULT ShaderEx::create_hs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11HullShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*hull_shader = it->second.Get();
		(*hull_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateHullShader(cso_data.get(), cso_sz, nullptr, hull_shader);
	assert(SUCCEEDED(hr));

	cache.insert(std::make_pair(cso_name, *hull_shader));

	return hr;
}

HRESULT ShaderEx::create_ds_from_cso(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11DomainShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*domain_shader = it->second.Get();
		(*domain_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateDomainShader(cso_data.get(), cso_sz, nullptr, domain_shader);
	assert(SUCCEEDED(hr));

	cache.insert(std::make_pair(cso_name, *domain_shader));

	return hr;
}



HRESULT ShaderEx::create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11GeometryShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*geometry_shader = it->second.Get();
		(*geometry_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateGeometryShader(cso_data.get(), cso_sz, nullptr, geometry_shader);
	assert(SUCCEEDED(hr));

	cache.insert(std::make_pair(cso_name, *geometry_shader));

	return hr;
}

HRESULT ShaderEx::create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		//it->second.Attach(*pixel_shader);
		*pixel_shader = it->second.Get();
		(*pixel_shader)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, cso_name, "rb");
	if (error != 0) assert("CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader);
	assert(SUCCEEDED(hr));

	cache.insert(std::make_pair(cso_name, *pixel_shader));

	return hr;
}

bool ShaderEx::Create(WCHAR* vsfilename, WCHAR* psfilename)
{
	ID3D11Device* device = Mo2System->DX11device.Get();
	ID3D11DeviceContext* device_context = Mo2System->DX11context.Get();
	UINT numElements = (UINT)input_element_desc.size();
	//ワイド文字からマルチバイト文字へ変換
	char fullname[256];
	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, vsfilename, wcslen(vsfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", vsfilename, ".cso");
	create_vs_from_cso(device, fullname, VS.GetAddressOf(), VertexLayout.GetAddressOf(), input_element_desc.data(), numElements);

	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, psfilename, wcslen(psfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", psfilename, ".cso");
	create_ps_from_cso(device, fullname, PS.GetAddressOf());

	// 入力レイアウト設定
 	device_context->IASetInputLayout(VertexLayout.Get());


	return false;
}

bool ShaderEx::Create(WCHAR* vsfilename, WCHAR* gsfilename, WCHAR* psfilename)
{
	ID3D11Device* device = Mo2System->DX11device.Get();
	ID3D11DeviceContext* device_context = Mo2System->DX11context.Get();
	UINT numElements = (UINT)input_element_desc.size();
	//ワイド文字からマルチバイト文字へ変換
	char fullname[256];
	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, vsfilename, wcslen(vsfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", vsfilename, ".cso");
	create_vs_from_cso(device, fullname, VS.GetAddressOf(), VertexLayout.GetAddressOf(), input_element_desc.data(), numElements);

	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, gsfilename, wcslen(gsfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", gsfilename, ".cso");
	create_gs_from_cso(device, fullname, GS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, psfilename, wcslen(psfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", psfilename, ".cso");
	create_ps_from_cso(device, fullname, PS.GetAddressOf());

	// 入力レイアウト設定
	device_context->IASetInputLayout(VertexLayout.Get());


	return false;
}

bool ShaderEx::Create(WCHAR* vsfilename, WCHAR* hsfilename, WCHAR* dsfilename, WCHAR* psfilename)
{
	ID3D11Device* device = Mo2System->DX11device.Get();
	ID3D11DeviceContext* device_context = Mo2System->DX11context.Get();
	UINT numElements = (UINT)input_element_desc.size();

	//ワイド文字からマルチバイト文字へ変換
	char fullname[256];
	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, vsfilename, wcslen(vsfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", vsfilename, ".cso");
	create_vs_from_cso(device, fullname, VS.GetAddressOf(), VertexLayout.GetAddressOf(), input_element_desc.data(), numElements);

	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, hsfilename, wcslen(hsfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", hsfilename, ".cso");
	create_hs_from_cso(device, fullname, HS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, dsfilename, wcslen(dsfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", dsfilename, ".cso");
	create_ds_from_cso(device, fullname, DS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, psfilename, wcslen(psfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", psfilename, ".cso");
	create_ps_from_cso(device, fullname, PS.GetAddressOf());


	// 入力レイアウト設定
	device_context->IASetInputLayout(VertexLayout.Get());

	return false;
}

bool ShaderEx::Create(WCHAR* vsfilename, WCHAR* hsfilename, WCHAR* dsfilename, WCHAR* gsfilename, WCHAR* psfilename)
{
	ID3D11Device* device = Mo2System->DX11device.Get();
	ID3D11DeviceContext* device_context = Mo2System->DX11context.Get();
	UINT numElements = (UINT)input_element_desc.size();

	//ワイド文字からマルチバイト文字へ変換
	char fullname[256];
	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, vsfilename, wcslen(vsfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", vsfilename, ".cso");
	create_vs_from_cso(device, "sprite_vs.cso", VS.GetAddressOf(), VertexLayout.GetAddressOf(), input_element_desc.data(), numElements);

	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, hsfilename, wcslen(hsfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", hsfilename, ".cso");
	create_hs_from_cso(device, fullname, HS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, dsfilename, wcslen(dsfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", dsfilename, ".cso");
	create_ds_from_cso(device, fullname, DS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, gsfilename, wcslen(gsfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", gsfilename, ".cso");
	create_gs_from_cso(device, fullname, GS.GetAddressOf());

	memset(fullname, NULL, sizeof(fullname));
	//wcstombs(fullname, psfilename, wcslen(psfilename));
	sprintf(fullname, "%s%ws%s", "./Data/System/ShaderObjects/", psfilename, ".cso");
	create_ps_from_cso(device, fullname, PS.GetAddressOf());

	// 入力レイアウト設定
	device_context->IASetInputLayout(VertexLayout.Get());

	return false;
}
