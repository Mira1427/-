#include "spriteBatch.hlsli"

VS_OUT main(float4 position : POSITION, float2 texcoord : TEXCOORD, column_major float4x4 ndcTransform : NDC_TRANSFORM, float4 texcoordTransform : TEXCOORD_TRANSFORM, float4 color : COLOR ) {


	VS_OUT vout;
	vout.position = mul(position, ndcTransform);
	vout.texcoord = texcoord * texcoordTransform.zw * texcoordTransform.xy;
	vout.color = color;

	return vout;
}