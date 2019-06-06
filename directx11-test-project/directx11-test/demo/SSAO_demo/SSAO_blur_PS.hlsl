struct VertexOutBlurPass
{
	float4 posH : SV_POSITION;
	float2 uv : TEXCOORD;
};

cbuffer PerFrameCBufferBlur : register(b0)
{
	float texelWidth;
	float texelHeight;
}

cbuffer Settings : register(b1)
{
	float weights[11] = { 0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f };
}

cbuffer Fixed : register(b2)
{
	static const int blurRadius = 5;
}

Texture2D normalDepthMap : register(t0);
Texture2D inputImage : register(t1);

SamplerState samNormalDepth : register(s0);
SamplerState samInputImage : register(s1);

float4 main(VertexOutBlurPass pin, uniform bool horizontalBlur) : SV_TARGET
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
	float4 color = weights[5] * inputImage.SampleLevel(samInputImage, pin.uv, 0.0f);
	float totalWeight = weights[5];
	float4 centerNormalDepth = normalDepthMap.SampleLevel(samNormalDepth, pin.uv, 0.0f);
	for (float i = -blurRadius; i <= blurRadius; ++i)
	{
		if (i == 0)
		{
			continue;
		}
		float2 tex = pin.uv + i * texOffset;
		float4 neighborNormalDepth = normalDepthMap.SampleLevel(samNormalDepth, tex, 0.0f);
		if (dot(neighborNormalDepth.xyz, centerNormalDepth.xyz) >= 0.8f &&
			abs(neighborNormalDepth.a - centerNormalDepth.a) <= 0.2f)
		{
			float weight = weights[i + blurRadius];
			color += weight * inputImage.SampleLevel(samInputImage, tex, 0.0f);
			totalWeight += weight;
		}
	}
	return color / totalWeight;
}