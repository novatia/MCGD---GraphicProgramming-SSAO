#define DIRECTIONAL_LIGHT_COUNT 2


struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

struct DirectionalLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 dirW;
};

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

cbuffer PerFrameCB : register(b1)
{
	DirectionalLight dirLights[DIRECTIONAL_LIGHT_COUNT];
	float3 eyePosW;
};

cbuffer RarelyChangedCB : register(b2)
{
	bool useShadowMap;
	float shadowMapResolution;
}

cbuffer AmbientOcclusionCBuffer : register(b3) //new struct
{
	float screenWidth;
	float screenHeight;
	uint sampleKernelSize;
	float noiseTextureDimension;
	float occlusionRadius;
	float SSAOPower;
	
};

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D glossTexture : register(t2);
Texture2D noiseTexture : register(t3); // new texture
Texture2D normalRoughnessTexture : register(t4); // new texture
StructuredBuffer<float4> sampleKernelBuffer : register(t5); // new structured buffer
Texture2D shadowMap : register(t10); // maybe depth texture?


SamplerState textureSampler : register(s0);
SamplerComparisonState shadowSampler : register(s10);





float4 CalculateAmbient(float4 matAmbient, float4 lightAmbient)
{
	return matAmbient * lightAmbient;
}


void CalculateDiffuseAndSpecular(
	float3 toLightW,
	float3 normalW,
	float3 toEyeW,
	float4 matDiffuse,
	float4 matSpec,
	float4 lightDiffuse,
	float4 lightSpec,
	float glossSample,
	inout float4 diffuseColor,
	inout float4 specularColor)
{

	// diffuse factor
	float Kd = dot(toLightW, normalW);

	[flatten]
	if (Kd > 0.0f)
	{
		// diffuse component
		diffuseColor = Kd * matDiffuse * lightDiffuse;

		// specular component
		float3 halfVectorW = normalize(toLightW + toEyeW);
		float exponent = exp2(9.0 * glossSample);
		float Ks = pow(max(dot(halfVectorW, normalW), 0.f), exponent);
		specularColor = Ks * matSpec * lightSpec;
	}
}


void ApplyAttenuation(
	float3 lightAttenuation,
	float distance,
	float falloff,
	inout float4 ambient,
	inout float4 diffuse,
	inout float4 specular)
{
	float attenuationFactor = 1.0f / dot(lightAttenuation, float3(1.0f, distance, distance*distance));
	ambient *= falloff;
	diffuse *= attenuationFactor * falloff;
	specular *= attenuationFactor * falloff;
}


void CalculateDirAndDistance(float3 pos, float3 target, out float3 dir, out float distance)
{
	float3 toTarget = target - pos;
	distance = length(toTarget);

	// now dir is normalize 
	toTarget /= distance;

	dir = toTarget;
}


float3 BumpNormalW(float2 uv, float3 normalW, float4 tangentW)
{
	float3 normalSample = normalTexture.Sample(textureSampler, uv).rgb;

	// remap the normal values inside the [-1,1] range from the [0,1] range
	float3 bumpNormalT = 2.f * normalSample - 1.f;

	// create the tangent space to world space matrix
	float3x3 TBN = float3x3(tangentW.xyz, tangentW.w * cross(normalW, tangentW.xyz), normalW);

	return mul(bumpNormalT, TBN);
}




void DirectionalLightContribution(Material mat, DirectionalLight light, float3 normalW, float3 toEyeW, float glossSample, out float4 ambient, out float4 diffuse, out float4 specular)
{
	// default values
	ambient = float4(0.f, 0.f, 0.f, 0.f);
	diffuse = float4(0.f, 0.f, 0.f, 0.f);
	specular = float4(0.f, 0.f, 0.f, 0.f);


	float3 toLightW = -light.dirW;

	// shading componets
	ambient = CalculateAmbient(mat.ambient, light.ambient);
	CalculateDiffuseAndSpecular(toLightW, normalW, toEyeW, mat.diffuse, mat.specular, light.diffuse, light.specular, glossSample, diffuse, specular);

}

