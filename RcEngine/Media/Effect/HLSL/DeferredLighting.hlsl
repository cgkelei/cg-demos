#include "LingtingUtil.hlsl"

#define POINTLIGHT

float4x4 WorldView;
float4x4 Projection;
float4x4 InvProj;

Texture2D GBuffer0;	
Texture2D GBuffer1;	
Texture2D<float> DepthBuffer;	
Texture2D LightAccumulateBuffer;

//---------------------------------------------------------------------
// General VS for Dir/Point/Spot light
void DeferedVS(
#if defined(DIRLIGHT)
	in float2 iPos	    : POSITION,
#elif defined(POINTLIGHT) || defined(SPOTLIGHT)  
	in float3 iPos	    : POSITION,
#endif
	out float3 oViewRay : TEXCOORD1,
	out float4 oPosHS   : SV_POSITION)
{
#if defined(DIRLIGHT)

	oPosHS = float4(iPos, 0.0, 1.0);
	
	float4 posVS = mul(oPosHS, InvProj);
	oViewRay = float3(posVS.xy / posVS.z, 1.0);       // Proj to Z=1 plane
	
#elif defined(POINTLIGHT) || defined(SPOTLIGHT)   
	
	float4 posVS = mul(float4(iPos, 1.0), WorldView);
	oViewRay = posVS.xyz;
	
	oPosHS = mul(posVS, Projection);
#endif
}

//---------------------------------------------------------------------
// Deferred Directional Light
float4 DirectionalLightingPS(
	in float3 iViewRay	  : TEXCOORD1,
	in float4 iFragCoord  : SV_Position
	out float4 oFragColor : SV_Target0 )
{
	float4 final = float4(0.0);
	int3 sampleIndex = int3(iFragCoord.xy, 0);

	// Convert non-linear depth to view space linear depth
	float linearDepth = LinearizeDepth( DepthBuffer.Load(sampleIndex), CameraNearFar.xy );
	// View space lit position
	float3 positionVS = iViewRay * linearDepth;
	
	float4 visibility = float4(1.0);
	if (ShadowEnabled)
	{
		float4 posWS = mul( float4(positionVS, 1.0), InvView );
		
		int iCascadeSelected = 0;
		float percentLit = EvalCascadeShadow(posWS, iCascadeSelected);
		
		visibility = float4(percentLit);
		if (VisiualizeCascades)
			visibility *= vCascadeColorsMultiplier[iCascadeSelected];
	}
	
	// Fetch GBuffer
	float4 tap0 = GBuffer0.Load(sampleIndex);
				
	float shininess = tap0.a * 255.0;  			// specular power
	float3 N = normalize(tap0.xyz * 2.0 - 1.0);   // view space normal
	float3 L = normalize(-LightDirVS.xyz);		// light vector
		
	float nDotl = dot(L, N);
	if (nDotl > 0.0)
	{
		float3 V = normalize(-positionVS);
		float3 H = normalize(V + L);
		
		float3 diffuse = LightColor * nDotl;
		
		// Frensel in moved to calculate in shading pass
		float3 specular = CalculateSpecular(N, H, shininess) * LightColor * nDotl;
		
		final = float4(diffuse, Luminance(specular));
	}
		
	oFragColor = final * visibility; 
}

//---------------------------------------------------------------------
// Deferred Point Light
float4 PointLightingPS(
	in float3 iViewRay	  : TEXCOORD1,
	in float4 iFragCoord  : SV_Position
	out float4 oFragColor : SV_Target0 )
{
	float4 final = float4(0.0);
	int3 sampleIndex = int3(iFragCoord.xy, 0);

	// Convert non-linear depth to view space linear depth
	float linearDepth = LinearizeDepth( texture2D(DepthBuffer, tex).x, CameraNearFar.xy );
	// View space lit position
	float3 positionVS = iViewRay * (linearDepth / iViewRay.z);
	
	float3 L = normalize(LightPosVS.xyz - positionVS);
	
	// Fetch GBuffer
	float4 tap0 = GBuffer0.Load(sampleIndex);

	// Decode view space normal
	float3 N = normalize(tap0.xyz * 2.0 - 1.0); 
	float shininess = tap0.a * 255.0;
	
	float NdotL = dot(L, N);
	if (NdotL > 0.0)
	{
		float3 V = normalize(-positionVS);
		float3 H = normalize(V + L);
		
		// calculate attenuation
		float attenuation = CalcAttenuation(LightPosVS.xyz, positionVS, LightFalloff);
		
		float3 diffuseLight = LightColor * NdotL * attenuation;	
		float3 specularLight = CalculateSpecular(N, H, shininess) * diffuseLight; // Frensel in moved to calculate in shading pass
		
		final = float4(diffuseLight, Luminance(specularLight));
	}
		
	oFragColor = final; 
}


