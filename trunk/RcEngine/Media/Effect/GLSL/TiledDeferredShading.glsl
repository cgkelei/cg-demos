#extension GL_ARB_compute_shader : enable
#pragma optimize(off)
#pragma debug(off)
#include "/LightingUtil.glsl"

#define WORK_GROUP_SIZE  32
#define NumGroupThreads (WORK_GROUP_SIZE  * WORK_GROUP_SIZE ) // Toal threads per Group
#define MAX_LIGHTS 256   // Max lights per tile

struct Light
{
	vec3 Color;
	float Range;

	vec3 Position;
	float padding1;

	vec3 Falloff;
	float padding2;
};

// Uniforms
layout (std140) uniform CSConstants
{
	mat4 View;
	mat4 InvProj;
	mat4 InvViewProj;
	vec4 ViewportDim; // zw for invDim
	//vec4 CameraNearFarAndProjRatio;
	vec2 CameraNearFar;
	vec2 ProjRatio;
	vec3 CameraOrigin;
	uint LightCount;
};

// Shader resource view
layout (binding = 0) uniform sampler2D DepthBuffer;
layout (binding = 1) uniform sampler2D GBuffer0; // Normal + Power
layout (binding = 2) uniform sampler2D GBuffer1; // Albedo + Specular Intensity

// Unorder resource view
layout (binding = 0, rgba32f) uniform writeonly image2D RWLightAccumulation;

// Structure Buffer
layout (std430, binding = 0) buffer LightsSRV
{
    Light Lights[];
};

shared uint TileMinZ;
shared uint TileMaxZ;

// Light list for the tile
shared uint TileLightList[MAX_LIGHTS];
shared uint NumTileLights;

//-----------------------------------------------------------------------------
// World space position
vec3 ReconstructWorldPosition(float zBuffer, uvec2 fragCoord)
{
	vec2 clipPos = (vec2(fragCoord) + 0.5) * ViewportDim.zw; // InvViewDim
    clipPos = clipPos * 2.0 - 1.0;
	
    vec4 worldPositionH = vec4(clipPos, zBuffer, 1.0) * InvViewProj;
    return worldPositionH.xyz / worldPositionH.w;
}

vec3 ReconstructViewPosition(float zBuffer, uvec2 fragCoord)
{
	vec2 clipPos = (vec2(fragCoord) + 0.5) * ViewportDim.zw; // InvViewDim
    clipPos = clipPos * 2.0 - 1.0;
	
	vec4 viewPositionH = vec4(clipPos, zBuffer, 1.0) * InvProj;
    return viewPositionH.xyz / viewPositionH.w; 
}

vec4 CreatePlane(vec3 p1, vec3 p2, vec3 p3)
{
	vec3 edge1 = p2 - p1;
	vec3 edge2 = p3 - p1;
	
	vec4 plane;
	plane.xyz = normalize(cross(edge1, edge2));
	plane.w = -dot(plane.xyz, p1);

	return plane;
}

void EvalulateAndAccumilateLight(Light light, vec3 litPos, vec3 N, vec3 V, float shininess,
                                 inout vec3 diffuseLight, inout vec3 specularLight)
{
	vec3 L = light.Position - litPos;
	float dist = length(L);
	if (dist > light.Range)
		return;

	L /= dist;
	//vec3 L = normalize(light.Position - litPos);
	vec3 H = normalize(V + L);
		
	// calculate attenuation
	float attenuation = CalcAttenuation(light.Position, litPos, light.Falloff);
	
	float NdotL = dot(L, N); 
	if (NdotL > 0.0)
	{
		vec3 lightRes = light.Color * NdotL * attenuation;

		diffuseLight += lightRes;
		specularLight += CalculateSpecular(N, H, shininess) * lightRes; // Frensel in moved to calculate in shading pass
	}
}

