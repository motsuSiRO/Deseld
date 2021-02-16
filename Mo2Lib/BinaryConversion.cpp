#include "BinaryConversion.h"
#include "Mo2Cerealize.h"
#include <fstream>

bool BinarySkinmeshInput(const char* file_path, ModelData* data, int _mode)
{
	if (!data)return false;
	std::ifstream ifs;
	char dirname[256];
	char filename[256];
	::_splitpath_s(file_path, nullptr, 0, dirname, 256, filename, 256, nullptr, 0);
	char f_path[256];
	//BINフォルダから読み込み
	::_makepath_s(f_path, 256, nullptr, "./Data/Assets/BIN/", filename, ".bin");

	ifs.open(f_path, std::ios::binary);
	if (!ifs)
	{
		ifs.close();
		return false;
	}
	//ModelData buf;
	if(_mode == 0)
	{
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(cereal::make_nvp(filename, *data));
	}
	else if (_mode == 1)
	{
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(cereal::make_nvp(filename, *data));
	}
	ifs.close();



	for (auto& m : data->meshes)
	{
		for (auto& n : m.tinvers)
		{
			DirectX::XMFLOAT4X4 inverse;

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					inverse.m[i][j] = n.m[i][j];
				}
			}

			m.inverse_transforms.push_back(inverse);
		}
	}
	return true;
}

bool BinarySkinmeshOutput(const char* file_path, ModelData* data)
{
	if (!data)return false;

	std::ofstream ofs;
	char dirname[256];
	char filename[256];
	::_splitpath_s(file_path, nullptr, 0, dirname, 256, filename, 256, nullptr, 0);
	char f_path[256];
	//BINフォルダに保存
	::_makepath_s(f_path, 256, nullptr, "./Data/Assets/BIN/", filename, ".bin");

	ofs.open(f_path, std::ios::binary);
	if (!ofs)
	{
		ofs.close();
		return false;
	}

	{
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(cereal::make_nvp(filename, *data));
	}
	ofs.close();

	::_makepath_s(f_path, 256, nullptr, "./Data/Assets/JSON/", filename, ".json");

	ofs.open(f_path, std::ios::binary);
	if (!ofs)
	{
		ofs.close();
		return false;
	}

	{
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(cereal::make_nvp(filename, *data));
	}
	ofs.close();

	return true;
}


bool BinaryFaceInput(const char* file_path, Face_List* data)
{
	if (!data)return false;
	std::ifstream ifs;
	char dirname[256];
	char filename[256];
	::_splitpath_s(file_path, nullptr, 0, dirname, 256, filename, 256, nullptr, 0);
	std::string str = filename;
	str += "_f"; 
	char f_path[256];
	//BINフォルダから読み込み
	::_makepath_s(f_path, 256, nullptr, "./Data/Assets/BIN/", str.c_str(), ".bin");

	ifs.open(f_path, std::ios::binary);
	if (!ifs)
	{
		ifs.close();
		return false;
	}
	//ModelData buf;
	{
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(cereal::make_nvp(filename, *data));
	}
	ifs.close();

	return true;
}

bool BinaryFaceOutput(const char* file_path, Face_List* data)
{
	if (!data)return false;

	std::ofstream ofs;
	char dirname[256];
	char filename[256];
	::_splitpath_s(file_path, nullptr, 0, dirname, 256, filename, 256, nullptr, 0);
	std::string str = filename;
	str += "_f";
	char f_path[256];
	//BINフォルダに保存
	::_makepath_s(f_path, 256, nullptr, "./Data/Assets/BIN/", str.c_str(), ".bin");

	ofs.open(f_path, std::ios::binary);
	if (!ofs)
	{
		ofs.close();
		return false;
	}

	{
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(cereal::make_nvp(filename, *data));
	}
	ofs.close();

	::_makepath_s(f_path, 256, nullptr, "./Data/Assets/JSON/", str.c_str(), ".json");

	ofs.open(f_path, std::ios::binary);
	if (!ofs)
	{
		ofs.close();
		return false;
	}

	{
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(cereal::make_nvp(filename, *data));
	}
	ofs.close();

	return true;
}