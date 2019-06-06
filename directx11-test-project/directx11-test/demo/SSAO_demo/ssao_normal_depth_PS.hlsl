//SHARE THE INPUT STRUCTURE
struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

//SHARE THE INPUT STRUCTURE
cbuffer PerObjectCB : register(b0)
{
	float4x4 W;
	float4x4 W_inverseTraspose;
	float4x4 WVP;
	float4x4 TexcoordMatrix;
	float4x4 WVPT_shadowMap;
	float4x4 projectionMatrix; // new matrix
	Material material;
};


//INPUT to the Normal/Depth PS
struct VertexOut
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
};

float4 main(VertexOut pin) : SV_TARGET
{
	//renormalize interpolated normals.
	pin.normalW = normalize(pin.normalW);

	return float4(pin.normalW, pin.posH.z);
}