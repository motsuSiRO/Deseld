#include "FaceLoader.h"
#include "BinaryConversion.h"
#include <wrl.h>
#include "Misc.h"

// FbxDouble4 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble4ToFloat3(const FbxDouble4& fbxValue)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2])
	);
}


// マテリアルインデックスを取得する
int FindMaterial_index(FbxScene* fbxScene, const FbxSurfaceMaterial* fbxSurfaceMaterial)
{
	int fbxMaterialCount = fbxScene->GetMaterialCount();

	for (int i = 0; i < fbxMaterialCount; ++i)
	{
		if (fbxScene->GetMaterial(i) == fbxSurfaceMaterial)
		{
			return i;
		}
	}
	return -1;
}

void Face_List::LoadFromData(const char* filename)
{
	if (!BinaryFaceInput(filename, this))
	{
		FbxManager* fbxManager = FbxManager::Create();

		// FBXに対する入出力を定義する
		FbxIOSettings* fbxIOS = FbxIOSettings::Create(fbxManager, IOSROOT);	// 特別な理由がない限りIOSROOTを指定
		fbxManager->SetIOSettings(fbxIOS);

		// インポータを生成
		FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
		bool result = fbxImporter->Initialize(filename, -1, fbxManager->GetIOSettings());	// -1でファイルフォーマット自動判定
		_ASSERT_EXPR_A(result, "FbxImporter::Initialize() : Failed!!\n");

		// SceneオブジェクトにFBXファイル内の情報を流し込む
		FbxScene* fbxScene = FbxScene::Create(fbxManager, "scene");
		fbxImporter->Import(fbxScene);
		fbxImporter->Destroy();	// シーンを流し込んだらImporterは解放してOK

		FbxNode* fbxRootNode = fbxScene->GetRootNode();

		BuildFaces(fbxRootNode);

		// マネージャ解放
		fbxManager->Destroy();		// 関連するすべてのオブジェクトが解放される

		BinaryFaceOutput(filename, this);
	}
}

// FBXノードを再帰的に辿ってメッシュデータを構築する
void Face_List::BuildFaces(FbxNode* fbxNode)
{
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();
	FbxNodeAttribute::EType fbxNodeAttributeType = FbxNodeAttribute::EType::eUnknown;

	if (fbxNodeAttribute != nullptr)
	{
		fbxNodeAttributeType = fbxNodeAttribute->GetAttributeType();
	}

	switch (fbxNodeAttributeType)
	{
	case FbxNodeAttribute::eMesh:
		BuildFace(fbxNode, static_cast<FbxMesh*>(fbxNodeAttribute));
		break;
	}

	// 再帰的に子ノードを処理する
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		BuildFaces(fbxNode->GetChild(i));
	}
}

// FBXメッシュからメッシュデータを構築する
void Face_List::BuildFace(FbxNode* fbxNode, FbxMesh* fbxMesh)
{
	int fbxControlPointsCount = fbxMesh->GetControlPointsCount();
	//int fbxPolygonVertexCount = fbxMesh->GetPolygonVertexCount();
	//const int* fbxPolygonVertices = fbxMesh->GetPolygonVertices();

	int fbxMaterialCount = fbxNode->GetMaterialCount();
	int fbxPolygonCount = fbxMesh->GetPolygonCount();

	//meshes.emplace_back(Mesh());
	//Mesh& mesh = meshes.back();
	//mesh.subsets.resize(fbxMaterialCount > 0 ? fbxMaterialCount : 1);
	//mesh.subsets.at(0).material = &materials.at(0);//ダミーテクスチャ用のマテリアル
	//mesh.node_index = FindNode_index(fbxNode->GetName());

	Face f;

	// サブセットのマテリアル設定
	//for (int fbxMaterial_index = 0; fbxMaterial_index < fbxMaterialCount; ++fbxMaterial_index)
	//{
	//	const FbxSurfaceMaterial* fbxSurfaceMaterial = fbxNode->GetMaterial(fbxMaterial_index);

	//	//Subset& subset = mesh.subsets.at(fbxMaterial_index);
	//	//int index = FindMaterial_index(fbxNode->GetScene(), fbxSurfaceMaterial);
	//	//subset.material = &materials.at(index);

	//}

	// サブセットの頂点インデックス範囲設定
	if (fbxMaterialCount > 0)
	{
		for (int fbxPolygon_index = 0; fbxPolygon_index < fbxPolygonCount; ++fbxPolygon_index)
		{
			int fbxMaterial_index = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(fbxPolygon_index);
			f.materialIndex = fbxMaterial_index;
			//mesh.subsets.at(fbxMaterial_index).index_count += 3;
		}

	}


	faces.resize(fbxPolygonCount * 3);



	int vertexCount = 0;
	const FbxVector4* fbxControlPoints = fbxMesh->GetControlPoints();
	for (int fbxPolygon_index = 0; fbxPolygon_index < fbxPolygonCount; ++fbxPolygon_index)
	{
		// ポリゴンに適用されているマテリアルインデックスを取得する

		for (int fbxVertex_index = 0; fbxVertex_index < 3; ++fbxVertex_index)
		{
			int fbxControlPoint_index = fbxMesh->GetPolygonVertex(fbxPolygon_index, fbxVertex_index);
			// Position
			DirectX::XMFLOAT3 position = FbxDouble4ToFloat3(fbxControlPoints[fbxControlPoint_index]);
			{

				// 面ごとの頂点データ 
				f.position[fbxVertex_index].x = position.x;
				f.position[fbxVertex_index].y = position.y;
				f.position[fbxVertex_index].z = position.z;

			}


		}

		faces[fbxPolygon_index] = f;

	}
}