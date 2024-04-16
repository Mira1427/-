#include "./SpriteRenderer.hlsli"
#include "./Common.hlsli"


// --- 2Dオブジェクト定数 ---
cbuffer CBObject2D : register(b0)
{
	ObjectConstants2D object_;
}


VSOutput main(VSInput vin)
{
	// --- 出力用変数 ---
	VSOutput vout;

	float theta = toRadian(object_.rotation_);
	float cosValue = cos(theta);
	float sinValue = sin(theta);

	float mx = (object_.size_.x * object_.scale_.x) / object_.size_.x * object_.center_.x;
	float my = (object_.size_.y * object_.scale_.y) / object_.size_.y * object_.center_.y;

	float2 position = vin.position_ * object_.size_ * object_.scale_;

	position.x -= mx;
	position.y -= my;

	float rx = position.x;
	float ry = position.y;
	position.x = rx * cosValue - ry * sinValue;
	position.y = rx * sinValue + ry * cosValue;

	position.x += mx;
	position.y += my;

	position.x += object_.position_.x - object_.scale_.x * object_.center_.x;
	position.y += object_.position_.y - object_.scale_.y * object_.center_.y;

	vout.position_ = float4(screenToNDC(position, window_.size_), 1.0f);
	vout.color_ = object_.color_;

	float2 texcoord = float2(
		(object_.texPos_.x + vin.texcoord_.x * object_.size_.x) / object_.texSize_.x,
		(object_.texPos_.y + vin.texcoord_.y * object_.size_.y) / object_.texSize_.y);

	vout.texcoord_ = texcoord;

	return vout;
}