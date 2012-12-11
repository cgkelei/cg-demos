#version 330

#define LIGHTCOUNT 3
#define SHADOW_BAIS 0.0005f
#define SHADOW_MAP_SIZE 1024

struct Light
{
	vec3 Position;	// Positon in world space
	vec3 Color;	
	float Amount;	
};	

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
uniform samplerCube EnvCube;
 

in vec3 oWorldPos;
in vec3 oWorldNormal;
in vec4 oShadowCoord[LIGHTCOUNT];
in vec2 oTex;

out vec4 FragColor;

#define saturate(value) clamp(value, 0.0, 1.0)

float Shadow(vec4 shadowCoord, sampler2D shadowTex)
{
    vec3 shadowCoordWDiv = shadowCoord.xyz / shadowCoord.w;  
    return (texture(shadowTex, shadowCoordWDiv.xy).r < (shadowCoordWDiv.z- SHADOW_BAIS)) ? 0.0 : 1.0;
}


float ShadowPCF(vec4 shadowCoord, sampler2D shadowTex, int samples, float width)
{
	vec3 shadowCoordWDiv = shadowCoord.xyz / shadowCoord.w;

    float shadow = 0.0;
    float offset = (samples - 1.0) / 2.0;
   
    for (float x = -offset; x <= offset; x += 1.0) 
	{
        for (float y = -offset; y <= offset; y += 1.0)
		{
            vec2 tex = shadowCoordWDiv.xy + width * vec2(x, y) / SHADOW_MAP_SIZE;
            shadow += (texture(shadowTex, tex).r < (shadowCoordWDiv.z- SHADOW_BAIS)) ? 0.0 : 1.0;
        }
    }
    shadow /= samples * samples;
    return shadow;
}


void main()
{
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

	//float L0Shadow = Shadow(oShadowCoord[0], ShadowTex[0]);
	//float L1Shadow = Shadow(oShadowCoord[1], ShadowTex[1]);
	//float L2Shadow = Shadow(oShadowCoord[2], ShadowTex[2]);
	
	float L0Shadow = ShadowPCF(oShadowCoord[0], ShadowTex[0], 4, 1);
	float L1Shadow = ShadowPCF(oShadowCoord[1], ShadowTex[1], 4, 1);
	float L2Shadow = ShadowPCF(oShadowCoord[2], ShadowTex[2], 4, 1);

	// compute world normal
	vec3 N_nonBumped = normalize( oWorldNormal );

	// compute bumped world normal
    vec3 objNormal = texture( NormalTex, oTex ).xyz * vec3( 2.0, 2.0, 2.0 ) - vec3( 1.0, 1.0, 1.0 );  
    vec3 N_bumped = normalize( mat3(World) * objNormal );

	float bumpDot_L0 = dot( N_bumped, L0 );
    float bumpDot_L1 = dot( N_bumped, L1 );
    float bumpDot_L2 = dot( N_bumped, L2 );

	vec3 pointLight0Color = Lights[0].Color * Lights[0].Amount * L0Shadow * L0atten;
    vec3 pointLight1Color = Lights[1].Color * Lights[1].Amount * L1Shadow * L1atten;
    vec3 pointLight2Color = Lights[2].Color * Lights[2].Amount * L2Shadow * L2atten;

	//// Specular Constant
 //   //vec4 specTap = texture( SpecTex, oTex ); // rho_s and roughness
 //   //float surface_roughness = specTap.w * 0.09 + 0.23;	 // m is specular roughness
 //   //float rho_s = specTap.x * 0.16 + 0.18;

	//// no specular map
	//float Roughness = 0.3;
	//float Rho_s = 0.18;

	//// DIFFUSE LIGHT  
    vec3 Li1cosi = saturate(bumpDot_L0) * pointLight0Color;
    vec3 Li2cosi = saturate(bumpDot_L1) * pointLight1Color;
    vec3 Li3cosi = saturate(bumpDot_L2) * pointLight2Color;

    float rho_dt_L0 = 1.0 - Rho_s * texture( Rho_d_Tex, vec2( bumpDot_L0, Roughness ) ).x;
    float rho_dt_L1 = 1.0 - Rho_s * texture( Rho_d_Tex, vec2( bumpDot_L1, Roughness ) ).x;
    float rho_dt_L2 = 1.0 - Rho_s * texture( Rho_d_Tex, vec2( bumpDot_L2, Roughness ) ).x;

	vec3 E0 = Li1cosi * rho_dt_L0;
    vec3 E1 = Li2cosi * rho_dt_L1;
    vec3 E2 = Li3cosi * rho_dt_L2;

	vec4 albedoTap = texture( AlbedoTex, oTex );
	vec3 albedo = albedoTap.xyz;
    //vec3 albedo = pow( albedoTap.xyz, vec3(AlbedoGamma) );
	//////float occlusion = albedoTap.w;  
	float occlusion = 1.0;

	//vec3 cubeTap1 = texture( EnvCube, N_nonBumped ).xyz;
	//vec3 envLight = saturate( EnvAmount * cubeTap1.xyz * occlusion);
	vec3 envLight = vec3(0.0, 0.0, 0.0);

	//// start mixing the diffuse lighting - re-compute non-blurred lighting per pixel to get maximum resolutions
    vec3 diffuseContrib = pow( albedo.xyz, vec3(DiffuseColorMix) ) * ( E0 + E1 + E2 + envLight);        
    vec3 finalCol = diffuseContrib.xyz;  

	//vec3 lit = (E0 + E1 + E2);
	//lit = clamp( lit, 0.0, 1.0);
	//FragColor = vec4(lit, 1);

	//FragColor = vec4(N_bumped, lit * 1.0);

	FragColor = vec4(finalCol,  1.0 );
}
