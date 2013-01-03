#version 330

#include "Common.include.glsl"

#define LIGHTCOUNT 3

uniform float Roughness = 0.3;
uniform float Rho_s = 0.18;
uniform float EnvAmount = 0.2;
uniform float SpecularIntensity = 1.88;

uniform Light Lights[LIGHTCOUNT];

uniform mat4 World;
uniform vec3 EyePos;

uniform float NearPlane = 0.1;
uniform float FarPlane = 20.0;

uniform sampler2D AlbedoTex;
uniform sampler2D NormalTex;
uniform sampler2D Rho_d_Tex; 
uniform sampler2D SpecTex;
uniform sampler2D   ShadowTex[LIGHTCOUNT];
uniform samplerCube IrradEnvMap;
uniform samplerCube GlossyEnvMap;

in vec3 oWorldPos;
in vec3 oWorldNormal;
in vec4 oShadowCoord[LIGHTCOUNT];
in float oLightDist[LIGHTCOUNT];
in vec2 oTex;

out vec4 FragColor;

void main()
{
	// compute world normal
	vec3 N_nonBumped = normalize( oWorldNormal );

	// compute bumped world normal
    vec3 objNormal = texture( NormalTex, oTex ).xyz * vec3( 2.0, 2.0, 2.0 ) - vec3( 1.0, 1.0, 1.0 );  
    vec3 N_bumped = normalize( mat3(World) * objNormal );

	// compute world view
	vec3 V = normalize(EyePos - oWorldPos);
	
	// lighting parameters
	vec3 L0 = normalize( Lights[0].Position - oWorldPos ); // point light 0 light vector
	vec3 L1 = normalize( Lights[1].Position - oWorldPos ); // point light 1 light vector
    vec3 L2 = normalize( Lights[2].Position - oWorldPos ); // point light 2 light vector


	//float L0atten = 600 * 600 / dot( Light[0].Position - oWorldPos, Light[0].Position - oWorldPos);
	//float L1atten = 600 * 600 / dot( Light[1].Position - oWorldPos, Light[1].Position - oWorldPos);
    //float L2atten = 600 * 600 / dot( Light[2].Position - oWorldPos, Light[2].Position - oWorldPos ); 

	float L0atten = 1.0;
	float L1atten = 1.0;
	float L2atten = 1.0;

	float depth0 = NormalizedDepth(oLightDist[0], Lights[0].NearFarPlane.x, Lights[0].NearFarPlane.y);
	float depth1 = NormalizedDepth(oLightDist[1], Lights[1].NearFarPlane.x, Lights[1].NearFarPlane.y);
	float depth2 = NormalizedDepth(oLightDist[2], Lights[2].NearFarPlane.x, Lights[2].NearFarPlane.y);

	vec2 UV0 = ShadowTexCoord(oShadowCoord[0]);
	vec2 UV1 = ShadowTexCoord(oShadowCoord[1]);
	vec2 UV2 = ShadowTexCoord(oShadowCoord[2]);

#ifdef SHADOW_PCF	
	float L0Shadow = ShadowPCF(UV0, ShadowTex[0], depth0 - SHADOW_BIAS, 4, 1);
	float L1Shadow = ShadowPCF(UV1, ShadowTex[1], depth1 - SHADOW_BIAS, 4, 1);
	float L2Shadow = ShadowPCF(UV2, ShadowTex[2], depth2 - SHADOW_BIAS, 4, 1);
#else
	#ifdef SHADOW_VSM 
	float L0Shadow = chebyshevUpperBound(depth0, texture2D(ShadowTex[0], UV0).ra);
	float L1Shadow = chebyshevUpperBound(depth1, texture2D(ShadowTex[1], UV1).ra);
	float L2Shadow = chebyshevUpperBound(depth2, texture2D(ShadowTex[2], UV2).ra);	
	#else
	float L0Shadow = Shadow(UV0, ShadowTex[0], depth0 - SHADOW_BIAS);
	float L1Shadow = Shadow(UV1, ShadowTex[1], depth1 - SHADOW_BIAS);
	float L2Shadow = Shadow(UV2, ShadowTex[2], depth2 - SHADOW_BIAS);
	#endif
#endif

	float bumpDot_L0 = dot( N_bumped, L0 );
    float bumpDot_L1 = dot( N_bumped, L1 );
    float bumpDot_L2 = dot( N_bumped, L2 );

	vec3 pointLight0Color = Lights[0].Color * Lights[0].Amount * L0Shadow * L0atten;
    vec3 pointLight1Color = Lights[1].Color * Lights[1].Amount * L1Shadow * L1atten;
    vec3 pointLight2Color = Lights[2].Color * Lights[2].Amount * L2Shadow * L2atten;

	vec3 Li0cosi = saturate(bumpDot_L0) * pointLight0Color;
    vec3 Li1cosi = saturate(bumpDot_L1) * pointLight1Color;
    vec3 Li2cosi = saturate(bumpDot_L2) * pointLight2Color;

	//Specular Constant
    //vec4 specTap = texture( SpecTex, oTex ); // rho_s and roughness
    //float m = specTap.w * 0.09 + 0.23;	 // m is specular roughness
    //float rho_s = specTap.x * 0.16 + 0.18;
	
	float m = Roughness;	 // m is specular roughness
    float rho_s = Rho_s;

	float rho_dt_L0 = 1.0 - rho_s * texture( Rho_d_Tex, vec2( bumpDot_L0, m ) ).x;
    float rho_dt_L1 = 1.0 - rho_s * texture( Rho_d_Tex, vec2( bumpDot_L1, m ) ).x;
    float rho_dt_L2 = 1.0 - rho_s * texture( Rho_d_Tex, vec2( bumpDot_L2, m ) ).x;

	vec3 E0 = Li0cosi * rho_dt_L0;
    vec3 E1 = Li1cosi * rho_dt_L1;
    vec3 E2 = Li2cosi * rho_dt_L2;

	vec4 albedoTap = texture( AlbedoTex, oTex );
	vec3 albedo = albedoTap.xyz;

	float occlusion = 1.0;
	vec3 cubeTap1 = texture( IrradEnvMap, N_nonBumped ).xyz;
	vec3 envLight = saturate( EnvAmount * cubeTap1.xyz * occlusion);

	//// start mixing the diffuse lighting - re-compute non-blurred lighting per pixel to get maximum resolutions
    vec3 diffuseContrib = albedo.xyz * ( E0 + E1 + E2 + envLight);        

	vec3 specularLight = vec3(0);  
    // Compute specular for each light  
    specularLight += Lights[0].Color * Lights[0].Amount * L0Shadow * KS_Skin_Specular(N_bumped, L0, V, m, rho_s, Rho_d_Tex );
	specularLight += Lights[1].Color * Lights[1].Amount * L1Shadow * KS_Skin_Specular(N_bumped, L1, V, m, rho_s, Rho_d_Tex );
	specularLight += Lights[2].Color * Lights[2].Amount * L2Shadow * KS_Skin_Specular(N_bumped, L2, V, m, rho_s, Rho_d_Tex );

	// Compute specular for env light 
	vec3 R_bumped = normalize( reflect( -V, N_bumped ) );	// refelct vector
    vec3 R_nonBumped = normalize( reflect( -V, N_nonBumped ) );

	// Gloss is the [0..1] value from your gloss map not decompressed in specular power
	float MipmapIndex = (1 - m) * (GlossyNumMipmap - 1);  
	vec3 AmbientSpecular = textureLod(GlossyEnvMap, R_bumped, MipmapIndex).xyz;
	specularLight += EnvAmount * AmbientSpecular * FresnelReflectance( normalize(R_bumped + V), V,  0.028 );
	
	specularLight *= SpecularIntensity;

	//specularLight = pow( specularLight.xyz, vec3(1.0 / 2.2) );
	//FragColor = vec4(specularLight,  diffuseContrib);

	FragColor = vec4(diffuseContrib + specularLight,  1.0 );
}