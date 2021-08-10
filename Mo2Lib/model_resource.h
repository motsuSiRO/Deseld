#pragma once
#include "model_data.h"

class Texture;

enum LOADING_TYPE
{
	LOAD_STATIC_MODEL = 0,
	LOAD_SKINNED_MODEL,
	LOAD_ANIMATION,
};

class ModelResource
{
public:
	ModelResource(ID3D11Device* device, const char* fbxFilename);
	ModelResource() {}
	~ModelResource() {}

	struct Node
	{
		std::string			name;
		int					parent_index;
		//DirectX::XMFLOAT3	scale;
		//DirectX::XMFLOAT4	rotate;
		//DirectX::XMFLOAT3	translate;
	};

	struct Material
	{
		DirectX::XMFLOAT4 color = { 1.f, 1.f, 1.f, 1.0f };
		std::shared_ptr<Texture> tex;
	};

	struct Subset
	{
		u_int		start_index = 0;
		u_int		index_count = 0;
		Material* material = nullptr;
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
	};


	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>	vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	index_buffer;
		std::vector<Subset>						subsets;

		int										node_index;
		std::vector<int>						node_indices;
		std::vector<DirectX::XMFLOAT4X4>		inverse_transforms;
	};

	struct NodeKeyData
	{
		std::string			name;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
	};

	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	node_keys;
	};
	struct Animation
	{
		//std::string					name;
		float						seconds_length;
		std::vector<Keyframe>		keyframes;
	};

	const std::vector<Mesh>& GetMeshes() const { return meshes; }
	const std::vector<Node>& GetNodes() const { return nodes; }
	const std::vector<Animation>& GetAnimations() const { return animations; }
	void AddAnimation(const char* Filename);
	int GetRemoveNodeCount() { return remove_node; }
	bool Load(ID3D11Device* device, ModelData* m_data, bool is_animation);
	//bool LoadFromBinary(ID3D11Device* device, const char* Filename, ModelData* m_data);
	void Set(std::shared_ptr<Texture> texture) { materials[0].tex = texture; }

private:
	// モデルデータを構築
	bool BuildModel(ID3D11Device* device, const char* dirname, FbxScene* fbxScene);

	// ノードデータを構築
	void BuildNodes(FbxNode* fbxNode, int parentNode_index);
	void BuildNode(FbxNode* fbxNode, int parentNode_index);

	// メッシュデータを構築
	void BuildMeshes(ID3D11Device* device, FbxNode* fbxNode);
	void BuildMesh(ID3D11Device* device, FbxNode* fbxNode, FbxMesh* fbxMesh);
	// マテリアルデータを構築
	void BuildMaterials(ID3D11Device* device, const char* dirname, FbxScene* fbxScene);
	void BuildMaterial(ID3D11Device* device, const char* dirname, FbxSurfaceMaterial* fbxSurfaceMaterial);

	// アニメーションデータを構築
	void BuildAnimations(FbxScene* fbxScene);
	void BuildAnimation(FbxScene* fbxScene, FbxString* fbxAnimStackName);
	// インデックスの検索
	int FindNode_index(const char* name);
	std::string FindNode_name(int index);
	int FindMaterial_index(FbxScene* fbxScene, const FbxSurfaceMaterial* fbxSurfaceMaterial);

	void ConvertFromData(ID3D11Device* device, ModelData* m_data);
	void ConvertFromAnimData(ID3D11Device* device, ModelData* m_data);
	//void AddAnimFromData(ModelData* a_data);
private:
	

	int						remove_node = 0;
	std::vector<Node>		nodes;
	std::vector<Material>	materials;
	std::vector<Mesh>		meshes;
	std::vector<Animation>	animations;
};



