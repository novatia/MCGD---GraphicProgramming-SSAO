//SHARE THE INPUT STRUCTURE
struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

//SHARE THE INPUT STRUCTURE
struct VertexIn
{
	float3 posL : POSITION;
	float3 normalL : NORMAL;
	float4 tangentL : TANGENT;
	float2 uv : TEXCOORD;
};

//SHARE THE INPUT STRUCTURE
cbuffer PerObjectCB : register(b0)
{
	float4x4 W;
	float4x4 W_inverseTraspose;
	float4x4 WVP;
	float4x4 TexcoordMatrix;
	float4x4 WVPT_shadowMap;
	float4x4 WVPT_ssao;
	Material material;
};


//INPUT to the Normal/Depth PS
struct VertexOutNormalDepth
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
};



VertexOutNormalDepth main(VertexIn vin)
{
	VertexOutNormalDepth vout;

	vout.posW = mul(float4(vin.posL, 1.0f), W).xyz;
	vout.normalW = mul(vin.normalL, (float3x3)W_inverseTraspose);
	
	vout.posH = mul(float4(vin.posL, 1.0f), WVP);
	vout.uv = mul(float4(vin.uv, 0.f, 1.f), TexcoordMatrix).xy;

	return vout;
}
