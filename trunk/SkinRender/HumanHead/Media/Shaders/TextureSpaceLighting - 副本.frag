#version 330

#include "Common.include.glsl"

#define LIGHTCOUNT 3

uniform Light Lights[LIGHTCOUNT];

uniform mat4 World;

uniform float AlbedoGamma = 2.2;
uniform float DiffuseColorMix = 0.5;
uniform float EnvAmount = 0.2;
uniform float Roughness = 0.3;
uniform float Rho_s = 0.18;

uniform sampler2D   AlbedoTex;
uniform sampler2D   SpecTex; // spec amount in r, g, b, and roughness value over the surface
uniform sampler2D   NormalTex;	    
uniform sampler2D   ShadowTex[LIGHTCOUNT];
uniform sampler2D   Rho_d_Tex; // Torrance-Sparrow BRDF integrated over hemisphere for range of roughness and incident angles
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

	//Specular Constant
	//vec4 specTap = texture( SpecTex, oTex ); // rho_s and roughness
	//float m = specTap.w * 0.09 + 0.23;	 // m is specular roughness
	//float rho_s = specTap.x * 0.16 + 0.18;
	
	float m = Roughness;	 // m is specular roughness
	float rho_s = Rho_s;

	vec3 totalLight = vec3(0.0);
	for(int i = 0; i < LIGHTCOUNT; ++i)
	{
		// lighting parameters
		vec3 L = normalize( Lights[i].Position - oWorldPos ); // light vector
		
		float Latten = 1.0;
		//float Latten = 600 * 600 / dot( Light[i].Position - oWorldPos, Light[i].Position - oWorldPos);

		// compute linear depth
		float depth = NormalizedDepth(oLightDist[i], Lights[i].NearFarPlane.x, Lights[i].NearFarPlane.y);
		vec2 UV = ShadowTexCoord(oShadowCoord[i]);

		#ifdef SHADOW_PCF	
			float LShadow = ShadowPCF(UV, ShadowTex[i], depth - SHADOW_BIAS, 4, 1);
		#else
			#ifdef SHADOW_VSM 
				float LShadow = chebyshevUpperBound(depth, texture2D(ShadowTex[i], UV).ra);
			#else
				float LShadow = Shadow(UV, ShadowTex[i], depth - SHADOW_BIAS);
			#endif
		#endif

		float bumpDot_L = dot( N_bumped, L );
		vec3 pointLightColor = Lights[i].Color * Lights[i].Amount * LShadow * Latten;

		//// DIFFUSE LIGHT  
		vec3 Licosi = saturate(bumpDot_L) * pointLightColor;
		float rho_dt_L = 1.0 - rho_s * texture( Rho_d_Tex, vec2( bumpDot_L, m ) ).x;
		
		totalLight += Licosi * rho_dt_L;
	}

	//float occlusion = albedoTap.w;  
	float occlusion = 1.0;

	// add env light
	vec3 cubeTap = texture( IrradEnvMap, N_nonBumped ).xyz;
	totalLight += saturate( EnvAmount * cubeTap.xyz * occlusion);

	vec4 albedoTap = texture( AlbedoTex, oTex );
	vec3 albedo = albedoTap.xyz;
	//vec3 albedo = pow( albedoTap.xyz, vec3(1.0 / AlbedoGamma) );

	//// start mixing the diffuse lighting - re-compute non-blurred lighting per pixel to get maximum resolutions
    vec3 diffuseContrib = pow( albedo, vec3(DiffuseColorMix) ) * totalLight;        
    vec3 finalCol = diffuseContrib.xyz;  

	FragColor = vec4(finalCol,  1.0 );
}
