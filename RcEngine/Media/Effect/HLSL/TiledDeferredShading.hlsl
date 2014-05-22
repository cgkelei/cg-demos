#include "LightingUtil.hlsl"

// threads per group
#define WORK_GROUP_SIZE  32
#define MAX_LIGHTS 256   // Max lights per tile
#define NumGroupThreads (WORK_GROUP_SIZE  * WORK_GROUP_SIZE) // Toal threads per Group

struct Light
{
	float3 Color;
	float Range;
	
	float3 Position;
	float Paddding1;
	
	float3 Falloff;
	float Paddding2;
};

cbuffer CSConstants
{
	matrix View;
	matrix Projection;
	matrix InvProj;
	matrix InvViewProj;
	float4 ViewportDim; // zw for invDim
	float2 CameraNearFar;
	float2 ProjRatio;
	float3 CameraOrigin;
	uint LightCount;

};

// SRV
Texture2D<float> DepthBuffer;
Texture2D<float4> GBuffer0;
Texture2D<float4> GBuffer1;

StructuredBuffer<Light> Lights;

// UAV
RWTexture2D<float4> RWLightAccumulation;

// Shared memory
groupshared uint TileMinZ;
groupshared uint TileMaxZ;

// Light list for the tile
groupshared uint TileLightList[MAX_LIGHTS];
groupshared uint NumTileLights;

//-------------------------------------------------------------
float3 ReconstructWorldPosition(float zBuffer, uint2 fragCoord)
{
	float2 clipPos = (float2(fragCoord) + 0.5) * ViewportDim.zw; // InvViewDim
    clipPos = clipPos * 2.0 - 1.0;
	clipPos.y *= -1.0;
	
	float4 worldPositionH = mul( float4(clipPos, zBuffer, 1.0), InvViewProj );
    return worldPositionH.xyz / worldPositionH.w;
}

float3 ReconstructViewPosition(float zBuffer, uint2 fragCoord)
{
	float2 clipPos = (float2(fragCoord) + 0.5) * ViewportDim.zw; // InvViewDim
    clipPos = clipPos * 2.0 - 1.0;
	clipPos.y *= -1.0;
	
	float4 viewPositionH = mul( float4(clipPos, zBuffer, 1.0), InvProj );
    return viewPositionH.xyz / viewPositionH.w; 
}

// p1 is always camera origin in view space, float3(0, 0, 0)
float4 CreatePlaneEquation(/*float3 p1,*/ float3 p2, float3 p3)
{
	float4 plane;

	plane.xyz = normalize(cross(p2, p3));
	plane.w = 0;

	return plane;
}

//void EvalulateAndAccumilateLight(Light light, float3 litPos, float3 N, float3 V, float shininess,
//                                 inout float3 diffuseLight, inout float3 specularLight)
//{
//	float3 L = light.Position - litPos;
//	float dist = length(L);
//	if (dist > light.Range)
//		return;
//
//	L /= dist;
//	//vec3 L = normalize(light.Position - litPos);
//	float3 H = normalize(V + L);
//		
//	// calculate attenuation
//	float attenuation = CalcAttenuation(light.Position, litPos, light.Falloff);
//	
//	float NdotL = dot(L, N); 
//	if (NdotL > 0.0)
//	{
//		float3 lightRes = light.Color * NdotL * attenuation;
//
//		diffuseLight += lightRes;
//		specularLight += CalculateSpecular(N, H, shininess) * lightRes; // Frensel in moved to calculate in shading pass
//	}
//}

