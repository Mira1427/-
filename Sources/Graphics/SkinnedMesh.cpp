#include "skinnedMesh.h"

#include <functional>
#include <sstream>
#include <filesystem>
#include <fstream>

#include "../misc.h"
#include "TextureTool.h"
#include "Graphics.h"

inline DirectX::XMFLOAT4X4 toXMFloat4x4(const fbxsdk::FbxAMatrix& fbxAMatrix);
inline DirectX::XMFLOAT3   toXMFloat3(const fbxsdk::FbxDouble3& fbxDouble3);
inline DirectX::XMFLOAT4   toXMFloat4(const fbxsdk::FbxDouble4& fbxDouble4);


//	ボーンの影響度用
struct BoneInfluence {

	uint32_t boneIndex;
	float boneWeight;
};

using boneInfluencesPerControlPoint = std::vector<BoneInfluence>;


void fetchBoneInfluences(const fbxsdk::FbxMesh* fbxMesh,
	std::vector<boneInfluencesPerControlPoint>& boneInfluences) {

	const int  controlPointsCount{ fbxMesh->GetControlPointsCount() };
	boneInfluences.resize(controlPointsCount);

	const int skinCount{ fbxMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin) };
	for (int skinIndex = 0; skinIndex < skinCount; skinIndex++) {

		const fbxsdk::FbxSkin* fbxSkin{
			static_cast<fbxsdk::FbxSkin*>(fbxMesh->GetDeformer(skinIndex, fbxsdk::FbxDeformer::eSkin))
		};

		const int clusterCount{ fbxSkin->GetClusterCount() };
		for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) {

			const fbxsdk::FbxCluster* fbxCluster{ fbxSkin->GetCluster(clusterIndex) };


			const int controlPointIndicesCount{ fbxCluster->GetControlPointIndicesCount() };
			for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount;
				controlPointIndicesIndex++) {

				int controlPointIndex{ fbxCluster->GetControlPointIndices()[controlPointIndicesIndex] };
				double controlPointWeight{
					fbxCluster->GetControlPointWeights()[controlPointIndicesIndex] };
				BoneInfluence& boneInfluence{ boneInfluences.at(controlPointIndex).emplace_back() };
				boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
				boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
			}
		}
	}
}



SkinnedMesh::SkinnedMesh(ID3D11Device* device, const char* fbxFileName, bool triangulate) {

	std::filesystem::path cerealFileName(fbxFileName);
	cerealFileName.replace_extension("cereal");

	//	ファイルがあったら
	if (std::filesystem::exists(cerealFileName.c_str())) {

		//	デシリアライズ(復元)
		std::ifstream ifs(cerealFileName.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(sceneView, meshes, materials, animationClips);
	}

	//	なかったらFBXをロード
	else {

		fbxsdk::FbxManager* fbxManager{ fbxsdk::FbxManager::Create() };
		fbxsdk::FbxScene* fbxScene{ fbxsdk::FbxScene::Create(fbxManager, "") };

		fbxsdk::FbxImporter* fbxImporter{ fbxsdk::FbxImporter::Create(fbxManager, "") };
		bool importStatus{ false };
		importStatus = fbxImporter->Initialize(fbxFileName);
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

		importStatus = fbxImporter->Import(fbxScene);
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

		fbxsdk::FbxGeometryConverter fbxConverter(fbxManager);
		if (triangulate) {

			fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
			fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
		}


		std::function<void(fbxsdk::FbxNode*)> traverse{ [&](fbxsdk::FbxNode* fbxNode) {
			NodeTree::Node& node{sceneView.nodes.emplace_back() };
			node.attribute = fbxNode->GetNodeAttribute() ?
				fbxNode->GetNodeAttribute()->GetAttributeType() : fbxsdk::FbxNodeAttribute::EType::eUnknown;
			node.name = fbxNode->GetName();
			node.uniqueID = fbxNode->GetUniqueID();
			node.parentIndex = sceneView.indexOf(fbxNode->GetParent() ?
			fbxNode->GetParent()->GetUniqueID() : 0);

			for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); childIndex++) {

				traverse(fbxNode->GetChild(childIndex));
			}
		}};

		traverse(fbxScene->GetRootNode());

		fetchMeshes(fbxScene, meshes);
		fetchMaterials(fbxScene, materials);

		fbxManager->Destroy();


		std::ofstream ofs(cerealFileName.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive serialization(ofs);
		serialization(sceneView, meshes, materials, animationClips);
	}

	createCOMObject(device, fbxFileName);
}


