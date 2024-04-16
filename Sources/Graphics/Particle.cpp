#include "Particle.h"

#include "Graphics.h"
#include "../misc.h"

Particle::Particle(ID3D11Device* device, size_t count) : maxParticleCount_(count)
{
	HRESULT hr{ S_OK };

	// --- パーティクルのバッファの作成 ---
	D3D11_BUFFER_DESC bufferDesc{};
	{
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ParticleData) * count);
		bufferDesc.StructureByteStride = sizeof(ParticleData);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		hr = device->CreateBuffer(&bufferDesc, NULL, buffer_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	// --- シェーダーリソースの作成 ---
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	{
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.ElementOffset = 0;
		srvDesc.Buffer.NumElements = static_cast<UINT>(count);

		hr = device->CreateShaderResourceView(buffer_.Get(), &srvDesc, bufferSRV_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// --- アンオーダーアクセスビューの作成 ---
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	{
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = static_cast<UINT>(count);
		uavDesc.Buffer.Flags = 0;

		hr = device->CreateUnorderedAccessView(buffer_.Get(), &uavDesc, bufferUAV_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// --- 定数バッファの作成 ---
	Graphics::createConstantBuffer(device, constantBuffer_.GetAddressOf(), sizeof(Particle::Constants));

	// --- シェーダーの読み込み ---
	Graphics::createVSFromCSO(device, "./Data/Shaders/Particle_VS.cso", vs_.ReleaseAndGetAddressOf());
	Graphics::createPSFromCSO(device, "./Data/Shaders/Particle_PS.cso", ps_.ReleaseAndGetAddressOf());
	Graphics::createGSFromCSO(device, "./Data/Shaders/Particle_GS.cso", gs_.ReleaseAndGetAddressOf());
	Graphics::createCSFromCSO(device, "./Data/Shaders/Particle_CS.cso", cs_.ReleaseAndGetAddressOf());
	Graphics::createCSFromCSO(device, "./Data/Shaders/ParticleInitializer_CS.cso", initializerCS_.ReleaseAndGetAddressOf());
}


void Particle::initialize(ID3D11DeviceContext* dc, float deltaTime)
{
	dc->CSSetUnorderedAccessViews(0, 1, bufferUAV_.GetAddressOf(), NULL);

	particleData_.time_			+= deltaTime;
	particleData_.deltaTime_	= deltaTime;
	Graphics::updateConstantBuffer(dc, constantBuffer_.Get(), particleData_);
	dc->CSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());
	dc->CSSetShader(initializerCS_.Get(), NULL, 0);

	auto align = [](UINT num, UINT alignment) {return (num + (alignment - 1)) & ~(alignment - 1); };
	const UINT threadGroupCountX = align(static_cast<UINT>(maxParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
	dc->Dispatch(threadGroupCountX, 1, 1);

	ID3D11UnorderedAccessView* nullUAV{};
	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);
}


void Particle::update(ID3D11DeviceContext* dc, float deltaTime)
{
	dc->CSSetUnorderedAccessViews(0, 1, bufferUAV_.GetAddressOf(), NULL);

	particleData_.time_ += deltaTime;
	particleData_.deltaTime_ = deltaTime;
	particleData_.emitterPosition_ = { 0.0f, 0.0f, -5.0f };
	Graphics::updateConstantBuffer(dc, constantBuffer_.Get(), particleData_);
	dc->CSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());
	dc->CSSetShader(cs_.Get(), NULL, 0);

	auto align = [](UINT num, UINT alignment) {return (num + (alignment - 1)) & ~(alignment - 1); };
	const UINT threadGroupCountX = align(static_cast<UINT>(maxParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
	dc->Dispatch(threadGroupCountX, 1, 1);

	ID3D11UnorderedAccessView* nullUAV{};
	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);
}


void Particle::draw(ID3D11DeviceContext* dc)
{
	// --- シェーダーのバインド ---
	dc->VSSetShader(vs_.Get(), nullptr, 0);
	dc->PSSetShader(ps_.Get(), nullptr, 0);
	dc->GSSetShader(gs_.Get(), nullptr, 0);
	dc->GSSetShaderResources(0, 1, bufferSRV_.GetAddressOf());


	// --- 定数バッファの更新とバインド ---
	Graphics::updateConstantBuffer(dc, constantBuffer_.Get(), particleData_);
	dc->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());
	dc->PSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());
	dc->GSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());

	auto** cbScene = Graphics::instance().constantBuffers[1/*SCENE*/].GetAddressOf();
	dc->GSSetConstantBuffers(1, 1, cbScene);


	dc->IASetInputLayout(nullptr);
	dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	dc->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	dc->Draw(static_cast<UINT>(maxParticleCount_), 0);

	ID3D11ShaderResourceView* nullSRV{};
	dc->GSSetShaderResources(0, 1, &nullSRV);
	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->GSSetShader(nullptr, nullptr, 0);
}
