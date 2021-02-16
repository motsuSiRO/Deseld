#pragma once
#include "model.h"
#include <unordered_map>

class SkyBox
{
public:
	Mo2Lib::Transform transform;
	std::shared_ptr<Mo2Lib::Model> sky;
	//Mo2Lib::Float4 light_dir;
	//Mo2Lib::Float4 light_color;

	void Initialize();
	void SetCamPos(Mo2Lib::Float3 pos) { transform.translate = pos; }
	bool Set(int index);
	void ImGui();

private:
	typedef std::unordered_map<int, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> TexMap;
	TexMap tex_list;
};

