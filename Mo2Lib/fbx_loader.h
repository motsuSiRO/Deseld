//// UNIT.02
//#pragma once
//
//#include <fbxsdk.h> 
//#include "model_data.h"
//
//class FbxLoader
//{
//public:
//	// FBXファイルからモデルデータ読み込み
//	bool Load(const char* filename, ModelData& data);
//
//private:
//	// モデルデータを構築
//	bool BuildModel(const char* dirname, FbxScene* fbx_scene, ModelData& data);
//
//	// ノードデータを構築
//	void BuildNodes(FbxNode* fbx_node, ModelData& data, int parent_node_index);
//	void BuildNode(FbxNode* fbx_node, ModelData& data, int parent_node_index);
//	
//	// メッシュデータを構築
//	void BuildMeshes(FbxNode* fbx_node, ModelData& data);
//	void BuildMesh(FbxNode* fbx_node, FbxMesh* fbx_mesh, ModelData& data);
//
//	// マテリアルデータを構築
//	void BuildMaterials(const char* dirname, FbxScene* fbx_scene, ModelData& data);
//	void BuildMaterial(const char* dirname, FbxSurfaceMaterial* fbx_surface_material, ModelData& data);
//
//	// アニメーションデータを構築
//	void BuildAnimations(FbxScene* fbx_scene, ModelData& data);
//
//	// インデックスの検索
//	int FindNode_index(ModelData& data, const char* name);
//	int FindMaterial_index(FbxScene* fbx_scene, const FbxSurfaceMaterial* fbx_surface_material);
//};
