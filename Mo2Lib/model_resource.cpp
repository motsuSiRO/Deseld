#include "model_resource.h"

//#include <stdlib.h>
#include <functional>
#include <WICTextureLoader.h>
#include <wrl.h>

#include "Misc.h"
#include "TextureLoader.h"
using namespace fbxsdk;

// FbxDouble2 → XMFLOAT2
inline DirectX::XMFLOAT2 FbxDouble2ToFloat2(const FbxDouble2& fbxValue)
{
	return DirectX::XMFLOAT2(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1])
	);
}

// FbxDouble3 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble3ToFloat3(const FbxDouble3& fbxValue)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2])
	);
}

// FbxDouble4 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble4ToFloat3(const FbxDouble4& fbxValue)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2])
	);
}

// FbxDouble4 → XMFLOAT4
inline DirectX::XMFLOAT4 FbxDouble4ToFloat4(const FbxDouble4& fbxValue)
{
	return DirectX::XMFLOAT4(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2]),
		static_cast<float>(fbxValue[3])
	);
}


// FbxDouble4 → XMFLOAT4
inline DirectX::XMFLOAT4X4 FbxAMatrixToFloat4x4(const FbxAMatrix& fbxValue)
{
	return DirectX::XMFLOAT4X4(
		static_cast<float>(fbxValue[0][0]),
		static_cast<float>(fbxValue[0][1]),
		static_cast<float>(fbxValue[0][2]),
		static_cast<float>(fbxValue[0][3]),
		static_cast<float>(fbxValue[1][0]),
		static_cast<float>(fbxValue[1][1]),
		static_cast<float>(fbxValue[1][2]),
		static_cast<float>(fbxValue[1][3]),
		static_cast<float>(fbxValue[2][0]),
		static_cast<float>(fbxValue[2][1]),
		static_cast<float>(fbxValue[2][2]),
		static_cast<float>(fbxValue[2][3]),
		static_cast<float>(fbxValue[3][0]),
		static_cast<float>(fbxValue[3][1]),
		static_cast<float>(fbxValue[3][2]),
		static_cast<float>(fbxValue[3][3])
	);
}


ModelResource::ModelResource(ID3D11Device* device, const char* fbxFilename)
{
	{

		FbxManager* fbxManager = FbxManager::Create();

		// FBXに対する入出力を定義する
		FbxIOSettings* fbxIOS = FbxIOSettings::Create(fbxManager, IOSROOT);	// 特別な理由がない限りIOSROOTを指定
		fbxManager->SetIOSettings(fbxIOS);

		// インポータを生成
		FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
		bool result = fbxImporter->Initialize(fbxFilename, -1, fbxManager->GetIOSettings());	// -1でファイルフォーマット自動判定
		_ASSERT_EXPR_A(result, "FbxImporter::Initialize() : Failed!!\n");

		// SceneオブジェクトにFBXファイル内の情報を流し込む
		FbxScene* fbxScene = FbxScene::Create(fbxManager, "scene");
		fbxImporter->Import(fbxScene);
		fbxImporter->Destroy();	// シーンを流し込んだらImporterは解放してOK

								// ジオメトリを三角形化しておく
		FbxGeometryConverter fbxGeometryConverter(fbxManager);
		fbxGeometryConverter.Triangulate(fbxScene, true);
		fbxGeometryConverter.RemoveBadPolygonsFromMeshes(fbxScene);

#if 0
		// DirectX座標系へ変換
		FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
		if (sceneAxisSystem != FbxAxisSystem::DirectX)
		{
			FbxAxisSystem::DirectX.ConvertScene(fbxScene);
		}
#elif 0
		// OpenGL座標系へ変換
		FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
		if (sceneAxisSystem != FbxAxisSystem::OpenGL)
		{
			FbxAxisSystem::OpenGL.ConvertScene(fbxScene);
		}
#endif

		// ディレクトリパス取得
		char dirname[256];
		::_splitpath_s(fbxFilename, nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);

		// モデル構築
		result = BuildModel(device, dirname, fbxScene);

		// マネージャ解放
		fbxManager->Destroy();		// 関連するすべてのオブジェクトが解放される


	}
}


bool ModelResource::Load(ID3D11Device* device, ModelData* m_data, bool anim)
{
	if (m_data)
	{
		if (!anim)ConvertFromData(device, m_data);
		else ConvertFromAnimData(device, m_data);

		return true;
	}
	return false;
}



