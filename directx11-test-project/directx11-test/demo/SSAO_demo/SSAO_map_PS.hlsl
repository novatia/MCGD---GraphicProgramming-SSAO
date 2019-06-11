#define sampleCount 14

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

struct VertexOutAmbientOcclusion
{
	float4 posH : SV_POSITION;
	float3 toFarPlane : TEXCOORD0;
	float2 uv : TEXCOORD1;
};

cbuffer PerObjectCBAmbientOcclusion : register(b3)
{
	float4x4 viewToTexSpace; // Proj*Tex
	float4 offsetVectors[14];
	float4 frustumCorners[4];
	float occlusionRadius = 0.5f;
	float occlusionFadeStart = 0.2f;
	float occlusionFadeEnd = 2.0f;
	float surfaceEpsilon = 0.05f;
};

Texture2D normalDepthMap : register(t11);
Texture2D randomVecMap : register(t12);
SamplerState samNormalDepth : register(s1);
SamplerState samRandomVec : register(s2);

float OcclusionFunction(float distZ)
{
	float occlusion = 0.0f;
	if (distZ > surfaceEpsilon)
	{
		float fadeLength = occlusionFadeEnd - occlusionFadeStart;
		occlusion = saturate((occlusionFadeEnd - distZ ) / fadeLength);
	}
	return occlusion;
}


float main(VertexOutAmbientOcclusion pin) : SV_TARGET
{
	float4 normalDepth = normalDepthMap.SampleLevel(samNormalDepth, pin.uv, 0.0f);
	float3 n = normalDepth.xyz;
	float pz = normalDepth.w;
	float3 p = (pz / pin.toFarPlane.z)*pin.toFarPlane;
	float3 randVec = 2.0f * randomVecMap.SampleLevel(samRandomVec, 4.0f*pin.uv, 0.0f).rgb - 1.0f;
	float occlusionSum = 0.0f;
	for (int i = 0; i < sampleCount; ++i)
	{
		float3 offset = reflect(offsetVectors[i].xyz, randVec);
		float flip = sign(dot(offset, n));
		float3 q = p + flip * occlusionRadius*offset;
		float4 projQ = mul(float4(q, 1.0f), viewToTexSpace);
		projQ /= projQ.w;
		float rz = normalDepthMap.SampleLevel(samNormalDepth, projQ.xy, 0.0f).a;
		float3 r = (rz / q.z)*q;
		float distZ = p.z - r.z;
		float dp = max(dot(n, normalize(r - p)), 0.0f);
		float occlusion = dp * OcclusionFunction(distZ);
		occlusionSum += occlusion;
	}
	occlusionSum /= sampleCount;
	float access = 1.0f - occlusionSum;
	return saturate(pow(access, 4.0f));
}
