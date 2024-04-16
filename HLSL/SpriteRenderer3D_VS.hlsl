#include "./SpriteRenderer.hlsli"

// --- 3Dオブジェクト定数 ---
cbuffer CBObject : register(b0)
{
	ObjectConstants object_;
}

VSOutput main( VSInput vin )
{
	VSOutput vout;

	row_major float4x4 offsetTranslation = 
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		object_.offset_.x, object_.offset_.y, 0, 1
	};

	float4 worldPosition = mul(float4(vin.position_, 1.0f), offsetTranslation);

	float4 worldPositionRotated = mul(worldPosition, object_.world_);

	row_major float4x4 offsetTranslationInverse =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-object_.offset_.x, -object_.offset_.y, 0, 1
	};

	worldPositionRotated = mul(worldPositionRotated, offsetTranslationInverse);

	vout.position_ = mul(worldPositionRotated, scene_.viewProjection_);
	vout.color_		= object_.color_;

	float2 texcoord = float2(
		(object_.texPos_.x + vin.texcoord_.x * object_.size_.x) / object_.texSize_.x,
		(object_.texPos_.y + vin.texcoord_.y * object_.size_.y) / object_.texSize_.y);
	vout.texcoord_ = texcoord;

	return vout;
}