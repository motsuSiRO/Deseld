#include "TextureLoader.h"
#include "Texture.h"

std::shared_ptr<Texture> TextureLoader::Load(const wchar_t* filename)
{
	auto it = texture_list.find(filename);
	if (it != texture_list.end())
	{
		//it->second.Attach(*shader_resource_view);
		return it->second;
	}
	else
	{
		std::shared_ptr<Texture> tex = std::make_shared<Texture>();

		tex->Load(filename);

		texture_list.insert(std::make_pair(filename, tex));

		return tex;
	}




}