// FBXシーンからモデルデータを構築する
bool ModelResource::BuildModel(ID3D11Device* device, const char* dirname, FbxScene* fbxScene)
{
	FbxNode* fbxRootNode = fbxScene->GetRootNode();

	BuildMaterials(device, dirname, fbxScene);
	BuildNodes(fbxRootNode, -1);
	BuildMeshes(device, fbxRootNode);
	BuildAnimations(fbxScene);

	return true;
}

// FBXノードを再帰的に辿ってデータを構築する
void ModelResource::BuildNodes(FbxNode* fbxNode, int parentNode_index)
{
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();
	FbxNodeAttribute::EType fbxNodeAttributeType = FbxNodeAttribute::EType::eUnknown;

	if (fbxNodeAttribute != nullptr)
	{
		fbxNodeAttributeType = fbxNodeAttribute->GetAttributeType();
	}

	switch (fbxNodeAttributeType)
	{
	case FbxNodeAttribute::eUnknown:
	case FbxNodeAttribute::eNull:
	case FbxNodeAttribute::eMarker:
	case FbxNodeAttribute::eMesh:
		remove_node++;
	case FbxNodeAttribute::eSkeleton:
		BuildNode(fbxNode, parentNode_index);
		break;
	}

	// 再帰的に子ノードを処理する
	parentNode_index = static_cast<int>(nodes.size() - 1);
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		BuildNodes(fbxNode->GetChild(i), parentNode_index);
	}
}

// FBXノードからノードデータを構築する
void ModelResource::BuildNode(FbxNode* fbxNode, int parentNode_index/*, int atribute_type*/)
{
	FbxAMatrix& fbxLocalTransform = fbxNode->EvaluateLocalTransform();

	Node node;
	//node.attribute = atribute_type;
	node.name = fbxNode->GetName();
	node.parent_index = parentNode_index;
	//node.scale = FbxDouble4ToFloat3(fbxLocalTransform.GetS());
	//node.rotate = FbxDouble4ToFloat4(fbxLocalTransform.GetQ());
	//node.translate = FbxDouble4ToFloat3(fbxLocalTransform.GetT());
	nodes.emplace_back(node);
}

// FBXノードを再帰的に辿ってメッシュデータを構築する
void ModelResource::BuildMeshes(ID3D11Device* device, FbxNode* fbxNode)
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
		BuildMesh(device, fbxNode, static_cast<FbxMesh*>(fbxNodeAttribute));
		break;
	}

	// 再帰的に子ノードを処理する
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		BuildMeshes(device, fbxNode->GetChild(i));
	}
}

