
struct VertexOut
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float4 tangentW : TANGENT;
	float2 uv : TEXCOORD;
	float4 shadowPosH : SHADOWPOS;
	float3 rayViewSpace : VIEW_RAY; // new thing
};

cbuffer BlurCBuffer : register(b0) // new struct
{
	uint noiseTextureDimension;
};

SamplerState textureSampler : register (s0);
Texture2D bufferTexture : register(t0);

float main(VertexOut pin) : SV_TARGET
{
	
	float w;
	float h;
	bufferTexture.GetDimensions(w, h);
	const float2 texelSize = 1.0f / float2(w, h);
	float result = 0.0f;
	const float hlimComponent = -float(noiseTextureDimension) * 0.5f + 0.5f;
	const float2 hlim = float2(hlimComponent, hlimComponent);
	[unroll(4)] // add unroll
	for (uint i = 0U; i < noiseTextureDimension; ++i) {
		for (uint j = 0U; j < noiseTextureDimension; ++j) {
			const float2 offset = (hlim + float2(float(i), float(j))) * texelSize;
			result += bufferTexture.Sample(textureSampler, pin.uv + offset).r;
		}
	}

	return result / float(noiseTextureDimension * noiseTextureDimension);

}
