#pragma once

#include <d3d11.h>

namespace Texture
{

	HRESULT LoadFromFile(ID3D11Device* device, const wchar_t* file_name, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);

}