// FBXメッシュからメッシュデータを構築する
void ModelResource::BuildMesh(ID3D11Device* device, FbxNode* fbxNode, FbxMesh* fbxMesh)
{
	int fbxControlPointsCount = fbxMesh->GetControlPointsCount();
	//int fbxPolygonVertexCount = fbxMesh->GetPolygonVertexCount();
	//const int* fbxPolygonVertices = fbxMesh->GetPolygonVertices();

	int fbxMaterialCount = fbxNode->GetMaterialCount();
	int fbxPolygonCount = fbxMesh->GetPolygonCount();

	meshes.emplace_back(Mesh());
	Mesh& mesh = meshes.back();
	mesh.subsets.resize(fbxMaterialCount > 0 ? fbxMaterialCount : 1);
	mesh.subsets.at(0).material = &materials.at(0);//ダミーテクスチャ用のマテリアル
	mesh.node_index = FindNode_index(fbxNode->GetName());



	// サブセットのマテリアル設定
	for (int fbxMaterial_index = 0; fbxMaterial_index < fbxMaterialCount; ++fbxMaterial_index)
	{
		const FbxSurfaceMaterial* fbxSurfaceMaterial = fbxNode->GetMaterial(fbxMaterial_index);

		Subset& subset = mesh.subsets.at(fbxMaterial_index);
		int index = FindMaterial_index(fbxNode->GetScene(), fbxSurfaceMaterial);
		subset.material = &materials.at(index);

	}

	// サブセットの頂点インデックス範囲設定
	if (fbxMaterialCount > 0)
	{
		for (int fbxPolygon_index = 0; fbxPolygon_index < fbxPolygonCount; ++fbxPolygon_index)
		{
			int fbxMaterial_index = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(fbxPolygon_index);
			//f.materialIndex = fbxMaterial_index;
			mesh.subsets.at(fbxMaterial_index).index_count += 3;
		}

		int offset = 0;
		for (Subset& subset : mesh.subsets)
		{
			subset.start_index = offset;
			offset += subset.index_count;

			subset.index_count = 0;
		}
	}

	// 頂点影響力データ
	struct BoneInfluence
	{
		int useCount = 0;
		int indices[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		float weights[8] = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };

		void Add(int index, float weight)
		{
			if (useCount < 4)
			{
				indices[useCount] = index;
				weights[useCount] = weight;
			}
			else if (useCount < 8)
			{
				indices[useCount] = index;
				weights[useCount] = weight;

			}
			useCount++;

			//static int a = 0;
			//if (a < useCount)
			//{
			//	a = useCount;
			//	char buffer[256];
			//	::sprintf_s(buffer, sizeof(buffer), "use_count:%d\n", useCount);
			//	::OutputDebugStringA(buffer);
			//}
		}
	};
	// 頂点影響力データを抽出する
	std::vector<BoneInfluence> boneInfluences;
	{
		boneInfluences.resize(fbxControlPointsCount);

		FbxAMatrix fbxGeometricTransform(
			fbxNode->GetGeometricTranslation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricRotation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricScaling(FbxNode::eSourcePivot)
		);

		// スキニングに必要な情報を取得する
		int fbxDeformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int fbxDeformer_index = 0; fbxDeformer_index < fbxDeformerCount; ++fbxDeformer_index)
		{
			FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(fbxDeformer_index, FbxDeformer::eSkin));

			int fbxClusterCount = fbxSkin->GetClusterCount();
			for (int fbxCluster_index = 0; fbxCluster_index < fbxClusterCount; ++fbxCluster_index)
			{
				FbxCluster* fbxCluster = fbxSkin->GetCluster(fbxCluster_index);

				// 頂点影響力データを抽出する
				{
					int fbxClusterControlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
					const int* fbxControlPointIndices = fbxCluster->GetControlPointIndices();
					const double* fbxControlPoint_weights = fbxCluster->GetControlPointWeights();

					for (int i = 0; i < fbxClusterControlPointIndicesCount; ++i)
					{
						BoneInfluence& boneInfluence = boneInfluences.at(fbxControlPointIndices[i]);

						boneInfluence.Add(fbxCluster_index, static_cast<float>(fbxControlPoint_weights[i]));
					}
				}


				// ボーン変換行列用の逆行列の計算をする
				{
					// メッシュ空間からワールド空間への変換行列
					FbxAMatrix fbxMeshSpaceTransform;
					fbxCluster->GetTransformMatrix(fbxMeshSpaceTransform);

					// ボーン空間からワールド空間への変換行列
					FbxAMatrix fbxBoneSpaceTransform;
					fbxCluster->GetTransformLinkMatrix(fbxBoneSpaceTransform);

					// ボーン逆行列を計算する
					FbxAMatrix fbxInverseTransform = fbxBoneSpaceTransform.Inverse() * fbxMeshSpaceTransform * fbxGeometricTransform;

					DirectX::XMFLOAT4X4 inverseTransform = FbxAMatrixToFloat4x4(fbxInverseTransform);
					mesh.inverse_transforms.emplace_back(inverseTransform);


					int node_index = FindNode_index(fbxCluster->GetLink()->GetName());
					mesh.node_indices.emplace_back(node_index);
				}
			}
		}
	}

	// UVセット名
	FbxStringList fbxUVSetNames;
	fbxMesh->GetUVSetNames(fbxUVSetNames);

	// 頂点データ
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	vertices.resize(fbxPolygonCount * 3);
	indices.resize(fbxPolygonCount * 3);



	int vertexCount = 0;
	const FbxVector4* fbxControlPoints = fbxMesh->GetControlPoints();
	for (int fbxPolygon_index = 0; fbxPolygon_index < fbxPolygonCount; ++fbxPolygon_index)
	{
		// ポリゴンに適用されているマテリアルインデックスを取得する
		int fbxMaterial_index = 0;
		if (fbxMaterialCount > 0)
		{
			fbxMaterial_index = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(fbxPolygon_index);
		}

		Subset& subset = mesh.subsets.at(fbxMaterial_index);
		const int indexOffset = subset.start_index + subset.index_count;


		for (int fbxVertex_index = 0; fbxVertex_index < 3; ++fbxVertex_index)
		{
			Vertex vertex;
			int fbxControlPoint_index = fbxMesh->GetPolygonVertex(fbxPolygon_index, fbxVertex_index);
			// Position
			{
				vertex.position = FbxDouble4ToFloat3(fbxControlPoints[fbxControlPoint_index]);

				// 面ごとの頂点データ 
				//f.position[fbxVertex_index].x = vertex.position.x;
				//f.position[fbxVertex_index].y = vertex.position.y;
				//f.position[fbxVertex_index].z = vertex.position.z;

			}

			// _weight
			{
				BoneInfluence& boneInfluence = boneInfluences.at(fbxControlPoint_index);
				vertex.bone_index_1to4.x = boneInfluence.indices[0];
				vertex.bone_index_1to4.y = boneInfluence.indices[1];
				vertex.bone_index_1to4.z = boneInfluence.indices[2];
				vertex.bone_index_1to4.w = boneInfluence.indices[3];
				vertex.bone_index_5to8.x = boneInfluence.indices[4];
				vertex.bone_index_5to8.y = boneInfluence.indices[5];
				vertex.bone_index_5to8.z = boneInfluence.indices[6];
				vertex.bone_index_5to8.w = boneInfluence.indices[7];
				vertex.bone_weight_1to4.x = boneInfluence.weights[0];
				vertex.bone_weight_1to4.y = boneInfluence.weights[1];
				vertex.bone_weight_1to4.z = boneInfluence.weights[2];
				vertex.bone_weight_1to4.w = boneInfluence.weights[3];
				vertex.bone_weight_5to8.x = boneInfluence.weights[4];
				vertex.bone_weight_5to8.y = boneInfluence.weights[5];
				vertex.bone_weight_5to8.z = boneInfluence.weights[6];
				vertex.bone_weight_5to8.w = 1.0f - (
					vertex.bone_weight_1to4.x +
					vertex.bone_weight_1to4.y +
					vertex.bone_weight_1to4.z +
					vertex.bone_weight_1to4.w +
					vertex.bone_weight_5to8.x +
					vertex.bone_weight_5to8.y +
					vertex.bone_weight_5to8.z);
				//vertex.bone_weight.w = boneInfluence.weights[3];
			}

			// Normal
			if (fbxMesh->GetElementNormalCount() > 0)
			{
				FbxVector4 fbxNormal;
				fbxMesh->GetPolygonVertexNormal(fbxPolygon_index, fbxVertex_index, fbxNormal);
				vertex.normal = FbxDouble4ToFloat3(fbxNormal);
			}
			else
			{
				vertex.normal = DirectX::XMFLOAT3(0, 0, 0);
			}

			// Texcoord
			if (fbxMesh->GetElementUVCount() > 0)
			{
				bool fbxUnmappedUV;
				FbxVector2 fbxUV;
				fbxMesh->GetPolygonVertexUV(fbxPolygon_index, fbxVertex_index, fbxUVSetNames[0], fbxUV, fbxUnmappedUV);
				fbxUV[1] = 1.0 - fbxUV[1];
				vertex.texcoord = FbxDouble2ToFloat2(fbxUV);
			}
			else
			{
				vertex.texcoord = DirectX::XMFLOAT2(0, 0);
			}

			indices.at(indexOffset + fbxVertex_index) = vertexCount;
			vertices.at(vertexCount) = vertex;

			vertexCount++;
		}

		subset.index_count += 3;

		//faces.push_back(f);
	}

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
		//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.data();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertex_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// インデックスバッファ
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * indices.size());
		//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = indices.data();
		subresourceData.SysMemPitch = 0; //Not use for index buffers.
		subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
		HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.index_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

}

