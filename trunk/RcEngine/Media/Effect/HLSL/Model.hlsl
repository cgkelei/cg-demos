#include "ModelVertexFactory.hlsl"

// Unifroms
float4x4 WorldView;
float4x4 Projection;

//-------------------------------------------------------------------------------------
VSOutput ModelVS(VSInput input)
{
	VSOutput output = (VSOutput)0;
	
	// calculate position in view space:
#ifdef _Skinning
	float4x4 Skin = CalculateSkinMatrix(input.BlendWeights, input.BlendIndices);
	float4x4 SkinWorldView = Skin * WorldView;
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
	 //output.TangentToView = transpose(output.TangentToView);
#else
	output.NormalVS = normal;
#endif
	
	output.Tex = input.Tex;
	output.PosCS = mul(output.PosVS, Projection);

	return output;
}
