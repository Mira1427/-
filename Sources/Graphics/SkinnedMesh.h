#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>


#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>



namespace DirectX {
	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT2& v) {

		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT3& v) {

		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4& v) {

		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4X4& m) {

		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}



struct NodeTree {

	struct Node {

		uint64_t uniqueID{ 0 };
		std::string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
		int64_t parentIndex{ -1 };

		template<class T>
		void serialize(T& archive) {

			archive(uniqueID, name, attribute, parentIndex);
		}
	};

	std::vector<Node> nodes;

	int64_t indexOf(uint64_t uniqueID) const {

		int64_t index{ 0 };

		for (const Node& node : nodes) {

			if (node.uniqueID == uniqueID) {

				return index;
			}

			index++;
		}

		return -1;
	}

	template<class T>
	void serialize(T& archive) {

		archive(nodes);
	}
};


class Skeleton {

public:

	struct Bone {

		uint64_t uniqueID{ 0 };
		std::string name;
		int64_t parentIndex{ -1 };	//	-1 : 親なし
		int64_t nodeIndex{ 0 };

		DirectX::XMFLOAT4X4 offsetTransform{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		bool isOrphan()const { return parentIndex < 0; }

		template<class T>
		void serialize(T& archive) {

			archive(uniqueID, name, parentIndex, nodeIndex, offsetTransform);
		}
	};


	std::vector<Bone> bones;

	int64_t indexOf(uint64_t uniqueID) const {

		int64_t index{ 0 };

		for (const Bone& bone : bones) {

			if (bone.uniqueID == uniqueID) {

				return index;
			}

			index++;
		}

		return -1;
	}

	template<class T>
	void serialize(T& archive) {

		archive(bones);
	}
};


struct Animation {

	std::string name;
	float samplingRate{ 0 };

	struct KeyFrame {

		struct Node {

			DirectX::XMFLOAT4X4 grobalTransform{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			};

			DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
			DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 };
			DirectX::XMFLOAT3 translation{ 0, 0, 0 };

			template<class T>
			void serialize(T& archive) {

				archive(grobalTransform, scaling, rotation, translation);
			}
		};

		std::vector<Node> nodes;

		template<class T>
		void serialize(T& archive) {

			archive(nodes);
		}
	};

	std::vector<KeyFrame> sequence;


	template<class T>
	void serialize(T& archive) {

		archive(name, samplingRate, sequence);
	}
};


class SkinnedMesh {

public:
	static const int MAxBONE_INFLUENCES{ 4 };

	struct Vertex {

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal{ 0, 1, 0 };
		DirectX::XMFLOAT4 tangent{ 1, 0, 0, 1 };
		DirectX::XMFLOAT2 texcoord{ 0, 0 };
		float boneWeights[MAxBONE_INFLUENCES]{ 1, 0, 0, 0 };
		uint32_t boneIndices[MAxBONE_INFLUENCES]{};

		template<class T>
		void serialize(T& archive) {

			archive(position, normal, tangent, texcoord, boneWeights, boneIndices);
		}
	};

	static const int MAxBONES{ 512 };
	struct Constants {

		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 materialColor;
		DirectX::XMFLOAT4X4 boneTransforms[MAxBONES]{
			{1, 0, 0, 0,
			 0, 1, 0, 0,
			 0, 0, 1, 0,
			 0, 0, 0, 1}
		};
	};


	struct Mesh {

		uint64_t uniqueID{ 0 };
		std::string name;
		int64_t nodeIndex{ 0 };

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		DirectX::XMFLOAT4X4 defaultGlobalTransform{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};


		Skeleton bindPose;


		struct Subset {

			uint64_t materialUniqueID{ 0 };
			std::string materialName;

			uint32_t startIndexLocation{ 0 };
			uint32_t indexCount{ 0 };


			template<class T>
			void serialize(T& archive) {

				archive(materialUniqueID, materialName, startIndexLocation, indexCount);
			}
		};

		std::vector<Subset> subsets;


		DirectX::XMFLOAT3 boundingBox[2]{
			{ +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
			{ -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
		};


		template<class T>
		void serialize(T& archive) {

			archive(uniqueID, name, nodeIndex, subsets, defaultGlobalTransform,
				bindPose, boundingBox, vertices, indices);
		}


	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
		friend class SkinnedMesh;
	};

	std::vector<Mesh> meshes;

	struct Material {

		uint64_t uniqueID{ 0 };
		std::string name;

		DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };

		std::string textureFileNames[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[4];


		template<class T>
		void serialize(T& archive) {

			archive(uniqueID, name, Ka, Kd, Ks, textureFileNames);
		}
	};

	std::unordered_map<uint64_t, Material> materials;

	std::vector<Animation> animationClips;


private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer;

public:
	SkinnedMesh(ID3D11Device* device, const char* fbxFileName, bool triangulate = false);
	virtual ~SkinnedMesh() = default;

	void fetchMeshes(fbxsdk::FbxScene* fbxScene, std::vector<Mesh>& meshes/*, fbxsdk::FbxGeometryConverter& fbxConverter*/);

	void fetchMaterials(fbxsdk::FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials);

	void fetchSkelton(fbxsdk::FbxMesh* fbxMesh, Skeleton& bindPose);

	void fetchAnimations(fbxsdk::FbxScene* fbxScene, std::vector<Animation>& animationClips,
		float samplingRate);

	void createCOMObject(ID3D11Device* device, const char* fbxFileName);

	void render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor,
		const Animation::KeyFrame* keyFrame);

	void updateAnimation(Animation::KeyFrame& keyFrame);

	//	アニメーションの読み込み
	bool appendAnimations(const char* animationFileName, float samplingRate);

	void blendAnimations(const Animation::KeyFrame* keyFrames[2], float factor, Animation::KeyFrame& keyFrame);

	NodeTree sceneView;
};