void SkinnedMesh::fetchMeshes(fbxsdk::FbxScene* fbxScene, std::vector<Mesh>& meshes) {

	for (const NodeTree::Node& node : sceneView.nodes)
	{
		if (node.attribute != fbxsdk::FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}

		fbxsdk::FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };
		fbxsdk::FbxMesh* fbxMesh{ fbxNode->GetMesh() };

		if (!fbxMesh)
			continue;

		Mesh& mesh{ meshes.emplace_back() };

		//mesh.uniqueID = node.uniqueID;
		mesh.uniqueID = fbxNode->GetUniqueID();
		//mesh.name = node.name;
		mesh.name = fbxNode->GetName();
		mesh.nodeIndex = sceneView.indexOf(node.uniqueID);
		mesh.defaultGlobalTransform = toXMFloat4x4(fbxNode->EvaluateGlobalTransform());


		std::vector<boneInfluencesPerControlPoint> boneInfluences;
		fetchBoneInfluences(fbxMesh, boneInfluences);
		fetchSkelton(fbxMesh, mesh.bindPose);

		float samplingRate = 0;
		fetchAnimations(fbxScene, animationClips, samplingRate);


		std::vector<Mesh::Subset>& subsets{ mesh.subsets };
		const int  materialCount{ fbxMesh->GetNode()->GetMaterialCount() };
		subsets.resize(materialCount > 0 ? materialCount : 1);

		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {

			const fbxsdk::FbxSurfaceMaterial* fbxMaterial{ fbxMesh->GetNode()->GetMaterial(materialIndex) };
			subsets.at(materialIndex).materialName = fbxMaterial->GetName();
			subsets.at(materialIndex).materialUniqueID = fbxMaterial->GetUniqueID();
		}

		if (materialCount > 0) {

			const int polygonCount{ fbxMesh->GetPolygonCount() };

			for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {

				const int materialIndex{
					fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) };
				subsets.at(materialIndex).indexCount += 3;
			}

			uint32_t offset{ 0 };

			for (Mesh::Subset& subset : subsets) {

				subset.startIndexLocation = offset;
				offset += subset.indexCount;
				subset.indexCount = 0;
			}
		}

		const int polygonCount{ fbxMesh->GetPolygonCount() };
		mesh.vertices.resize(polygonCount * 3LL);
		mesh.indices.resize(polygonCount * 3LL);

		fbxsdk::FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);
		const fbxsdk::FbxVector4* controlPoints{ fbxMesh->GetControlPoints() };

		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {

			const int materialIndex{ materialCount > 0 ?
			fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0 };
			Mesh::Subset& subset{ subsets.at(materialIndex) };
			const uint32_t offset{ subset.startIndexLocation + subset.indexCount };

			for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon) {

				const int vertexIndex{ polygonIndex * 3 + positionInPolygon };

				Vertex vertex;
				const int polygonVertex{ fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon) };
				vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);
				vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
				vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);


				const boneInfluencesPerControlPoint& influencesPerControlPoint
				{ boneInfluences.at(polygonVertex) };

				//size_t sz = influencesPerControlPoint.size();
				//int waste = 0;
				for (size_t influenceIndex = 0; influenceIndex < influencesPerControlPoint.size();
					influenceIndex++) {

					if (influenceIndex < MAxBONE_INFLUENCES) {

						vertex.boneWeights[influenceIndex] =
							influencesPerControlPoint.at(influenceIndex).boneWeight;
						vertex.boneIndices[influenceIndex] =
							influencesPerControlPoint.at(influenceIndex).boneIndex;
					}
				}


				if (fbxMesh->GetElementNormalCount() > 0) {

					fbxsdk::FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}

				if (fbxMesh->GetElementUVCount() > 0) {

					fbxsdk::FbxVector2 uv;
					bool unmappedUV;
					fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon, uvNames[0], uv, unmappedUV);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}


				//	法線ベクトルの値を取得
				if (fbxMesh->GenerateTangentsData(0, false)) {

					const fbxsdk::FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);


				}

				mesh.vertices.at(vertexIndex) = std::move(vertex);
				//mesh.indices.at(vertexIndex) = vertexIndex;
				mesh.indices.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
				subset.indexCount++;
			}
		}


		for (const Vertex& v : mesh.vertices) {

			mesh.boundingBox[0].x = std::min<float>(mesh.boundingBox[0].x, v.position.x);
			mesh.boundingBox[0].y = std::min<float>(mesh.boundingBox[0].y, v.position.y);
			mesh.boundingBox[0].z = std::min<float>(mesh.boundingBox[0].z, v.position.z);
			mesh.boundingBox[1].x = std::max<float>(mesh.boundingBox[1].x, v.position.x);
			mesh.boundingBox[1].y = std::max<float>(mesh.boundingBox[1].y, v.position.y);
			mesh.boundingBox[1].z = std::max<float>(mesh.boundingBox[1].z, v.position.z);
		}
	}
}