// new function
float NdcZToScreenSpaceZ(const float depthNDC, const float4x4 projection)
{
	// depthNDC = A + B / depthV, where A = projection[2, 2] and B = projection[3,2].
	const float depthV = projection._m32 / (depthNDC - projection._m22);

	return depthV;
}

// new function
float3 ViewRayToViewPosition(const float3 normalizedViewRayV, const float depthNDC, const float4x4 projection)
{
	const float depthV = NdcZToScreenSpaceZ(depthNDC, projection);

	//
	// Reconstruct full view space position (x,y,z).
	// Find t such that p = t * normalizedViewRayV.
	// p.z = t * normalizedViewRayV.z
	// t = p.z / normalizedViewRayV.z
	//
	const float3 fragmentPositionViewSpace = (depthV / normalizedViewRayV.z) * normalizedViewRayV;

	return fragmentPositionViewSpace;
}

// new function
float2 OctWrap(float2 v)
{
	return (1.0 - abs(v.yx)) * (v.xy >= 0.0 ? 1.0 : -1.0);
}


//new function
float3 Decode(float2 encN)
{
	encN = encN * 2.0 - 1.0;

	float3 n;
	n.z = 1.0 - abs(encN.x) - abs(encN.y);
	n.xy = n.z >= 0.0 ? encN.xy : OctWrap(encN.xy);
	n = normalize(n);
	return n;
}

// new function
int2 NdcToScreenSpace(const float2 ndcPoint, const float screenTopLeftX, const float screenTopLeftY, const float screenWidth, const float screenHeight)
{
	const int2 viewportPoint = int2((ndcPoint.x + 1.0f) * screenWidth * 0.5f + screenTopLeftX,(1.0f - ndcPoint.y) * screenHeight * 0.5f + screenTopLeftY);
	return viewportPoint;
}


