// ===== 定数用ヘッダー =============================================================================================================================

// ===== 定数 =======================================================================================================================================

// --- ブレンドステート用 ---
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

#define PI 3.14159265358979

// ===== 定数バッファ用構造体 ===================================================================================================================

// --- シーン定数 ---
struct SceneConstants
{
	row_major float4x4	viewProjection_;
	row_major float4x4  invViewProjection_;
	float4				cameraPosition_;
};


// --- ウィンドウ定数 ---
struct WindowConstants
{
	float2 size_;
};


// --- ディゾルブ定数 ---
struct DissolveConstants
{
	float amount_;
};


// --- シェーダートイ定数 ---
cbuffer ShaderToyConstants : register(b5)
{
	float4 iMouse;
	float4 iChannelResolution[4];
	float4 iResolution;
	float iTime;
	float iFrame;
}


// --- 環境光定数 ---
struct AmbientLight
{
	float3 color_;
};


// --- ディレクションライト定数 ---
struct DirectionLight
{
	float3	direction_;
	float	intensity_;
	float3	color_;
};


// --- ポイントライト定数 ---
struct PointLight
{
	float3	position_;
	float	intensity_;
	float3	color_;
	float	range_;
};

#define POINT_LIGHT_MAX 256


// --- パーティクル定数 ---
struct ParticleConstants
{
	float3 emitterPosition_;
	float size_;
	float time_;
	float deltaTime_;
};