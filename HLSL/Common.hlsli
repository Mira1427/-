// --- �X�N���[�����W�n����NDC�ɕϊ� ---
float3 screenToNDC(const float2 position, const float2 screenSize)
{
	float x = 2.0f * position.x / screenSize.x - 1.0f;
	float y = 1.0f - 2.0f * position.y / screenSize.y;
	float z = 0.0f;

	return float3(x, y, z);
}


// --- �x���@����ʓx�@�ւ̕ϊ� ---
float toRadian(const float degree)
{
	float theta = 3.141592653589 / 180.0f * degree;
	return theta;
}


// --- �^�������̐��� ---
float rand(float n)
{
	return frac(sin(n) * 43758.5453123);
}


// --- Lambert�g�U���ˌ��̌v�Z ---
float3 calcLambertDiffuse(
	float3 lightDirection,
	float3 lightColor,
	float3 normal)
{
	// --- �s�N�Z���̖@���ƃ��C�g�̕����̓��ς̌v�Z ---
	float t = dot(normal, lightDirection) * -1.0f;

	// --- ���ς̒l��0�ȏ�ɂ��� ---
	t = max(0.0f, t);

	// --- �g�U���ˌ��̌v�Z ---
	return lightColor * t;
}


// --- Phong���ʔ��ˌ��̌v�Z ---
float3 calcPhongSpecular(
	float3 lightDirection,
	float3 lightColor,
	float3 eyePos,
	float3 worldPos,
	float3 normal)
{
	// --- ���˃x�N�g�������߂� ---
	float3 refVec = reflect(lightDirection, normal);

	// --- �������������T�[�t�F�X���王�_�ɐL�т�x�N�g�������߂� ---
	float3 toEye = eyePos - worldPos;
	toEye = normalize(toEye);

	// --- ���ʔ��˂̋��������߂� ---
	float t = dot(refVec, toEye);

	// --- ���ʔ��˂̋�����0�ȏ�̒l�ɂ��� ---
	t = max(0.0f, t);

	// --- ���ʔ��˂̋������i�� ---
	t = pow(t, 5.0f);

	// --- ���ʔ��ˌ������߂� ---
	return lightColor * t;
}