float4 main(VertexOut pin) : SV_TARGET
{
	//SSAO part
	
	const float2 noiseScale = float2(screenWidth / noiseTextureDimension,
			   screenHeight / noiseTextureDimension);
	const int3 fragmentPositionScreenSpace = int3(pin.posH.xy, 0);
	const float fragmentZNDC = shadowMap.Load(fragmentPositionScreenSpace).r; // add .r
	const float3 rayViewSpace = normalize(pin.rayViewSpace);
	const float4 fragmentPositionViewSpace = float4(ViewRayToViewPosition(pin.rayViewSpace, fragmentZNDC, projectionMatrix), 1.0f);
	const float2 normal = normalRoughnessTexture.Load(fragmentPositionScreenSpace).xy;
	const float3 normalViewSpace = normalize(Decode(normal));

	// Build a matrix to reorient the sample kerne
	// along current fragment normal vector.
	const float3 noiseVec = noiseTexture.SampleLevel(textureSampler, noiseScale * pin.uv, 0).xyz * 2.0f - 1.0f;
	const float3 tangentViewSpace = normalize(noiseVec - normalViewSpace * dot(noiseVec, normalViewSpace));
	const float3 bitangentViewSpace = normalize(cross(normalViewSpace, tangentViewSpace));
	const float3x3 sampleKernelRotationMatrix = float3x3(tangentViewSpace, bitangentViewSpace, normalViewSpace);
	float occlusionSum = 0.0f;

	for (uint i = 0U; i < sampleKernelSize; ++i) {
		// Rotate sample and get sample position in view space
		float4 rotatedSample = float4(mul(sampleKernelBuffer[i].xyz, sampleKernelRotationMatrix), 0.0f);
		float4 samplePositionViewSpace = fragmentPositionViewSpace + rotatedSample * occlusionRadius;

		float4 samplePositionNDC = mul(samplePositionViewSpace, projectionMatrix);
		samplePositionNDC.xy /= samplePositionNDC.w;

		const int2 samplePositionScreenSpace = NdcToScreenSpace(samplePositionNDC.xy,
			0.0f,
			0.0f,
			screenWidth,
			screenHeight);

		const bool isOutsideScreenBorders =
			samplePositionScreenSpace.x < 0.0f ||
			samplePositionScreenSpace.x > screenWidth ||
			samplePositionScreenSpace.y < 0.0f ||
			samplePositionScreenSpace.y > screenHeight;

		if (isOutsideScreenBorders == false) {
			float sampleZNDC = shadowMap.Load(int3(samplePositionScreenSpace, 0)).r; // add .r

			const float sampleZViewSpace = NdcZToScreenSpaceZ(sampleZNDC, projectionMatrix);

			const float rangeCheck =
				abs(fragmentPositionViewSpace.z - sampleZViewSpace) <
				occlusionRadius ? 1.0f : 0.0f;
			occlusionSum += (sampleZViewSpace <= samplePositionViewSpace.z ? 1.0f : 0.0f) * rangeCheck;
		}
	}

	float ambientAccessibility = 1.0f - (occlusionSum / sampleKernelSize);


	// Sharpen the contrast
	ambientAccessibility = saturate(pow(abs(ambientAccessibility), SSAOPower));

	
	//***********************************************************************************
	
	pin.normalW = normalize(pin.normalW);

	// make sure tangentW is still orthogonal to normalW and is unit leght even
	// after the rasterizer stage (interpolation) 
	pin.tangentW.xyz = pin.tangentW.xyz - (dot(pin.tangentW.xyz, pin.normalW)*pin.normalW);
	pin.tangentW.xyz = normalize(pin.tangentW.xyz);


	// bump mapping
	float3 bumpNormalW = BumpNormalW(pin.uv, pin.normalW, pin.tangentW);


	float litFactor = 1.f;
	if (useShadowMap)
	{
		litFactor = 0.f;

		pin.shadowPosH.xyz /= pin.shadowPosH.w;
		float depthNDC = pin.shadowPosH.z;

		const float delta = 1.f / shadowMapResolution;


		const float2 texelOffset[9] =
		{
			float2(0.f, delta),
			float2(delta, delta),
			float2(delta, 0.f),
			float2(delta, -delta),
			float2(0.f, -delta),
			float2(-delta, -delta),
			float2(-delta, 0.f),
			float2(-delta, +delta),
			float2(0.f, 0.f),
		};


		[unroll]
		for (int i = 0; i < 9; i++)
		{
			litFactor += shadowMap.SampleCmpLevelZero(shadowSampler, pin.shadowPosH.xy + texelOffset[i], depthNDC).r;
		}

		litFactor /= 9;
	}


	float glossSample = glossTexture.Sample(textureSampler, pin.uv).r;
	float3 toEyeW = normalize(eyePosW - pin.posW);

	float4 totalAmbient = float4(0.f, 0.f, 0.f, 0.f);
	float4 totalDiffuse = float4(0.f, 0.f, 0.f, 0.f);
	float4 totalSpecular = float4(0.f, 0.f, 0.f, 0.f);
	float4 ambient;
	float4 diffuse;
	float4 specular;


	// we want to apply our shadow map only to the directional key light
	float litFactors[2] = { litFactor, 1.f };

	[unroll]
	for (uint c = 0; c < DIRECTIONAL_LIGHT_COUNT; c++)
	{
		DirectionalLightContribution(material, dirLights[c], bumpNormalW, toEyeW, glossSample, ambient, diffuse, specular);
		totalAmbient += ambient * ambientAccessibility;
		totalDiffuse += diffuse * litFactors[c];
		totalSpecular += specular * litFactors[c];
	}


	float4 diffuseColor = diffuseTexture.Sample(textureSampler, pin.uv);
	float4 finalColor = diffuseColor * (totalAmbient + totalDiffuse) + totalSpecular;
	finalColor.a = diffuseColor.a * totalDiffuse.a;

	return finalColor;

}