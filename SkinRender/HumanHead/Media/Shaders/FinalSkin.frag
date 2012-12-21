#version 330

#include "Common.include.glsl"

#define LIGHTCOUNT 3
#define SHADOW_BAIS 0.0004f
#define SHADOW_MAP_SIZE 1024

struct Light
{
	vec3 Position;	// Positon in world space
	vec3 Color;	
	float Amount;	
};	

uniform Light Lights[LIGHTCOUNT];
uniform mat4 World;

uniform sampler2D AlbedoTex;
uniform sampler2D NormalTex;
uniform sampler2D Rho_d_Tex; 
uniform sampler2D SpecTex;
uniform sampler2D IrradTex[6];
uniform sampler2D ShadowTex[LIGHTCOUNT];

uniform vec3 GaussWeights[6];
uniform vec3 EyePos;

uniform float DiffuseColorMix = 0.5;
uniform float Roughness = 0.3;	
uniform float Rho_s = 0.18;


in vec3 oWorldPos;
in vec3 oWorldNormal;
in vec4 oShadowCoord[LIGHTCOUNT];
in vec2 oTex;

out vec4 FragColor;


void main()
{
	// lighting parameters
	vec3 L0 = normalize( Lights[0].Position - oWorldPos ); // point light 0 light vector
	vec3 L1 = normalize( Lights[1].Position - oWorldPos ); // point light 1 light vector
    vec3 L2 = normalize( Lights[2].Position - oWorldPos ); // point light 2 light vector

	// View vector
	vec3 V = normalize( EyePos - oWorldPos );

	vec3 N_nonBumped = normalize( oWorldNormal );

	// compute bumped world normal
    vec3 objNormal = texture( NormalTex, oTex ).xyz * vec3( 2.0, 2.0, 2.0 ) - vec3( 1.0, 1.0, 1.0 );  
    vec3 N_bumped = normalize( mat3(World) * objNormal );

#ifdef SHADOW_PCF	
	float L0Shadow = ShadowPCF(oShadowCoord[0], ShadowTex[0], vec2(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE), SHADOW_BAIS, 4, 1);
	float L1Shadow = ShadowPCF(oShadowCoord[1], ShadowTex[1], vec2(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE), SHADOW_BAIS, 4, 1);
	float L2Shadow = ShadowPCF(oShadowCoord[2], ShadowTex[2], vec2(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE), SHADOW_BAIS, 4, 1);
#else
	float L0Shadow = Shadow(oShadowCoord[0], ShadowTex[0], SHADOW_BAIS);
	float L1Shadow = Shadow(oShadowCoord[1], ShadowTex[1], SHADOW_BAIS);
	float L2Shadow = Shadow(oShadowCoord[2], ShadowTex[2], SHADOW_BAIS);
#endif

	vec3 diffuseLight = vec3(0.0);
	 
	vec4 irrad0Tap = texture(IrradTex[0], oTex);
	vec4 irrad1Tap = texture(IrradTex[1], oTex);
	vec4 irrad2Tap = texture(IrradTex[2], oTex);
	vec4 irrad3Tap = texture(IrradTex[3], oTex);
	vec4 irrad4Tap = texture(IrradTex[4], oTex);
	vec4 irrad5Tap = texture(IrradTex[5], oTex);

	diffuseLight += GaussWeights[0] * irrad0Tap.xyz;  
	diffuseLight += GaussWeights[1] * irrad1Tap.xyz;  
	diffuseLight += GaussWeights[2] * irrad2Tap.xyz;  
	diffuseLight += GaussWeights[3] * irrad3Tap.xyz;  
	diffuseLight += GaussWeights[4] * irrad4Tap.xyz;  
	diffuseLight += GaussWeights[5] * irrad5Tap.xyz; 
	
    // Renormalize diffusion profiles to white  
    vec3 normConst = GaussWeights[0] + GaussWeights[1] + GaussWeights[2] + GaussWeights[3] + GaussWeights[4] + GaussWeights[5];  
    diffuseLight /= normConst; // Renormalize to white diffuse light   

	// Determine skin color from a diffuseColor map
	vec3 albedo = texture( AlbedoTex, oTex ).xyz;
	diffuseLight *=  pow( albedo.xyz, vec3(1.0 - DiffuseColorMix) );

	// Constant for specular calculation
	// Use constant parameters m(roughness) and rho_s(intensity) for specular calculation
	
	float m = Roughness;
	float rho_s = Rho_s;

	//vec4 specTap = texture( SpecTex, oTex ); // rho_s and roughness
    //float m = specTap.w * 0.09 + 0.23;	 // m is specular roughness
    //float rho_s = specTap.x * 0.16 + 0.18;
	
	//Energy conservation (optional) - rho_s and m can be painted
	float finalScale = 1 - rho_s * texture(Rho_d_Tex, vec2(dot(N_bumped, V), m)).x;
	diffuseLight *= finalScale;

	vec3 specularLight = vec3(0);  
    // Compute specular for each light  
    specularLight += Lights[0].Color * Lights[0].Amount * L0Shadow * KS_Skin_Specular(N_bumped, L0, V, m, rho_s, Rho_d_Tex );
	specularLight += Lights[1].Color * Lights[1].Amount * L1Shadow * KS_Skin_Specular(N_bumped, L1, V, m, rho_s, Rho_d_Tex );
	specularLight += Lights[2].Color * Lights[2].Amount * L2Shadow * KS_Skin_Specular(N_bumped, L2, V, m, rho_s, Rho_d_Tex );
	
	//specularLight = pow( specularLight.xyz, vec3(1.0 / 2.2) );
    //FragColor = vec4( /*diffuseLight +*/ specularLight, saturate(length(diffuseLight) + 1.0) );  	

	FragColor = vec4( diffuseLight + specularLight, 1.0 );  
}