#include "MaterialTemplate.hlsl"

// Unifroms
float4x4 WorldView;
float4x4 Projection;

#ifdef _Skinning
	#define MaxNumBone 92
	cbuffer SkinCB
	{
		float4x4 SkinMatrices[MaxNumBone];	
	};
#endif

// Inputs
struct VSInput 
{
	float3 Pos 		 	 : POSITION;

#ifdef _Skinning
	float4 BlendWeights  : BLENDWEIGHTS;
	uint4  BlendIndices  : BLENDINDICES;
#endif

	float3 Normal		 : NORMAL;
	float2 Tex			 : TEXCOORD0;

#ifdef _NormalMap
	float3 Tangent		 : TANGENT;
	float3 Binormal      : BINORMAL;
#endif
};

// Outputs
struct VSOutput
{
	float4 PosVS : TEXCOORD0;
	float2 Tex   : TEXCOORD1;

#ifdef _NormalMap
	float3x3 TangentToView : TEXCOORD3;
#else
	float3 NormalVS 	   : TEXCOORD3;
#endif 
	
	float4 PosCS : SV_POSITION;
};

//-------------------------------------------------------------------------------------
void ShadowMapVS(VSInput input, 
			#if defined(_AlphaTest)
				 out float2 oTex   : TEXCOORD0,
			#endif
				 out float4 oPosCS : SV_POSITION)
{
#ifdef _Skinning
	float4x4 SkinMatrix = SkinMatrices[input.BlendIndices[0]] * input.BlendWeights[0] +
					      SkinMatrices[input.BlendIndices[1]] * input.BlendWeights[1] +
					      SkinMatrices[input.BlendIndices[2]] * input.BlendWeights[2] +
					      SkinMatrices[input.BlendIndices[3]] * input.BlendWeights[3];
#endif

	float4x4 wvp = mul(WorldView, Projection);

#ifdef _Skinning
	oPosCS = mul(float4(input.Pos, 1.0), mul(SkinMatrix, wvp));
#else
	oPosCS = mul(float4(input.Pos, 1.0), wvp);
#endif
	
#if defined(_AlphaTest)
	oTex = input.Tex;
#endif
}

void ShadowMapVSM(
			#if defined(_AlphaTest)
				  in float2 iTex        : TEXCOORD0,
			#endif
				  in float4 FragCoord   : SV_POSITION,
				  out float2 oFragDepth : SV_Target0)
{
#if defined(_AlphaTest)
	float4 tap = DiffuseMap.Sample(LinearSampler, iTex);
	if( tap.a < 0.01 ) discard;
#endif

	oFragDepth.x = FragCoord.z;
	oFragDepth.y = FragCoord.z * FragCoord.z;
	
	//vec2 dxdy = float2(dFdx(FragCoord.z), dFdy(FragCoord.z));
	//oFragDepth.y = FragCoord.z * FragCoord.z + 0.25 * dot(dxdy, dxdy);
}

//-------------------------------------------------------------------------------------
VSOutput GeneralVS(VSInput input)
{
	VSOutput output = (VSOutput)0;

#ifdef _Skinning
	float4x4 SkinMatrix = SkinMatrices[input.BlendIndices[0]] * input.BlendWeights[0] +
					      SkinMatrices[input.BlendIndices[1]] * input.BlendWeights[1] +
					      SkinMatrices[input.BlendIndices[2]] * input.BlendWeights[2] +
					      SkinMatrices[input.BlendIndices[3]] * input.BlendWeights[3];
#endif
	
	// calculate position in view space:
#ifdef _Skinning
	mat4 SkinWorldView = SkinMatrix * WorldView;
	output.PosVS = mul( float4(input.Pos, 1.0), SkinWorldView );
#else
	output.PosVS = mul( float4(input.Pos, 1.0), WorldView );
#endif

	// calculate view space normal.
#ifdef _Skinning
	float3 normal = normalize( mul(input.Normal, (float3x3)SkinWorldView) );
#else
	float3 normal = normalize( mul(input.Normal, (float3x3)WorldView) );
#endif

	// calculate tangent and binormal.
#ifdef _NormalMap
	#ifdef _Skinning
		float3 tangent = normalize( mul(input.Tangent, (float3x3)SkinWorldView) );
		float3 binormal = normalize( mul(input.Binormal, (float3x3)SkinWorldView) );
	#else
		float3 tangent = normalize( mul(input.Tangent, (float3x3)WorldView) );
		float3 binormal = normalize( mul(input.Binormal, (float3x3)WorldView) );
	#endif

	// actualy this is a world to tangent matrix, because we always use V * Mat.
	output.TangentToView = float3x3( tangent, binormal, normal);

	// transpose to get tangent to world matrix
	// oTangentToView = transpose(oTangentToView);
#else
	output.NormalVS = normal;
#endif
		
	output.PosCS = mul(output.PosVS, Projection);

	return output;
}

//---------------------------------------------------------------
void GBufferPS(in VSOutput input,
               out float4 oFragColor0 : SV_Target0,
			   out float4 oFragColor1 : SV_Target1 )
{

#ifdef _DiffuseMap
	float4 diffuseTap = DiffuseMap.Sample(LinearSampler, input.Tex);
	float3 albedo = diffuseTap.rgb;
	#ifdef _AlphaTest
		if( diffuseTap.a < 0.01 ) discard;
	#endif	
#else
	float3 albedo = DiffuseColor;
#endif

	// specular material
#ifdef _SpecularMap
	float4 specularTap = SpecularMap.Sample(LinearSampler, input.Tex);
	//vec3 specular = specularTap.rgb;
	float3 specular = specularTap.rrr;
	#ifdef _GlossyMap
		float shininess = specularTap.a;
	#else 
		float shininess = Shininess / 255.0;
	#endif
#else
	float3 specular = SpecularColor;
	float shininess = Shininess / 255.0;
#endif
	
	// normal map
#ifdef _NormalMap
	float3 normal = NormalMap.Sample(LinearSampler , input.Tex ).rgb * 2.0 - 1.0;
	normal = normalize( mul(normal, input.TangentToView) );
#else
	float3 normal = normalize(input.NormalVS);
#endif	
	
	normal = normal * 0.5 + 0.5;
	//CompressUnsignedNormalToNormalsBuffer(normal);	
	
	oFragColor0 = float4(normal.xyz, shininess);
	oFragColor1 = float4(albedo.rgb, dot(specular, float3(0.2126, 0.7152, 0.0722)) );	 // Specular luminance
}
