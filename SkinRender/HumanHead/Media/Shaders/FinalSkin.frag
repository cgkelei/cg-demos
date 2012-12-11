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

uniform sampler2D AlbedoTex;
uniform sampler2D NormalTex;
uniform sampler2D Rho_d_Tex; 
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

float FresnelReflectance( vec3 H, vec3 V, float F0 )  
{
	float base = 1.0 - dot( V, H );  
	float exponential = pow( base, 5.0 );  
	return exponential + F0 * ( 1.0 - exponential );  
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
		float PH = pow( 2.0* texture(beckmannTex,vec2(ndoth,m)).r, 10.0 );  
		float F = FresnelReflectance( H, V, 0.028 );  
		float frSpec = max( PH * F / dot( h, h ), 0 );  
		result = ndotl * rho_s * frSpec; // BRDF * dot(N,L) * rho_s  
	}  
	return result;  
}  

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

	float L0Shadow = ShadowPCF(oShadowCoord[0], ShadowTex[0], 4, 1);
	float L1Shadow = ShadowPCF(oShadowCoord[1], ShadowTex[1], 4, 1);
	float L2Shadow = ShadowPCF(oShadowCoord[2], ShadowTex[2], 4, 1);

	// View vector
	vec3 V = normalize( EyePos - oWorldPos );

	vec3 N_nonBumped = normalize( oWorldNormal );

	// compute bumped world normal
    vec3 objNormal = texture( NormalTex, oTex ).xyz * vec3( 2.0, 2.0, 2.0 ) - vec3( 1.0, 1.0, 1.0 );  
    vec3 N_bumped = normalize( mat3(World) * objNormal );

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
	diffuseLight *=  pow( albedo.xyz, vec3(DiffuseColorMix) );

	// Constant for specular calculation
	// Use constant parameters m(roughness) and rho_s(intensity) for specular calculation
	float m = Roughness;
	float rho_s = Rho_s;
	
	//Energy conservation (optional) - rho_s and m can be painted
	//float finalScale = 1 - rho_s * texture(Rho_d_Tex, vec2(dot(N_bumped, L), m)).x;
	//diffuseLight *= finalScale;

	vec3 specularLight = vec3(0);  
    // Compute specular for each light  
    //specularLight += Lights[0].Color * Lights[0].Amount * L0Shadow * KS_Skin_Specular(N_bumped, L0, V, m, rho_s, Rho_d_Tex );
	//specularLight += Lights[1].Color * Lights[1].Amount * L1Shadow * KS_Skin_Specular(N_bumped, L1, V, m, rho_s, Rho_d_Tex );
	//specularLight += Lights[2].Color * Lights[2].Amount * L2Shadow * KS_Skin_Specular(N_bumped, L2, V, m, rho_s, Rho_d_Tex );
	
	//float lit = (L0Shadow + L1Shadow + L2Shadow);
	//lit = clamp( lit, 0.0, 1.0);
	//FragColor = vec4(albedo.xy * lit, 1, 1);
 
    FragColor = vec4( diffuseLight + specularLight, 1.0 );  	
}