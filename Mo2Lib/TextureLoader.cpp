#include "TextureLoader.h"
#include "Texture.h"

std::shared_ptr<Texture> TextureLoader::Load(const wchar_t* filename = nullptr)
{
	auto it = texture_list.find(filename);
	if (it != texture_list.end())
	{
		//it->second.Attach(*shader_resource_view);
		return it->second;
	}
	else
	{
		Texture tex;
		tex.Load(filename);

		texture_list.insert(std::make_pair(filename, &tex));

		return std::make_shared<Texture>(tex);
	}




}