void SkinnedMesh::fetchMaterials(fbxsdk::FbxScene* fbxScene,
	std::unordered_map<uint64_t, Material>& materials) {

	const size_t nodeCount{ sceneView.nodes.size() };

	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {

		const NodeTree::Node& node{ sceneView.nodes.at(nodeIndex) };
		const fbxsdk::FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };

		const int materialCount{ fbxNode->GetMaterialCount() };

		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {

			const fbxsdk::FbxSurfaceMaterial* fbxMaterial{ fbxNode->GetMaterial(materialIndex) };

			Material material;
			material.name.assign(fbxMaterial->GetName(), fbxMaterial->GetName() + strlen(fbxMaterial->GetName()));
			material.uniqueID = fbxMaterial->GetUniqueID();
			fbxsdk::FbxProperty fbxProperty;

			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

			//	Kd
			if (fbxProperty.IsValid()) {

				const fbxsdk::FbxDouble3 color{ fbxProperty.Get<fbxsdk::FbxDouble3>() };
				material.Kd.x = static_cast<float>(color[0]);
				material.Kd.y = static_cast<float>(color[1]);
				material.Kd.z = static_cast<float>(color[2]);
				material.Kd.w = 1.0f;

				const fbxsdk::FbxFileTexture* fbxTexture{ fbxProperty.GetSrcObject<fbxsdk::FbxFileTexture>() };

				material.textureFileNames[0] = ((fbxTexture) ? fbxTexture->GetRelativeFileName() : "");
			}


			//	Ka
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);

			if (fbxProperty.IsValid()) {

				const fbxsdk::FbxDouble3 color{ fbxProperty.Get<fbxsdk::FbxDouble3>() };
				material.Ka.x = static_cast<float>(color[0]);
				material.Ka.y = static_cast<float>(color[1]);
				material.Ka.z = static_cast<float>(color[2]);
				material.Ka.w = 1.0f;
			}


			//	Ks
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);

			if (fbxProperty.IsValid()) {

				const fbxsdk::FbxDouble3 color{ fbxProperty.Get<fbxsdk::FbxDouble3>() };
				material.Ks.x = static_cast<float>(color[0]);
				material.Ks.y = static_cast<float>(color[1]);
				material.Ks.z = static_cast<float>(color[2]);
				material.Ks.w = 1.0f;
			}


			fbxProperty = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sNormalMap);

			//	法線マップのファイル名を取得
			if (fbxProperty.IsValid()) {

				const fbxsdk::FbxFileTexture* fileTexture{ fbxProperty.GetSrcObject<fbxsdk::FbxFileTexture>() };

				material.textureFileNames[1] = ((fileTexture) ? fileTexture->GetRelativeFileName() : "");
			}

			materials.emplace(material.uniqueID, std::move(material));
		}
	}

	materials.emplace();
}


