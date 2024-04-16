// --- スクリーン座標系からNDCに変換 ---
float3 screenToNDC(const float2 position, const float2 screenSize)
{
	float x = 2.0f * position.x / screenSize.x - 1.0f;
	float y = 1.0f - 2.0f * position.y / screenSize.y;
	float z = 0.0f;

	return float3(x, y, z);
}


// --- 度数法から弧度法への変換 ---
float toRadian(const float degree)
{
	float theta = 3.141592653589 / 180.0f * degree;
	return theta;
}


// --- 疑似乱数の生成 ---
float rand(float n)
{
	return frac(sin(n) * 43758.5453123);
}


// --- Lambert拡散反射光の計算 ---
float3 calcLambertDiffuse(
	float3 lightDirection,
	float3 lightColor,
	float3 normal)
{
	// --- ピクセルの法線とライトの方向の内積の計算 ---
	float t = dot(normal, lightDirection) * -1.0f;

	// --- 内積の値を0以上にする ---
	t = max(0.0f, t);

	// --- 拡散反射光の計算 ---
	return lightColor * t;
}


// --- Phong鏡面反射光の計算 ---
float3 calcPhongSpecular(
	float3 lightDirection,
	float3 lightColor,
	float3 eyePos,
	float3 worldPos,
	float3 normal)
{
	// --- 反射ベクトルを求める ---
	float3 refVec = reflect(lightDirection, normal);

	// --- 光が当たったサーフェスから視点に伸びるベクトルを求める ---
	float3 toEye = eyePos - worldPos;
	toEye = normalize(toEye);

	// --- 鏡面反射の強さを求める ---
	float t = dot(refVec, toEye);

	// --- 鏡面反射の強さを0以上の値にする ---
	t = max(0.0f, t);

	// --- 鏡面反射の強さを絞る ---
	t = pow(t, 5.0f);

	// --- 鏡面反射光を求める ---
	return lightColor * t;
}