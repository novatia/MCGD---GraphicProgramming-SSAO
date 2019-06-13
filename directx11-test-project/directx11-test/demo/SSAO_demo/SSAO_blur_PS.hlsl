
struct VertexOut
{
	float4 posH : SV_POSITION;
	float3 toFarPlane : TEXCOORD0;
	float2 uv : TEXCOORD1;
};

cbuffer BlurCBufferPerFrame : register(b5)
{
	float texelWidth;
	float texelHeight;
	bool horizontalBlur;
};

cbuffer BlurCBufferSettings
{
	float weights[11] = { 0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f };
};

cbuffer BlurCBufferFixed
{
	static const int blurRadius = 5;
};

Texture2D SSAOMap : register (t13);
Texture2D normalDepthMap : register(t11);
//SamplerState samNormalDepth : register(s4);
SamplerState SSAOSampler : register(s4);

float4 main(VertexOut pin) : SV_TARGET
{
	float2 texOffset;
	if (horizontalBlur)
	{
		texOffset = float2(texelWidth, 0.0f);
	}
	else
	{
		texOffset = float2(0.0f, texelHeight);
	}
	float4 color = weights[5] * SSAOMap.SampleLevel(SSAOSampler, pin.uv, 0.0);
	float totalWeight = weights[5];
	float4 centerNormalDepth = normalDepthMap.SampleLevel(SSAOSampler, pin.uv, 0.0f);
	int i = 0;
	for (i = -blurRadius; i <= blurRadius; i++)
	{
		if (i == 0)
		{
			continue;
		}
		float2 tex = pin.uv + i * texOffset;
		float4 neighborNormalDepth = normalDepthMap.SampleLevel(SSAOSampler, tex, 0.0f);
		if (dot(neighborNormalDepth.xyz, centerNormalDepth.xyz) >= 0.8f &&
			abs(neighborNormalDepth.a - centerNormalDepth.a) <= 0.2f)
		{
			float weight = weights[i + blurRadius];
			color += weight * SSAOMap.SampleLevel(SSAOSampler, tex, 0.0);
			totalWeight += weight;
		}

	}
	return color / totalWeight;
}