//	FBXメッシュからバインドポーズの情報を抽出
void SkinnedMesh::fetchSkelton(fbxsdk::FbxMesh* fbxMesh, Skeleton& bindPose) {

	const int deformerCount = fbxMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);

	for (int deformerIndex = 0; deformerIndex < deformerCount; deformerIndex++) {

		fbxsdk::FbxSkin* skin = static_cast<fbxsdk::FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, fbxsdk::FbxDeformer::eSkin));
		const int clusterCount = skin->GetClusterCount();
		bindPose.bones.resize(clusterCount);


		for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) {

			fbxsdk::FbxCluster* cluster = skin->GetCluster(clusterIndex);

			Skeleton::Bone& bone{ bindPose.bones.at(clusterIndex) };
			bone.name = cluster->GetLink()->GetName();
			bone.uniqueID = cluster->GetLink()->GetUniqueID();
			bone.parentIndex = bindPose.indexOf(cluster->GetLink()->GetParent()->GetUniqueID());
			bone.nodeIndex = sceneView.indexOf(bone.uniqueID);


			//	メッシュの初期グローバル行列を取得
			fbxsdk::FbxAMatrix referenceGlobalInitPosition;
			cluster->GetTransformMatrix(referenceGlobalInitPosition);


			//	ボーンの初期グローバル行列を取得
			fbxsdk::FbxAMatrix clusterGlobalInitPosition;
			cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);


			//	メッシュ空間からボーン空間へ変換するオフセット行列の作成
			bone.offsetTransform =
				toXMFloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
		}
	}
}


//	FBXシーンからアニメーション情報を抽出
void SkinnedMesh::fetchAnimations(fbxsdk::FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate) {

	fbxsdk::FbxArray<fbxsdk::FbxString*> animationStackNames;
	fbxScene->FillAnimStackNameArray(animationStackNames);

	const int animationStackCount{ animationStackNames.GetCount() };

	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; animationStackIndex++) {

		Animation& animationClip{ animationClips.emplace_back() };
		animationClip.name = animationStackNames[animationStackIndex]->Buffer();

		fbxsdk::FbxAnimStack* animationStack{ fbxScene->FindMember<fbxsdk::FbxAnimStack>(animationClip.name.c_str()) };
		fbxScene->SetCurrentAnimationStack(animationStack);

		const fbxsdk::FbxTime::EMode timeMode{ fbxScene->GetGlobalSettings().GetTimeMode() };
		fbxsdk::FbxTime oneSecond;
		oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
		animationClip.samplingRate = samplingRate > 0 ?
			samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));

		const fbxsdk::FbxTime samplingInterval{
			static_cast<fbxsdk::FbxLongLong>(oneSecond.Get() / animationClip.samplingRate) };

		const fbxsdk::FbxTakeInfo* takeInfo{ fbxScene->GetTakeInfo(animationClip.name.c_str()) };
		const fbxsdk::FbxTime startTime{ takeInfo->mLocalTimeSpan.GetStart() };
		const fbxsdk::FbxTime stopTime{ takeInfo->mLocalTimeSpan.GetStop() };


		for (fbxsdk::FbxTime time = startTime; time < stopTime; time += samplingInterval) {

			Animation::KeyFrame& keyFrame{ animationClip.sequence.emplace_back() };

			const size_t nodeCount{ sceneView.nodes.size() };
			keyFrame.nodes.resize(nodeCount);

			for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {

				fbxsdk::FbxNode* fbxNode{ fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str()) };

				if (fbxNode) {

					Animation::KeyFrame::Node& node{ keyFrame.nodes.at(nodeIndex) };

					node.grobalTransform = toXMFloat4x4(fbxNode->EvaluateGlobalTransform(time));

					const fbxsdk::FbxAMatrix& localTransform{ fbxNode->EvaluateLocalTransform(time) };
					node.scaling = toXMFloat3(localTransform.GetS());

					node.rotation = toXMFloat4(localTransform.GetQ());

					node.translation = toXMFloat3(localTransform.GetT());
				}
			}
		}
	}


	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; animationStackIndex++) {

		delete animationStackNames[animationStackIndex];
	}
}