// FBXシーン内のFBXマテリアルからマテリアルデータを構築する
void ModelResource::BuildMaterials(ID3D11Device* device, const char* dirname, FbxScene* fbxScene)
{
	int fbxMaterialCount = fbxScene->GetMaterialCount();

	if (fbxMaterialCount > 0)
	{
		for (int fbxMaterial_index = 0; fbxMaterial_index < fbxMaterialCount; ++fbxMaterial_index)
		{
			FbxSurfaceMaterial* fbxSurfaceMaterial = fbxScene->GetMaterial(fbxMaterial_index);

			BuildMaterial(device, dirname, fbxSurfaceMaterial);
		}
	}
	else
	{
		Material material;
		material.color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		materials.emplace_back(material);
	}
}

// FBXマテリアルからマテリアルデータを構築する
void ModelResource::BuildMaterial(ID3D11Device* device, const char* dirname, FbxSurfaceMaterial* fbxSurfaceMaterial)
{
	bool ret = false;

	Material material;
	// ディフューズカラー
	FbxProperty fbxDiffuseProperty = fbxSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	FbxProperty fbxDiffuseFactorProperty = fbxSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
	if (fbxDiffuseProperty.IsValid() && fbxDiffuseFactorProperty.IsValid())
	{
		FbxDouble fbxFactor = fbxDiffuseFactorProperty.Get<FbxDouble>();
		FbxDouble3 fbxColor = fbxDiffuseProperty.Get<FbxDouble3>();

		material.color.x = static_cast<float>(fbxColor[0] * fbxFactor);
		material.color.y = static_cast<float>(fbxColor[1] * fbxFactor);
		material.color.z = static_cast<float>(fbxColor[2] * fbxFactor);
		material.color.w = 1.0f;

	}
#if 0
	FbxProperty fbx_transparency_factor_property = fbxSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if (fbx_transparency_factor_property.IsValid())
	{
		FbxDouble fbxFactor = fbx_transparency_factor_property.Get<FbxDouble>();

		material.color.w = static_cast<float>(fbxFactor);
	}
#endif
	// ディフューズテクスチャ
	if (fbxDiffuseProperty.IsValid())
	{
		material.tex = std::make_shared<Texture>();

		int fbxTextureCount = fbxDiffuseProperty.GetSrcObjectCount<FbxFileTexture>();
		if (fbxTextureCount > 0)
		{
			FbxFileTexture* fbxTexture = fbxDiffuseProperty.GetSrcObject<FbxFileTexture>();

			// 相対パスの解決
			char filename[256];
			::_makepath_s(filename, 256, nullptr, dirname, fbxTexture->GetRelativeFileName(), nullptr);


			// マルチバイト文字からワイド文字へ変換
			size_t length;
			wchar_t wfilename[256];
			::mbstowcs_s(&length, wfilename, 256, filename, _TRUNCATE);

			// テクスチャ読み込み
			//Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			//Texture::LoadFromFile(device, wfilename, material.shader_resource_view.GetAddressOf(), nullptr);
			material.tex = Mo2Tex().Load(wfilename);
			//HRESULT hr = DirectX::CreateWICTextureFromFile(device, wfilename, resource.GetAddressOf(), material.shader_resource_view.GetAddressOf());
			//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else
		{
			// ダミーテクスチャ読み込み
			//Texture::LoadFromFile(device, L"./Data/Assets/textures/dummy_texture.png", material.shader_resource_view.GetAddressOf(), nullptr);
			material.tex = Mo2Tex().Load(L"./Data/Assets/textures/dummy_texture.png");
			//Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			//HRESULT hr = DirectX::CreateWICTextureFromFile(device, L"./Data/Assets/textures/dummy_texture.png", resource.GetAddressOf(), material.shader_resource_view.GetAddressOf());
			//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}

	materials.emplace_back(material);
}

// アニメーションデータを構築
void ModelResource::BuildAnimations(FbxScene* fbxScene)
{
	// すべてのアニメーション名を取得
	FbxArray<FbxString*> fbxAnimStackNames;
	fbxScene->FillAnimStackNameArray(fbxAnimStackNames);


	int fbxAnimationCount = fbxAnimStackNames.Size();
	//for (int fbxAnimation_index = 0; fbxAnimation_index < fbxAnimationCount; ++fbxAnimation_index)
	if (fbxAnimationCount > 0)
	{
		BuildAnimation(fbxScene, fbxAnimStackNames.GetAt(0));
	}

	// 後始末
	for (int i = 0; i < fbxAnimationCount; i++)
	{
		delete fbxAnimStackNames[i];
	}
}

void ModelResource::BuildAnimation(FbxScene* fbxScene, FbxString* fbxAnimStackName)
{
	animations.emplace_back(Animation());
	Animation& animation = animations.back();

	// アニメーションデータのサンプリング設定
	FbxTime::EMode fbxTimeMode = fbxScene->GetGlobalSettings().GetTimeMode();
	FbxTime fbxFrameTime;
	fbxFrameTime.SetTime(0, 0, 0, 1, 0, fbxTimeMode);

	float samplingRate = static_cast<float>(fbxFrameTime.GetFrameRate(fbxTimeMode));
	float samplingTime = 1.0f / samplingRate;

	//FbxString* fbxAnimStackName = fbxAnimStackNames.GetAt(fbxAnimation_index);
	FbxAnimStack* fbxAnimStack = fbxScene->FindMember<FbxAnimStack>(fbxAnimStackName->Buffer());

	// 再生するアニメーションを指定する。
	fbxScene->SetCurrentAnimationStack(fbxAnimStack);

	// アニメーションの再生開始時間と再生終了時間を取得する
	FbxTakeInfo* fbxTakeInfo = fbxScene->GetTakeInfo(fbxAnimStackName->Buffer());
	FbxTime fbxStartTime = fbxTakeInfo->mLocalTimeSpan.GetStart();
	FbxTime fbxEndTime = fbxTakeInfo->mLocalTimeSpan.GetStop();

	// 抽出するデータは60フレーム基準でサンプリングする
	FbxTime fbxSamplingStep;
	fbxSamplingStep.SetTime(0, 0, 1, 0, 0, fbxTimeMode);
	fbxSamplingStep = static_cast<FbxLongLong>(fbxSamplingStep.Get() * samplingTime);

	int startFrame = static_cast<int>(fbxStartTime.Get() / fbxSamplingStep.Get());
	int endFrame = static_cast<int>(fbxEndTime.Get() / fbxSamplingStep.Get());
	int frameCount = static_cast<int>((fbxEndTime.Get() - fbxStartTime.Get()) / fbxSamplingStep.Get());

	// アニメーションの対象となるノードを列挙する
	std::vector<FbxNode*> fbxNodes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* fbxNode)
	{
		FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();
		FbxNodeAttribute::EType fbxNodeAttributeType = FbxNodeAttribute::EType::eUnknown;
		if (fbxNodeAttribute != nullptr)
		{
			fbxNodeAttributeType = fbxNodeAttribute->GetAttributeType();
		}

		switch (fbxNodeAttributeType)
		{
			//case FbxNodeAttribute::eUnknown:
			//case FbxNodeAttribute::eNull:
			//case FbxNodeAttribute::eMarker:
			//case FbxNodeAttribute::eMesh:
		case FbxNodeAttribute::eSkeleton:
			fbxNodes.emplace_back(fbxNode);
			break;
		}

		for (int i = 0; i < fbxNode->GetChildCount(); i++)
		{
			traverse(fbxNode->GetChild(i));
		}
	};
	traverse(fbxScene->GetRootNode());

	// アニメーションデータを抽出する
	//animation.name = fbxAnimStack->GetName();
	animation.seconds_length = frameCount * samplingTime;
	animation.keyframes.resize(frameCount + 1);

	float seconds = 0.0f;
	Keyframe* keyframe = animation.keyframes.data();
	size_t fbxNodeCount = fbxNodes.size();

	FbxTime fbxCurrentTime = fbxStartTime;
	for (FbxTime fbxCurrentTime = fbxStartTime; fbxCurrentTime < fbxEndTime; fbxCurrentTime += fbxSamplingStep, ++keyframe)
	{
		// キーフレーム毎の姿勢データを取り出す。
		keyframe->seconds = seconds;
		keyframe->node_keys.resize(fbxNodeCount);

		for (size_t fbxNode_index = 0; fbxNode_index < fbxNodeCount; ++fbxNode_index)
		{
			NodeKeyData& keyData = keyframe->node_keys.at(fbxNode_index);

			FbxNode* fbxNode = fbxNodes.at(fbxNode_index);

			// 指定時間のローカル行列からスケール値、回転値、移動値を取り出す。
			const FbxAMatrix& fbxLocalTransform = fbxNode->EvaluateLocalTransform(fbxCurrentTime);
			keyData.name = fbxNode->GetName();
			keyData.scale = FbxDouble4ToFloat3(fbxLocalTransform.GetS());
			keyData.rotate = FbxDouble4ToFloat4(fbxLocalTransform.GetQ());
			keyData.translate = FbxDouble4ToFloat3(fbxLocalTransform.GetT());

		}
		seconds += samplingTime;
	}

}

void ModelResource::AddAnimation(const char* fbx_filename)
{

	FbxManager* fbxManager = FbxManager::Create();

	// FBXに対する入出力を定義する
	FbxIOSettings* fbxIOS = FbxIOSettings::Create(fbxManager, IOSROOT);	// 特別な理由がない限りIOSROOTを指定
	fbxManager->SetIOSettings(fbxIOS);

	// インポータを生成
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	bool result = fbxImporter->Initialize(fbx_filename, -1, fbxManager->GetIOSettings());	// -1でファイルフォーマット自動判定
	_ASSERT_EXPR_A(result, "FbxImporter::Initialize() : Failed!!\n");

	// SceneオブジェクトにFBXファイル内の情報を流し込む
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "scene");
	fbxImporter->Import(fbxScene);
	fbxImporter->Destroy();	// シーンを流し込んだらImporterは解放してOK

	BuildAnimations(fbxScene);

	// マネージャ解放
	fbxManager->Destroy();		// 関連するすべてのオブジェクトが解放される

}



// ノードインデックスを取得する
int ModelResource::FindNode_index(const char* name)
{
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].name == name)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

