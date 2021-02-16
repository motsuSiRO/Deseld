#pragma once


#undef max
#undef min
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>


#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#include <DirectXMath.h>

namespace DirectX
{
	template<class Archive>
	void serialize(Archive& archive, DirectX::XMFLOAT2& vector)
	{
		archive(cereal::make_nvp("x", vector.x), cereal::make_nvp("y", vector.y));
	}

	template<class Archive>
	void serialize(Archive& archive, DirectX::XMFLOAT3& vector)
	{
		archive(cereal::make_nvp("x", vector.x), cereal::make_nvp("y", vector.y), cereal::make_nvp("z", vector.z));
	}

	template<class Archive>
	void serialize(Archive& archive, DirectX::XMFLOAT4& vector)
	{
		archive(cereal::make_nvp("x", vector.x), cereal::make_nvp("y", vector.y), cereal::make_nvp("z", vector.z), cereal::make_nvp("w", vector.w));
	}

	template<class Archive>
	void serialize(Archive& archive, DirectX::XMUINT4& vector)
	{
		archive(cereal::make_nvp("x", vector.x), cereal::make_nvp("y", vector.y), cereal::make_nvp("z", vector.z), cereal::make_nvp("w", vector.w));
	}
};
