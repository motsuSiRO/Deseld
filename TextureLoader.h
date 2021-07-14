#pragma once

#include <unordered_map>
#include <string>
#include <memory>

class Texture;

class TextureLoader
{
public:
	void Create();

	static std::shared_ptr<Texture> Load(const wchar_t* filename = nullptr);
private:
	static std::unordered_map<std::wstring, std::shared_ptr<Texture>> texture_list;

private:
	static TextureLoader* ins;
};

using Mo2Tex = TextureLoader;