//---------------------------------------------------------------------
// Deferred Spot Light
float4 SpotLightingPS(
	in float3 iViewRay	  : TEXCOORD1,
	in float4 iFragCoord  : SV_Position
	out float4 oFragColor : SV_Target0 )
{
	float4 final = float4(0.0);
	int3 sampleIndex = int3(iFragCoord.xy, 0);

	// Convert non-linear depth to view space linear depth
	float linearDepth = LinearizeDepth( texture2D(DepthBuffer, tex).x, CameraNearFar.xy );
	
	// View space lit position
	float3 positionVS = iViewRay * (linearDepth / iViewRay.z);
	
	// Fetch GBuffer
	float4 tap0 = GBuffer0.Load(sampleIndex);

	float3 L = normalize(LightPosVS.xyz - positionVS);
	float spot = SpotLighting(L, LightDirVS.xyz, float2(LightPosVS.w, LightDirVS.w));
	if(spot > 0.0)
	{
		// Decode view space normal
		float3 N = normalize(tap0.xyz * 2.0 - 1.0); 
		float shininess = tap0.a * 255.0;
	
		float NdotL = dot(L, N);
		if (NdotL > 0.0)
		{
			float3 V = normalize(-positionVS);
			float3 H = normalize(V + L);
		
			// calculate attenuation
			float attenuation = spot * CalcAttenuation(LightPosVS.xyz, positionVS, LightFalloff);	
			float3 diffuse = LightColor * NdotL * attenuation;
		
			// Frensel in moved to calculate in shading pass
			float3 specular = CalculateSpecular(N, H, shininess) * LightColor * NdotL * attenuation;
		
			final = float4(diffuse, Luminance(specular));
		}
		
		float visibility = 1.0;
		if (ShadowEnabled)
			visibility = EvalCascadeShadow( mul(float4(positionVS, 1.0), InvView) );
	
		final *= visibility;
	}
		
	oFragColor = final; 
}

//--------------------------------------------------------
// Final Shading Pass
float4 DeferredShadingPS(
	in float3 iViewRay	  : TEXCOORD1,
	in float4 iFragCoord  : SV_Position
	out float4 oFragColor : SV_Target0 )
{
	float3 final = vec3(0, 0, 0);
	int3 sampleIndex = int3(iFragCoord.xy, 0);

	// Fetch GBuffer
	float4 tap0 = GBuffer0.Load(sampleIndex);
	float4 tap1 = GBuffer1.Load(sampleIndex);
	
	// Decode view space normal
	float3 N = normalize(tap0.rgb * 2.0 - 1.0); 
	float3 V = normalize(-oViewRay);

	// Get Diffuse Albedo and Specular
	float3 diffuseAlbedo = tap1.rgb;
	float3 specularAlbedo = tap1.aaa;
	float specularPower = tap0.a * 256.0;

	float4 lightColor = LightAccumulateBuffer.Load(sampleIndex);
	                    
	float3 diffueLight = lightColor.rgb;
	float3 specularLight = lightColor.a / (Luminance(diffueLight) + 1e-6) * diffueLight;

	// Approximate fresnel by N and V
	float3 fresnelTerm = CalculateAmbiemtFresnel(specularAlbedo, N, V);
	
	final =  diffueLight * diffuseAlbedo + ((specularPower + 2.0) / 8.0) * fresnelTerm * specularLight;
	final = final + float3(0.1, 0.1, 0.1) * diffuseAlbedo;

	oFragColor = float4(final, 1.0);
}