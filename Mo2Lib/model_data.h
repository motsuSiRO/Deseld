#pragma once

#include <string>
#include <vector>
#include <d3d11.h>
#include <fbxsdk.h>
#include <wrl.h>

#include "Vector3D.h"
#include "Mo2Cerealize.h"

struct tMatrix
{
	float	m[4][4];

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(m[0][0], m[0][1], m[0][2], m[0][3],
			m[1][0], m[1][1], m[1][2], m[1][3],
			m[2][0], m[2][1], m[2][2], m[2][3],
			m[3][0], m[3][1], m[3][2], m[3][3]);
	}
};


struct ModelData
{
	enum LOAD_FORMAT
	{
		BINARY = 0,
		JSON,
	};

	ModelData(const char* fbxfilename, int _mode = BINARY);
	ModelData() { }
	bool Load(const char* fbxfilename, int _mode = BINARY);
	struct Node
	{
		std::string			name;
		int					parent_index;
		//DirectX::XMFLOAT3	scale;
		//DirectX::XMFLOAT4	rotate;
		//DirectX::XMFLOAT3	translate;


		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(parent_index));
		}
	};

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT3	normal;
		DirectX::XMFLOAT2	texcoord;
		DirectX::XMFLOAT4	bone_weight_1to4;
		DirectX::XMFLOAT4	bone_weight_5to8;
		DirectX::XMUINT4	bone_index_1to4;
		DirectX::XMUINT4	bone_index_5to8;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				CEREAL_NVP(position),
				CEREAL_NVP(normal),
				CEREAL_NVP(texcoord),
				CEREAL_NVP(bone_weight_1to4),
				CEREAL_NVP(bone_index_1to4),
				CEREAL_NVP(bone_weight_5to8),
				CEREAL_NVP(bone_index_5to8)
			);
		}

	};

	struct Subset
	{
		int					material_index;
		unsigned int		start_index;
		unsigned int		index_count;


		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				CEREAL_NVP(material_index),
				CEREAL_NVP(start_index),
				CEREAL_NVP(index_count)
			);
		}

	};

	struct Mesh
	{
		std::vector<Vertex>	vertices;
		std::vector<int>	indices;
		std::vector<Subset>	subsets;

		int					node_index;

		std::vector<int>					node_indices;
		std::vector<DirectX::XMFLOAT4X4>	inverse_transforms;
		std::vector<tMatrix>	tinvers;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				CEREAL_NVP(vertices),
				CEREAL_NVP(indices),
				CEREAL_NVP(subsets),
				CEREAL_NVP(node_index),
				CEREAL_NVP(node_indices),
				CEREAL_NVP(tinvers)
			);
		}

	};


	struct Material
	{
		DirectX::XMFLOAT4	color;
		std::string			texture_filename;
		int tex_count;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				CEREAL_NVP(color),
				CEREAL_NVP(texture_filename),
				CEREAL_NVP(tex_count)
			);
		}

	};

	struct NodeKeyData
	{
		std::string			name;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				CEREAL_NVP(name),
				CEREAL_NVP(scale),
				CEREAL_NVP(rotate),
				CEREAL_NVP(translate)
			);
		}


	};

	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	node_keys;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				CEREAL_NVP(seconds),
				CEREAL_NVP(node_keys)
			);
		}

	};
	struct Animation
	{
		std::string name;
		float						seconds_length;
		std::vector<Keyframe>		keyframes;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				CEREAL_NVP(name),
				CEREAL_NVP(seconds_length),
				CEREAL_NVP(keyframes)
			);
		}
	};

	const char* filename;
	std::vector<Node>		nodes;

	std::vector<Mesh>		meshes;
	std::vector<Material>	materials;

	std::vector<Animation>	animations;

	void Clear()
	{
		filename = "";
		nodes.clear();
		meshes.clear();
		materials.clear();
		animations.clear();
	}

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			CEREAL_NVP(nodes),
			CEREAL_NVP(meshes),
			CEREAL_NVP(materials),
			CEREAL_NVP(animations)
		);
	}
	void AddAnimation(const char* Filename);

	// モデルデータを構築
	bool BuildModel(const char* dirname, FbxScene* fbxScene);

	// ノードデータを構築
	void BuildNodes(FbxNode* fbxNode, int parentNode_index);
	void BuildNode(FbxNode* fbxNode, int parentNode_index);

	// メッシュデータを構築
	void BuildMeshes(FbxNode* fbxNode);
	void BuildMesh(FbxNode* fbxNode, FbxMesh* fbxMesh);

	// マテリアルデータを構築
	void BuildMaterials(const char* dirname, FbxScene* fbxScene);
	void BuildMaterial(const char* dirname, FbxSurfaceMaterial* fbxSurfaceMaterial);

	// アニメーションデータを構築
	void BuildAnimations(FbxScene* fbxScene);
	void BuildAnimation(FbxScene* fbxScene, FbxString* fbxAnimStackName);

	// インデックスの検索
	int FindNode_index(const char* name);
	std::string FindNode_name(int index);
	int FindMaterial_index(FbxScene* fbxScene, const FbxSurfaceMaterial* fbxSurfaceMaterial);

	void SaveToFile(const char* modelname);
};