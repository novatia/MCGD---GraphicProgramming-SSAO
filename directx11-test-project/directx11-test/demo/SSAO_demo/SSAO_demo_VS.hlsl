
struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

struct VertexInAmbientOcclusion
{
	float3 posL : POSITION;
	float3 toFarPlaneIndex : NORMAL;
	float4 uv : TEXCOORD;
};

struct VertexOutAmbientOcclusion
{
	float4 posH : SV_POSITION;
	float3 toFarPlane : TEXCOORD0;
	float2 uv : TEXCOORD1;
};

cbuffer PerObjectCB : register(b0)
{
	float4x4 W;
	float4x4 W_inverseTraspose;
	float4x4 WVP;
	float4x4 TexcoordMatrix;
	float4x4 WVPT_shadowMap;
	Material material;
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

VertexOutAmbientOcclusion main(VertexInAmbientOcclusion vin)
{
	VertexOutAmbientOcclusion vout;

	vout.posH = float4(vin.posL, 1.0f);
	vout.uv = vin.uv.xy;
	vout.toFarPlane = frustumCorners[vin.toFarPlaneIndex.x].xyz;

	return vout;
}