//----------------------------------------------------------------------------------
[numthreads(WORK_GROUP_SIZE, WORK_GROUP_SIZE, 1)]
void TiledDeferrdCSMain(
	uint3 DispatchThreadID	: SV_DispatchThreadID,
	uint3 GroupID			: SV_GroupID,
	uint  GroupIndex		: SV_GroupIndex)
{
	int3 pixelIndex = int3(DispatchThreadID.xy, 0);

	float zBuffer = DepthBuffer.Load(pixelIndex);
	
	// Compute view space z
	float viewSpaceZ = ProjRatio.y / (zBuffer - ProjRatio.x);	

	// Initialize per-tile variables
    if (GroupIndex == 0) 
	{
        TileMinZ = 0x7F7FFFFF;     
        TileMaxZ = 0;
		NumTileLights = 0;
    }
    GroupMemoryBarrierWithGroupSync();
	
	InterlockedMin(TileMinZ, asuint(viewSpaceZ));
    InterlockedMax(TileMaxZ, asuint(viewSpaceZ));
	GroupMemoryBarrierWithGroupSync();
		
	float minTileZ = asfloat(TileMinZ);
	float maxTileZ = asfloat(TileMaxZ);
	
	// Construct frustum planes
	float4 frustumPlanes[6];
	{
		float3 frustumCorner[4];

		// View frustum far plane corners
		frustumCorner[0] = ReconstructViewPosition(1.0, uint2(GroupID.x * WORK_GROUP_SIZE, GroupID.y *WORK_GROUP_SIZE));
		frustumCorner[1] = ReconstructViewPosition(1.0, uint2(GroupID.x * WORK_GROUP_SIZE, (GroupID.y+1) * WORK_GROUP_SIZE));
		frustumCorner[2] = ReconstructViewPosition(1.0, uint2((GroupID.x+1) * WORK_GROUP_SIZE, (GroupID.y+1) * WORK_GROUP_SIZE));
		frustumCorner[3] = ReconstructViewPosition(1.0, uint2((GroupID.x+1) * WORK_GROUP_SIZE, GroupID.y * WORK_GROUP_SIZE));

		for(int i=0; i <4; i++)
			frustumPlanes[i] = CreatePlaneEquation(frustumCorner[i], frustumCorner[(i+1)&3]);		
				
		// Near/far
		frustumPlanes[4] = float4(0.0f, 0.0f,  1.0f, -minTileZ);
		frustumPlanes[5] = float4(0.0f, 0.0f, -1.0f,  maxTileZ);
	}

	// Compute tile frustum using PerspectiveOffCenter. Intel's Code may be wrong, I deduce myself, need verify.
	//{
	//	// Work out scale/bias from [0, 1]
	//	float2 tileScale = float2(ViewportDim.xy) * rcp(2.0f * float2(WORK_GROUP_SIZE, WORK_GROUP_SIZE));
	//	float2 tileBias = tileScale - float2(GroupID.xy);

	//	frustumPlanes[0] = float4(Projection._11 * tileScale.x, 0, tileBias.x, 0);
	//	frustumPlanes[1] = float4(-Projection._11 * tileScale.x, 0, 1 - tileBias.x, 0);
	//	frustumPlanes[2] = float4(0, Projection._22 * tileScale.y, 1 - tileBias.y, 0);
	//	frustumPlanes[3] = float4(0, -Projection._22 * tileScale.y, tileBias.y, 0);

	//	// Near/far
	//	frustumPlanes[4] = float4(0.0f, 0.0f,  1.0f, -minTileZ);
	//	frustumPlanes[5] = float4(0.0f, 0.0f, -1.0f,  maxTileZ);

	//	// Normalize frustum planes (near/far already normalized)
	//	[unroll]
	//	for (uint i = 0; i < 4; ++i)
	//		frustumPlanes[i] *= rcp(length(frustumPlanes[i].xyz));
	//}

	// Cull lights for this tile	
	for (uint lightIndex = GroupIndex; lightIndex < LightCount; lightIndex += NumGroupThreads)
	{
		float4 lightPosVS = mul( float4(Lights[lightIndex].Position, 1.0), View);

		// Intersect with each frustun plane
		bool inFrustum = true;
		[unroll]
		for (int i = 0; i < 6; ++i)
		{
			float d = dot(frustumPlanes[i], float4(lightPosVS.xyz, 1.0));
			inFrustum = inFrustum && (d >= -Lights[lightIndex].Range);
		}

		if (inFrustum)
		{
			uint listIndex;
            InterlockedAdd(NumTileLights, 1, listIndex);
            TileLightList[listIndex] = lightIndex;
		}
	}
	GroupMemoryBarrierWithGroupSync();

	if (all(float2(DispatchThreadID.xy) < ViewportDim.xy))
	{
		float3 worldPosition = ReconstructWorldPosition(zBuffer, DispatchThreadID.xy);
		float3 V = normalize(CameraOrigin - worldPosition);
		
		float4 normalShininess = GBuffer0.Load(int3(DispatchThreadID.xy, 0));
		normalShininess.xyz = normalize(normalShininess.xyz); // Normal

		float3 diffuseLight = 0;
		float3 specularLight = 0;
		for (uint i = 0; i < NumTileLights; ++i)
		{
			uint lightIndex = TileLightList[i];
            Light light = Lights[lightIndex];
			
			float3 L = light.Position - worldPosition;
			float dist = length(L);
			if (dist > light.Range)
				continue;

			L /= dist;
			float3 H = normalize(V + L);
		
			// calculate attenuation
			float attenuation = CalcAttenuation(light.Position, worldPosition, light.Falloff);
	
			float NdotL = dot(L, normalShininess.xyz); 
			if (NdotL > 0.0)
			{
				float3 lightRes = light.Color * NdotL * attenuation;

				diffuseLight += lightRes;
				specularLight += CalculateSpecular(normalShininess.xyz, H, normalShininess.w) * lightRes; // Frensel in moved to calculate in shading pass
			}

			//EvalulateAndAccumilateLight(light, worldPosition, normalShininess.xyz, V, normalShininess.w, diffuseLight, specularLight);
		}


		//diffuseLight = frustumPlanes[3].rgb;
		RWLightAccumulation[DispatchThreadID.xy] = float4(diffuseLight, Luminance(specularLight));
	}
}