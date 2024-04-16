// ===== �萔�p�w�b�_�[ =============================================================================================================================

// ===== �萔 =======================================================================================================================================

// --- �u�����h�X�e�[�g�p ---
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

#define PI 3.14159265358979

// ===== �萔�o�b�t�@�p�\���� ===================================================================================================================

// --- �V�[���萔 ---
struct SceneConstants
{
	row_major float4x4	viewProjection_;
	row_major float4x4  invViewProjection_;
	float4				cameraPosition_;
};


// --- �E�B���h�E�萔 ---
struct WindowConstants
{
	float2 size_;
};


// --- �f�B�]���u�萔 ---
struct DissolveConstants
{
	float amount_;
};


// --- �V�F�[�_�[�g�C�萔 ---
cbuffer ShaderToyConstants : register(b5)
{
	float4 iMouse;
	float4 iChannelResolution[4];
	float4 iResolution;
	float iTime;
	float iFrame;
}


// --- �����萔 ---
struct AmbientLight
{
	float3 color_;
};


// --- �f�B���N�V�������C�g�萔 ---
struct DirectionLight
{
	float3	direction_;
	float	intensity_;
	float3	color_;
};


// --- �|�C���g���C�g�萔 ---
struct PointLight
{
	float3	position_;
	float	intensity_;
	float3	color_;
	float	range_;
};

#define POINT_LIGHT_MAX 256


// --- �p�[�e�B�N���萔 ---
struct ParticleConstants
{
	float3 emitterPosition_;
	float size_;
	float time_;
	float deltaTime_;
};