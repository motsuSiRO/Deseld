#pragma once
#include <string>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <fbxsdk.h>
#include "Mo2Cerealize.h"

struct Face
{
	int materialIndex;
	DirectX::XMFLOAT3 position[3];
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			CEREAL_NVP(materialIndex),
			CEREAL_NVP(position[0]),
			CEREAL_NVP(position[1]),
			CEREAL_NVP(position[2])
		);
	}

};


struct Face_List
{
	std::vector<Face> faces;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			CEREAL_NVP(faces)
		);
	}

	void LoadFromData(const char* filename);
	void BuildFaces(FbxNode* fbxNode);
	void BuildFace(FbxNode* fbxNode, FbxMesh* fbxMesh);

};