void SkinnedMesh::createCOMObject(ID3D11Device* device, const char* fbxFileName) {

	for (Mesh& mesh : meshes) {

		HRESULT hr{ S_OK };
		D3D11_BUFFER_DESC bufferDesc{};
		D3D11_SUBRESOURCE_DATA subResourceData{};
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subResourceData.pSysMem = mesh.vertices.data();
		subResourceData.SysMemPitch = 0;
		subResourceData.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(&bufferDesc, &subResourceData,
			mesh.vertexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subResourceData.pSysMem = mesh.indices.data();

		hr = device->CreateBuffer(&bufferDesc, &subResourceData,
			mesh.indexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

#if 1
		mesh.vertices.clear();
		mesh.indices.clear();
#endif
	}


	// シェーダーリソースビューオブジェクトの生成
	for (std::unordered_map<uint64_t, Material>::iterator iterator = materials.begin();
		iterator != materials.end(); ++iterator) {

		for (size_t textureIndex = 0; textureIndex < 2; textureIndex++) {

			if (iterator->second.textureFileNames[textureIndex].size() > 0) {

				std::filesystem::path path(fbxFileName);
				path.replace_filename(iterator->second.textureFileNames[textureIndex]);
				D3D11_TEXTURE2D_DESC texture2dDesc{};
				loadTextureFromFile(device, path.c_str(),
					iterator->second.shaderResourceViews[textureIndex].GetAddressOf(), &texture2dDesc);
			}

			else {

				makeDummyTexture(device, iterator->second.shaderResourceViews[textureIndex].GetAddressOf(),
					textureIndex == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
			}
		}
	}

	HRESULT hr{ S_OK };
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]{

		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TANGENT",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"WEIGHTS",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"BONES",		0, DXGI_FORMAT_R32G32B32A32_UINT,	0, D3D11_APPEND_ALIGNED_ELEMENT },
	};

	Graphics::createVSAndILFromCSO(device, "./Data/Shaders/skinnedMeshVS.cso", vertexShader.ReleaseAndGetAddressOf(),
		inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	Graphics::createPSFromCSO(device, "./Data/Shaders/skinnedMeshPS.cso", pixelShader.ReleaseAndGetAddressOf());

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(Constants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


void SkinnedMesh::render(ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor,
	const Animation::KeyFrame* keyFrame) {

	using namespace DirectX;

	immediateContext->PSSetConstantBuffers(2, 1, Graphics::instance().getIlluminationManager()->constantBuffer_.GetAddressOf());

	for (const Mesh& mesh : meshes) {

		uint32_t stride{ sizeof(Vertex) };
		uint32_t offset{ 0 };
		immediateContext->IASetVertexBuffers(
			0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		immediateContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		immediateContext->IASetInputLayout(inputLayout.Get());

		immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
		immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

		Constants data{};

		if (keyFrame && keyFrame->nodes.size() > 0) {

			const Animation::KeyFrame::Node& meshNode{ keyFrame->nodes.at(mesh.nodeIndex) };
			DirectX::XMStoreFloat4x4(&data.world,
				DirectX::XMLoadFloat4x4(&meshNode.grobalTransform) * DirectX::XMLoadFloat4x4(&world));


			const size_t boneCount{ mesh.bindPose.bones.size() };
			_ASSERT_EXPR(boneCount < MAxBONES, L"The value of the 'boneCount' has exceeded MAxBONES. ");

			for (int boneIndex = 0; boneIndex < boneCount; boneIndex++) {

				const Skeleton::Bone& bone{ mesh.bindPose.bones.at(boneIndex) };

				const Animation::KeyFrame::Node& boneNode{ keyFrame->nodes.at(bone.nodeIndex) };

				DirectX::XMStoreFloat4x4(&data.boneTransforms[boneIndex],
					DirectX::XMLoadFloat4x4(&bone.offsetTransform) *
					DirectX::XMLoadFloat4x4(&boneNode.grobalTransform) *
					DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform))
				);
			}
		}

		else {

			DirectX::XMStoreFloat4x4(&data.world,
				DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform) * DirectX::XMLoadFloat4x4(&world));

			for (size_t boneIndex = 0; boneIndex < MAxBONES; boneIndex++) {

				data.boneTransforms[boneIndex] = {
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1
				};
			}
		}


		for (const Mesh::Subset& subset : mesh.subsets) {

			const Material& material{ materials.at(subset.materialUniqueID) };

			DirectX::XMStoreFloat4(&data.materialColor, DirectX::XMLoadFloat4(&materialColor) * DirectX::XMLoadFloat4(&material.Kd));

			immediateContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
			immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			immediateContext->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
			immediateContext->PSSetShaderResources(1, 1, material.shaderResourceViews[1].GetAddressOf());

			immediateContext->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
		}
	}
}


void SkinnedMesh::updateAnimation(Animation::KeyFrame& keyFrame) {

	size_t nodeCount{ keyFrame.nodes.size() };

	for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {

		Animation::KeyFrame::Node& node{ keyFrame.nodes.at(nodeIndex) };

		DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z) };
		DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
		DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };


		int64_t parentIndex{ sceneView.nodes.at(nodeIndex).parentIndex };
		DirectX::XMMATRIX P{ parentIndex < 0 ? DirectX::XMMatrixIdentity() :
			DirectX::XMLoadFloat4x4(&keyFrame.nodes.at(parentIndex).grobalTransform) };

		DirectX::XMStoreFloat4x4(&node.grobalTransform, S * R * T * P);
	}
}


