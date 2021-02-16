#include "Texture.h"

#include "misc.h"

#include <WICTextureLoader.h>
#include <wrl.h>
#include <map>
#include <string>
#include <iostream>

namespace Texture
{
	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;

	HRESULT LoadFromFile(ID3D11Device* device, const wchar_t* file_name, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		//hr = DirectX::CreateWICTextureFromFile(device, file_name, resource.GetAddressOf(), shader_resource_view);
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	
		//std::wstring full_path(file_name);
		//int path_index = full_path.find_last_of(L"\\")+1;
		//std::wstring name(full_path.substr(path_index, full_path.size() - path_index));

		//file_path += file_name;
		// UNIT.05
		auto it = cache.find(file_name);
		if (it != cache.end())
		{
			//it->second.Attach(*shader_resource_view);
			*shader_resource_view = it->second.Get();
			(*shader_resource_view)->AddRef();
			(*shader_resource_view)->GetResource(resource.GetAddressOf());
		}
		else
		{
			// UNIT.04
			hr = DirectX::CreateWICTextureFromFile(device, file_name, resource.GetAddressOf(), shader_resource_view);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			// UNIT.05
			cache.insert(std::make_pair(file_name, *shader_resource_view));
		}

		if (texture2d_desc)
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
			hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			texture2d->GetDesc(texture2d_desc);
		}

		return hr;
	}
}
