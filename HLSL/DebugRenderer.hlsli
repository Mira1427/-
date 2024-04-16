#include "./Constant.hlsli"

// --- 入力用構造体 ---
struct VSInput
{
	float4 position_ : POSITION;
};

// --- 出力用構造体 ---
struct VSOutput
{
	float4 position_	: SV_POSITION;
	float4 color_		: COLOR;
};


// --- オブジェクト定数 ---
struct ObjectConstants
{
	row_major float4x4 world_;
	float4 color_;
};

// --- オブジェクト定数2D ---
struct ObjectConstants2D
{
	float2	position_;
	float2	size_;
	float4	color_;
	float2	center_;
	float	rotation_;
};


// --- シーン定数 ---
cbuffer CBScene : register(b1)
{
	SceneConstants scene_;
}

// --- ウィンドウ定数 ---
cbuffer CBWindow : register(b2)
{
	WindowConstants window_;
}