//	アニメーションの読み込み
bool SkinnedMesh::appendAnimations(const char* animationFileName, float samplingRate) {

	fbxsdk::FbxManager* fbxManager{ fbxsdk::FbxManager::Create() };
	fbxsdk::FbxScene* fbxScene{ fbxsdk::FbxScene::Create(fbxManager, "") };

	fbxsdk::FbxImporter* fbxImporter{ fbxsdk::FbxImporter::Create(fbxManager, "") };
	bool importStatus{ false };
	importStatus = fbxImporter->Initialize(animationFileName);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
	importStatus = fbxImporter->Import(fbxScene);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

	fetchAnimations(fbxScene, animationClips, samplingRate);

	fbxManager->Destroy();

	return true;
}


void SkinnedMesh::blendAnimations(const Animation::KeyFrame* keyFrames[2], float factor, Animation::KeyFrame& keyFrame) {

	size_t nodeCount{ keyFrames[0]->nodes.size() };
	keyFrame.nodes.resize(nodeCount);

	for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {

		DirectX::XMVECTOR S[2]{
			DirectX::XMLoadFloat3(&keyFrames[0]->nodes.at(nodeIndex).scaling),
			DirectX::XMLoadFloat3(&keyFrames[1]->nodes.at(nodeIndex).scaling),
		};

		DirectX::XMStoreFloat3(&keyFrame.nodes.at(nodeIndex).scaling, DirectX::XMVectorLerp(S[0], S[1], factor));


		DirectX::XMVECTOR R[2]{
			DirectX::XMLoadFloat4(&keyFrames[0]->nodes.at(nodeIndex).rotation),
			DirectX::XMLoadFloat4(&keyFrames[1]->nodes.at(nodeIndex).rotation),
		};
		DirectX::XMStoreFloat4(&keyFrame.nodes.at(nodeIndex).rotation, DirectX::XMQuaternionSlerp(R[0], R[1], factor));


		DirectX::XMVECTOR T[2]{
			DirectX::XMLoadFloat3(&keyFrames[0]->nodes.at(nodeIndex).translation),
			DirectX::XMLoadFloat3(&keyFrames[1]->nodes.at(nodeIndex).translation),
		};
		DirectX::XMStoreFloat3(&keyFrame.nodes.at(nodeIndex).translation, DirectX::XMVectorLerp(T[0], T[1], factor));
	}
}


inline DirectX::XMFLOAT4X4 toXMFloat4x4(const fbxsdk::FbxAMatrix& fbxAMatrix) {

	DirectX::XMFLOAT4X4 matrix;

	for (int row = 0; row < 4; row++) {

		for (int column = 0; column < 4; column++) {

			matrix.m[row][column] = static_cast<float>(fbxAMatrix[row][column]);
		}
	}

	return matrix;
}


inline DirectX::XMFLOAT3 toXMFloat3(const fbxsdk::FbxDouble3& fbxDouble3) {

	DirectX::XMFLOAT3 xmFloat3;

	xmFloat3.x = static_cast<float>(fbxDouble3[0]);
	xmFloat3.y = static_cast<float>(fbxDouble3[1]);
	xmFloat3.z = static_cast<float>(fbxDouble3[2]);

	return xmFloat3;
}


inline DirectX::XMFLOAT4 toXMFloat4(const fbxsdk::FbxDouble4& fbxDouble4) {

	DirectX::XMFLOAT4 xmFloat4;

	xmFloat4.x = static_cast<float>(fbxDouble4[0]);
	xmFloat4.y = static_cast<float>(fbxDouble4[1]);
	xmFloat4.z = static_cast<float>(fbxDouble4[2]);
	xmFloat4.w = static_cast<float>(fbxDouble4[3]);

	return xmFloat4;
}