// ノードインデックスを取得する
std::string ModelResource::FindNode_name(int index)
{
	if (nodes.size() >= index && 0 <= index)
	{
		return nodes[index].name;
	}
	return "ERROR";
}

// マテリアルインデックスを取得する
int ModelResource::FindMaterial_index(FbxScene* fbxScene, const FbxSurfaceMaterial* fbxSurfaceMaterial)
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

void ModelResource::ConvertFromData(ID3D11Device* device, ModelData* m_data)
{
	if (!m_data)return;

	//Node
	nodes.resize(m_data->nodes.size());

	for (size_t i = 0; i < nodes.size(); ++i)
	{
		//nodes[i] = *(Node*)&m_data->nodes[i];
		//memcpy_s(&nodes[i], sizeof(Node), &m_data->nodes[i], sizeof(ModelData::Node));
		nodes[i].name = m_data->nodes[i].name;
		nodes[i].parent_index = m_data->nodes[i].parent_index;
	}

	//Material
	materials.resize(m_data->materials.size());

	for (size_t i = 0; i < materials.size(); ++i)
	{
		ModelData::Material& materialData = m_data->materials[i];
		Material& material = materials[i];
		material.color = materialData.color;


		if (materialData.tex_count > 0 && materialData.texture_filename != "")
		{

			// マルチバイト文字からワイド文字へ変換
			size_t length;
			wchar_t wfilename[256];
			::mbstowcs_s(&length, wfilename, 256, materialData.texture_filename.c_str(), _TRUNCATE);

			// テクスチャ読み込み
			//Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			//Texture::LoadFromFile(device, wfilename, material.shader_resource_view.GetAddressOf(), nullptr);
			material.tex = Mo2Tex().Load(wfilename);
			//HRESULT hr = DirectX::CreateWICTextureFromFile(device, wfilename, resource.GetAddressOf(), material.shader_resource_view.GetAddressOf());
			//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else
		{
			// ダミーテクスチャ読み込み
			//Texture::LoadFromFile(device, L"./Data/Assets/textures/dummy_texture.png", material.shader_resource_view.GetAddressOf(), nullptr);
			material.tex = Mo2Tex().Load(L"./Data/Assets/textures/dummy_texture.png");
			//Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			//HRESULT hr = DirectX::CreateWICTextureFromFile(device, L"./Data/Assets/textures/dummy_texture.png", resource.GetAddressOf(), material.shader_resource_view.GetAddressOf());
			//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}

	//mesh
	meshes.resize(m_data->meshes.size());

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		ModelData::Mesh& md_mesh = m_data->meshes[i];
		Mesh& mesh = meshes[i];
		mesh.subsets.resize(md_mesh.subsets.size());

		for (size_t j = 0; j < meshes[i].subsets.size(); ++j)
		{
			meshes[i].subsets[j].start_index = m_data->meshes[i].subsets[j].start_index;
			meshes[i].subsets[j].index_count = m_data->meshes[i].subsets[j].index_count;
			meshes[i].subsets[j].material = &materials[m_data->meshes[i].subsets[j].material_index];
		}

		meshes[i].node_index = m_data->meshes[i].node_index;
		meshes[i].node_indices.resize(m_data->meshes[i].node_indices.size());
		for (size_t j = 0; j < meshes[i].node_indices.size(); ++j)
		{
			meshes[i].node_indices[j] = m_data->meshes[i].node_indices[j];
		}

		meshes[i].inverse_transforms.resize(m_data->meshes[i].tinvers.size());

		for (size_t j = 0; j < meshes[i].inverse_transforms.size(); ++j)
		{
			for (int x = 0; x < 4; ++x)
			{
				for (int y = 0; y < 4; ++y)
				{
					meshes[i].inverse_transforms[j].m[x][y] = m_data->meshes[i].tinvers[j].m[x][y];
				}
			}
		}

		// 頂点バッファ
		{
			D3D11_BUFFER_DESC bufferDesc = {};
			D3D11_SUBRESOURCE_DATA subresourceData = {};

			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * m_data->meshes[i].vertices.size());
			//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			subresourceData.pSysMem = m_data->meshes[i].vertices.data();
			subresourceData.SysMemPitch = 0;
			subresourceData.SysMemSlicePitch = 0;

			HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, meshes[i].vertex_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		// インデックスバッファ
		{
			D3D11_BUFFER_DESC bufferDesc = {};
			D3D11_SUBRESOURCE_DATA subresourceData = {};

			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * m_data->meshes[i].indices.size());
			//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			subresourceData.pSysMem = m_data->meshes[i].indices.data();
			subresourceData.SysMemPitch = 0; //Not use for index buffers.
			subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
			HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, meshes[i].index_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}



	}

	//Face
	//faces.resize(m_data->faces.size());

	//for (size_t i = 0; i < faces.size(); ++i)
	//{
	//	//faces[i] = *(Face*)&m_data->faces[i];
	//	//memcpy_s(&faces[i], sizeof(Face), &m_data->faces[i], sizeof(ModelData::Face));
	//	faces[i].position[0] = m_data->faces[i].position[0];
	//	faces[i].position[1] = m_data->faces[i].position[1];
	//	faces[i].position[2] = m_data->faces[i].position[2];
	//	faces[i].materialIndex = m_data->faces[i].materialIndex;
	//}

	//Animation
	animations.resize(m_data->animations.size());

	for (size_t i = 0; i < animations.size(); ++i)
	{
		ModelData::Animation& animData = m_data->animations[i];
		Animation& anim = animations[i];

		anim.seconds_length = animData.seconds_length;

		for (auto keyData : animData.keyframes)
		{
			anim.keyframes.emplace_back(Keyframe());

			Keyframe& key = anim.keyframes.back();

			key.seconds = keyData.seconds;
			for (auto nodeData : keyData.node_keys)
			{
				key.node_keys.emplace_back(NodeKeyData());

				NodeKeyData& node_key = key.node_keys.back();
				node_key.name = nodeData.name;
				node_key.scale = nodeData.scale;
				node_key.rotate = nodeData.rotate;
				node_key.translate = nodeData.translate;

				//memcpy_s(&node_key, sizeof(NodeKeyData), &nodeData, sizeof(ModelData::NodeKeyData));
			}
		}

	}

}

void ModelResource::ConvertFromAnimData(ID3D11Device* device, ModelData* m_data)
{


	for (auto& a_data : m_data->animations)
	{
		animations.emplace_back(Animation());
		Animation& anim = animations.back();

		anim.seconds_length = a_data.seconds_length;

		for (auto keyData : a_data.keyframes)
		{
			anim.keyframes.emplace_back(Keyframe());

			Keyframe& key = anim.keyframes.back();

			key.seconds = keyData.seconds;
			for (auto nodeData : keyData.node_keys)
			{
				key.node_keys.emplace_back(NodeKeyData());

				NodeKeyData& node_key = key.node_keys.back();
				node_key.name = nodeData.name;
				node_key.scale = nodeData.scale;
				node_key.rotate = nodeData.rotate;
				node_key.translate = nodeData.translate;

			}
		}

	}

}

//void ModelResource::SaveToData(const char* filename, ModelData* m_data)
//{
//	ConvertFromData(m_data);
//
//	BinarySkeltalOutput(filename, m_data);
//}
//
