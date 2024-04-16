#include "SpriteRenderer.h"

#include <Windows.h>

#include <WICTextureLoader.h>

#include "Graphics.h"
#include "../Library/Camera.h"

#include "../sceneManager.h"
#include "../GameObject.h"


// ===== テクスチャクラス ==================================================================================================================================
Texture::Texture(ID3D11Device* device, const char* fileName) :
	fileName_(fileName)
{
	// --- char*型からwchar_t*型へ変換 ---
	std::string str = fileName;
	size_t stringSize = str.size() + 1;
	std::unique_ptr<wchar_t[]> wFileName = std::make_unique<wchar_t[]>(stringSize);
	MultiByteToWideChar(CP_UTF8, 0, fileName, -1, wFileName.get(), static_cast<int>(stringSize));

	HRESULT hr{ S_OK };
	Microsoft::WRL::ComPtr<ID3D11Resource> resource{};

	// --- テクスチャの読み込み ---
	hr = DirectX::CreateWICTextureFromFile(device, wFileName.get(), resource.ReleaseAndGetAddressOf(), texture_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// --- テクスチャ情報の取得 ---
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d{};
	hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_TEXTURE2D_DESC desc{};
	texture2d->GetDesc(&desc);
	size_ = { static_cast<float>(desc.Width), static_cast<float>(desc.Height) };
}


// ===== スプライト描画クラス =============================================================================================================================

// --- コンストラクタ ---
SpriteRenderer::SpriteRenderer(ID3D11Device* device)
{
	Vertex vertices[]
	{
		{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
	};

	Vertex vertices3D[]
	{
		{ { -0.5f,  0.5, 0.0f }, { 0.0f, 0.0f }},
		{ {  0.5f,  0.5, 0.0f }, { 1.0f, 0.0f }},
		{ { -0.5f, -0.5, 0.0f }, { 0.0f, 1.0f }},
		{ {  0.5f, -0.5, 0.0f }, { 1.0f, 1.0f }}
	};


	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Graphics::createVSAndILFromCSO(device, "./Data/Shaders/SpriteRenderer_VS.cso", shaderResources_[SPRITE2D].vertexShader_.GetAddressOf(), inputLayout_.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	Graphics::createVSFromCSO(device, "./Data/Shaders/SpriteRenderer3D_VS.cso", shaderResources_[SPRITE3D].vertexShader_.GetAddressOf());
	Graphics::createPSFromCSO(device, "./Data/Shaders/SpriteRenderer_PS.cso", pixelShader_.GetAddressOf());
	Graphics::createPSFromCSO(device, "./Data/Shaders/Dissolve_PS.cso", dissolvePixelShader_.GetAddressOf());

	Graphics::createVertexBuffer(device, shaderResources_[SPRITE2D].vertexBuffer_.GetAddressOf(), sizeof(vertices), D3D11_USAGE_DEFAULT, vertices);
	Graphics::createVertexBuffer(device, shaderResources_[SPRITE3D].vertexBuffer_.GetAddressOf(), sizeof(vertices3D), D3D11_USAGE_DEFAULT, vertices3D);

	Graphics::createConstantBuffer(device, shaderResources_[SPRITE2D].constantBuffer_.GetAddressOf(), sizeof(ObjectConstants2D));
	Graphics::createConstantBuffer(device, shaderResources_[SPRITE3D].constantBuffer_.GetAddressOf(), sizeof(ObjectConstants3D));
}


// --- 描画処理 ---
void SpriteRenderer::draw(
	ID3D11DeviceContext* dc,
	Texture& texture,
	const Vector3& position,
	const Vector3& scale,
	const Vector2& texPos,
	const Vector2& texSize,
	const Vector2& center,
	const Vector3& rotation,
	const Vector4& color,
	bool inWorld,
	bool useBillboard)
{
	// --- シェーダーのバインド ---
	dc->PSSetShader(pixelShader_.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout_.Get());

	// --- 定数バッファのバインド ---
	dc->VSSetConstantBuffers(1, 1, Graphics::instance().constantBuffers[Constants::SCENE].GetAddressOf());
	dc->VSSetConstantBuffers(2, 1, Graphics::instance().constantBuffers[Constants::WINDOW].GetAddressOf());

	// --- シェーダーリソースのバインド ---
	dc->PSSetShaderResources(0, 1, texture.texture_.GetAddressOf());

	// --- 頂点バッファをバインドするための変数 ---
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };

	// --- ワールド空間に描画 ---
	if (inWorld)
	{
		// --- 頂点シェーダーのバインド ---
		dc->VSSetShader(shaderResources_[SPRITE3D].vertexShader_.Get(), nullptr, 0);
						
		// --- 頂点バッファのバインド ---
		dc->IASetVertexBuffers(0, 1, shaderResources_[SPRITE3D].vertexBuffer_.GetAddressOf(), &stride, &offset);

		// --- 定数バッファのバインド ---
		dc->VSSetConstantBuffers(0, 1, shaderResources_[SPRITE3D].constantBuffer_.GetAddressOf());

		// --- 定数バッファの更新 ---
		{
			ObjectConstants3D data;
			Matrix s;
			s.makeScaling({ texSize.x * scale.x, texSize.y * scale.y, scale.z });
			Matrix r;
			r.makeRotation(rotation);
			Matrix t;
			t.makeTranslation(position);
			data.world_ = s * r * t;

			data.color_ = color;

			data.offset_ = center;

			data.size_		= texSize;
			data.texPos_	= texPos;
			data.texSize_	= texture.size_;

			Graphics::updateConstantBuffer(dc, shaderResources_[SPRITE3D].constantBuffer_.Get(), data);
		}
	}

	// --- スクリーン空間に描画 ---
	else
	{
		// --- 頂点シェーダーのバインド ---
		dc->VSSetShader(shaderResources_[SPRITE2D].vertexShader_.Get(), nullptr, 0);

		// --- 頂点バッファのバインド ---
		dc->IASetVertexBuffers(0, 1, shaderResources_[SPRITE2D].vertexBuffer_.GetAddressOf(), &stride, &offset);

		// --- 定数バッファのバインド ---
		dc->VSSetConstantBuffers(0, 1, shaderResources_[SPRITE2D].constantBuffer_.GetAddressOf());

		// --- 定数バッファの更新 ---
		{
			ObjectConstants2D data;
			data.position_ = { position.x, position.y };
			data.size_ = texSize;
			data.color_ = color;
			data.center_ = center;
			data.scale_ = { scale.x, scale.y };
			data.rotation_ = rotation.x;
			data.texPos_ = texPos;
			data.texSize_ = texture.size_;


			if (useBillboard)
			{
				D3D11_VIEWPORT viewport;
				UINT num = 1;
				dc->RSGetViewports(&num, &viewport);

				auto* camera = SceneManager::instance().scene->camera_->getComponent<CameraComponent>();
				
				Vector3 worldPosition;
				DirectX::XMStoreFloat3(&worldPosition.vec_,
					DirectX::XMVector3Project(
						position,
						viewport.TopLeftX, viewport.TopLeftY,
						viewport.Width, viewport.Height,
						viewport.MinDepth, viewport.MaxDepth,
						camera->projection_,
						camera->view_,
						Matrix::Identify_
					)
				);

				data.position_ = Vector2(worldPosition.x, worldPosition.y);
			}

			Graphics::updateConstantBuffer(dc, shaderResources_[SPRITE2D].constantBuffer_.Get(), data);
		}
	}


	// --- 描画法設定 ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// --- 描画 ---
	dc->Draw(4, 0);
}

void SpriteRenderer::drawDissolve(ID3D11DeviceContext* dc, Texture& texture, Texture& noiseTexture, float dissolveAmount, const Vector3& position, const Vector3& scale, const Vector2& texPos, const Vector2& texSize, const Vector2& center, const Vector3& rotation, const Vector4& color, bool inWorld)
{
	// --- シェーダーのバインド ---
	dc->PSSetShader(dissolvePixelShader_.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout_.Get());

	// --- 定数バッファのバインド ---
	dc->VSSetConstantBuffers(1, 1, Graphics::instance().constantBuffers[Constants::SCENE].GetAddressOf());
	dc->VSSetConstantBuffers(2, 1, Graphics::instance().constantBuffers[Constants::WINDOW].GetAddressOf());

	// --- シェーダーリソースのバインド ---
	dc->PSSetShaderResources(0, 1, texture.texture_.GetAddressOf());
	dc->PSSetShaderResources(1, 1, noiseTexture.texture_.GetAddressOf());

	// --- 頂点バッファをバインドするための変数 ---
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };

	// --- ディゾルブ用の定数バッファのバインドと更新 ---
	dc->PSSetConstantBuffers(3, 1, Graphics::instance().constantBuffers[Constants::DISSOLVE].GetAddressOf());
	{
		Constants::Dissolve data;
		data.amount_ = dissolveAmount;
		Graphics::instance().updateConstantBuffer(Constants::DISSOLVE, data);
	}

	// --- ワールド空間に描画 ---
	if (inWorld)
	{
		// --- 頂点シェーダーのバインド ---
		dc->VSSetShader(shaderResources_[SPRITE3D].vertexShader_.Get(), nullptr, 0);

		// --- 頂点バッファのバインド ---
		dc->IASetVertexBuffers(0, 1, shaderResources_[SPRITE3D].vertexBuffer_.GetAddressOf(), &stride, &offset);

		// --- 定数バッファのバインド ---
		dc->VSSetConstantBuffers(0, 1, shaderResources_[SPRITE3D].constantBuffer_.GetAddressOf());

		// --- 定数バッファの更新 ---
		{
			ObjectConstants3D data;
			Matrix s;
			s.makeScaling({ texSize.x * scale.x, texSize.y * scale.y, scale.z });
			Matrix r;
			r.makeRotation(rotation);
			Matrix t;
			t.makeTranslation(position);
			data.world_ = s * r * t;

			data.color_ = color;

			data.offset_ = center;

			data.size_ = texSize;
			data.texPos_ = texPos;
			data.texSize_ = texture.size_;

			Graphics::updateConstantBuffer(dc, shaderResources_[SPRITE3D].constantBuffer_.Get(), data);
		}
	}

	// --- スクリーン空間に描画 ---
	else
	{
		// --- 頂点シェーダーのバインド ---
		dc->VSSetShader(shaderResources_[SPRITE2D].vertexShader_.Get(), nullptr, 0);

		// --- 頂点バッファのバインド ---
		dc->IASetVertexBuffers(0, 1, shaderResources_[SPRITE2D].vertexBuffer_.GetAddressOf(), &stride, &offset);

		// --- 定数バッファのバインド ---
		dc->VSSetConstantBuffers(0, 1, shaderResources_[SPRITE2D].constantBuffer_.GetAddressOf());

		// --- 定数バッファの更新 ---
		{
			ObjectConstants2D data;
			data.position_	= { position.x, position.y };
			data.size_		= texSize;
			data.color_		= color;
			data.center_	= center;
			data.scale_		= { scale.x, scale.y };
			data.rotation_	= rotation.x;
			data.texPos_	= texPos;
			data.texSize_	= texture.size_;

			Graphics::updateConstantBuffer(dc, shaderResources_[SPRITE2D].constantBuffer_.Get(), data);
		}
	}


	// --- 描画法設定 ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// --- 描画 ---
	dc->Draw(4, 0);
}
