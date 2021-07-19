#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "Singleton.h"
#include "Texture.h"

class TextureLoader : public Singleton<TextureLoader>
{
public:

	std::shared_ptr<Texture> Load(const wchar_t* filename = nullptr);
private:
	std::unordered_map<std::wstring, std::shared_ptr<Texture>> texture_list;

};

static TextureLoader& Mo2Tex()
{
	return TextureLoader::GetInstance();
}
