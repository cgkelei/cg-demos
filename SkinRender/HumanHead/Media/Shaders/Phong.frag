#version 330

#define LIGHTCOUNT 3
#define SHADOW_BAIS 0.0004
#define SHADOW_MAP_SIZE 1024

struct Light
{
	vec3 Position;	// Positon in world space
	vec3 Color;	
	float Amount;	
};	

uniform float Roughness = 0.3;
uniform float Rho_s = 0.18;

uniform Light Lights[LIGHTCOUNT];

uniform mat4 World;
uniform vec3 EyePos;

uniform sampler2D AlbedoTex;
uniform sampler2D NormalTex;
uniform sampler2D Rho_d_Tex; 
uniform sampler2D SpecTex;
uniform sampler2D   ShadowTex[LIGHTCOUNT];
uniform samplerCube EnvCube;

in vec3 oWorldPos;
in vec3 oWorldNormal;
in vec4 oShadowCoord[LIGHTCOUNT];
in vec2 oTex;

out vec4 FragColor;


#define saturate(value) clamp(value, 0.0, 1.0)

float FresnelReflectance( vec3 H, vec3 V, float F0 )  
{
	float base = 1.0 - dot( V, H );  
	float exponential = pow( base, 5.0 );  
	return F0 + (1.0 - F0) * exponential;
}

float KS_Skin_Specular( vec3 N, // Bumped surface normal  
                        vec3 L, // Points to light  
                        vec3 V, // Points to eye  
                        float m,  // Roughness  
                        float rho_s, // Specular brightness  
                        sampler2D beckmannTex )  
{  
	float result = 0.0;  
    float ndotl = dot( N, L );  
    if( ndotl > 0.0 )  
    {  
		vec3 h = L + V; // Unnormalized half-way vector  
		vec3 H = normalize( h );  
		float ndoth = dot( N, H );  
		float PH = pow( 2.0* texture(beckmannTex,vec2(ndoth,m)).y, 10.0 );  
		float F = FresnelReflectance( H, V, 0.028 );  
		float frSpec = max( PH * F / dot( h, h ), 0 );  
		result = ndotl * rho_s * frSpec; // BRDF * dot(N,L) * rho_s  
	}  
	return result;  
}  


float Shadow(vec4 shadowCoord, sampler2D shadowTex)
{
    vec3 shadowCoordWDiv = shadowCoord.xyz / shadowCoord.w;  

	//if((texture(shadowTex, shadowCoordWDiv.xy).r < (shadowCoordWDiv.z- SHADOW_BAIS)))
	//	return 0;
	//else
	//	return 1;

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
	// compute world normal
	vec3 N_nonBumped = normalize( oWorldNormal );

	// compute bumped world normal
    vec3 objNormal = texture( NormalTex, oTex ).xyz * vec3( 2.0, 2.0, 2.0 ) - vec3( 1.0, 1.0, 1.0 );  
    vec3 N_bumped = normalize( mat3(World) * objNormal );

	// compute view
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


	float L0Shadow = Shadow(oShadowCoord[0], ShadowTex[0]);
	float L1Shadow = Shadow(oShadowCoord[1], ShadowTex[1]);
	float L2Shadow = Shadow(oShadowCoord[2], ShadowTex[2]);
	//float L1Shadow = 1;
	//float L2Shadow = 1;
	
	//float L0Shadow = ShadowPCF(oShadowCoord[0], ShadowTex[0], 4, 1);
	//float L1Shadow = ShadowPCF(oShadowCoord[1], ShadowTex[1], 4, 1);
	//float L2Shadow = ShadowPCF(oShadowCoord[2], ShadowTex[2], 4, 1);


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

	//vec3 cubeTap1 = texture( EnvCube, N_nonBumped ).xyz;
	//vec3 envLight = saturate( EnvAmount * cubeTap1.xyz * occlusion);
	vec3 envLight = vec3(0.1, 0.1, 0.1);

	//// start mixing the diffuse lighting - re-compute non-blurred lighting per pixel to get maximum resolutions
    vec3 diffuseContrib = albedo.xyz * ( E0 + E1 + E2 + envLight);        
   

	vec3 specularContrib = vec3(0);  
    // Compute specular for each light  
    specularContrib += Lights[0].Color * Lights[0].Amount * L0Shadow * KS_Skin_Specular(N_bumped, L0, V, m, rho_s, Rho_d_Tex );
	specularContrib += Lights[1].Color * Lights[1].Amount * L1Shadow * KS_Skin_Specular(N_bumped, L1, V, m, rho_s, Rho_d_Tex );
	specularContrib += Lights[2].Color * Lights[2].Amount * L2Shadow * KS_Skin_Specular(N_bumped, L2, V, m, rho_s, Rho_d_Tex );


	FragColor = vec4(diffuseContrib + specularContrib,  1.0 );
}