//--------------------------------------------------------------------------------
// CSMain 
layout (local_size_x = WORK_GROUP_SIZE , local_size_y = WORK_GROUP_SIZE , local_size_z = 1) in;
void main()
{
	ivec2 fragCoord = ivec2(gl_GlobalInvocationID.xy);

	float zBuffer = texelFetch(DepthBuffer, fragCoord, 0).r;
	vec3 worldPosition = ReconstructWorldPosition(zBuffer, fragCoord);
	
	// Compute view space z
	float viewSpaceZ = ProjRatio.y / (zBuffer - ProjRatio.x);
	
	// Work out Z bounds for our samples
	float minZSample = CameraNearFar.y; // Far
    float maxZSample = CameraNearFar.x; // Near
	
	// Avoid shading skybox/background or otherwise invalid pixels
	bool validPixel = (viewSpaceZ >= CameraNearFar.x && viewSpaceZ <  CameraNearFar.y);
	if (validPixel)
	{
		minZSample = min(minZSample, viewSpaceZ);
		maxZSample = max(maxZSample, viewSpaceZ);
	}	
	
	// Calculate tile Z bound
	if (gl_LocalInvocationIndex == 0)
	{
		NumTileLights = 0;
        TileMinZ = 0x7F7FFFFF;      // Max float;
        TileMaxZ = 0;
	}
	barrier();
	
	if(maxZSample >= minZSample)
	{
		atomicMin(TileMinZ, floatBitsToUint(minZSample));
		atomicMax(TileMaxZ, floatBitsToUint(maxZSample));
    }	
	barrier();
		
	float minTileZ = uintBitsToFloat(TileMinZ);
	float maxTileZ = uintBitsToFloat(TileMaxZ);
	
	vec3 frutumCornerPosVS[4];
			
	// Construct frustum planes
	vec4 frustumPlanes[6];

	// View frustum far plane corners
	frutumCornerPosVS[0] = ReconstructViewPosition(1.0, uvec2(gl_WorkGroupID.x * gl_WorkGroupSize.x, gl_WorkGroupID.y * gl_WorkGroupSize.y));
	frutumCornerPosVS[1] = ReconstructViewPosition(1.0, uvec2((gl_WorkGroupID.x+1) * gl_WorkGroupSize.x, gl_WorkGroupID.y * gl_WorkGroupSize.y));
	frutumCornerPosVS[2] = ReconstructViewPosition(1.0, uvec2((gl_WorkGroupID.x+1) * gl_WorkGroupSize.x, (gl_WorkGroupID.y+1) * gl_WorkGroupSize.y));
	frutumCornerPosVS[3] = ReconstructViewPosition(1.0, uvec2(gl_WorkGroupID.x * gl_WorkGroupSize.x, (gl_WorkGroupID.y+1) * gl_WorkGroupSize.y));

	for(int i=0; i <4; i++)
		frustumPlanes[i] = CreatePlane(vec3(0.0), frutumCornerPosVS[i], frutumCornerPosVS[(i+1)&3]);		
				
	// Near/Far
	frustumPlanes[4] = vec4(0, 0, 1, -minTileZ);
	frustumPlanes[5] = vec4(0, 0, -1, maxTileZ);

	// Cull lights for this tile	
	for (uint lightIndex = gl_LocalInvocationIndex; lightIndex < LightCount; lightIndex += NumGroupThreads)
	{
		vec4 lightPosVS = vec4(Lights[lightIndex].Position, 1.0) * View;

		// Intersect with each frustun plane
		bool inFrustum = true;
		for (int i = 0; i < 4; ++i)
		{
			float d = dot(frustumPlanes[i].xyz, lightPosVS.xyz) + frustumPlanes[i].z;
			inFrustum = inFrustum && (d >= -Lights[lightIndex].Range);
		}

		if (inFrustum)
		{
			uint listIndex = atomicAdd(NumTileLights, 1);		
            TileLightList[listIndex] = lightIndex;		
		}
	}
	barrier();
	
	// Only process onscreen pixels (tiles can span screen edges)
	if (all(lessThan(gl_GlobalInvocationID.xy, ViewportDim.xy)))
	{
		vec3 V = normalize(CameraOrigin - worldPosition);
		
		vec4 normalShininess = texelFetch(GBuffer0, fragCoord, 0);
		normalShininess.xyz = normalize(normalShininess.xyz); // Normal

		vec3 diffuseLight = vec3(0);
		vec3 specularLight = vec3(0);
		for (uint i = 0; i < NumTileLights; ++i)
		{
			uint lightIndex = TileLightList[i];
            Light light = Lights[lightIndex];
			
			vec3 L = light.Position - worldPosition;
			float dist = length(L);
			if (dist > light.Range)
				continue;

			L /= dist;
			//vec3 L = normalize(light.Position - litPos);
			vec3 H = normalize(V + L);
		
			// calculate attenuation
			float attenuation = CalcAttenuation(light.Position, worldPosition, light.Falloff);
	
			float NdotL = dot(L, normalShininess.xyz); 
			if (NdotL > 0.0)
			{
				vec3 lightRes = light.Color * NdotL * attenuation;

				diffuseLight += lightRes;

				attenuation *= dot(light.Color, V);
				specularLight += light.Color* attenuation;
				//specularLight += CalculateSpecular(normalShininess.xyz, H, normalShininess.w) * lightRes; // Frensel in moved to calculate in shading pass
			}

			//EvalulateAndAccumilateLight(light, worldPosition, normalShininess.xyz, V, normalShininess.w, diffuseLight, specularLight);
		}

		imageStore(RWLightAccumulation, fragCoord, vec4(diffuseLight, Luminance(specularLight)));
	}
}
