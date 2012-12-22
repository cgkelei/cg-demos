struct Light
{
	vec3 Position;	// Positon in world space
	vec3 Color;	
	float Amount;
	float Bias;
	vec2 NearFarPlane;
};	

#define SHADOW_MAP_SIZE 1024
#define saturate(value) clamp(value, 0.0, 1.0)

float NormalizedDepth(float depth, float zNear, float zFar)
{
	return (depth - zNear) / (zFar - zNear);
}

//float RealDepth(float depth, float zNear, float zFar)
//{
//	return depth * (zFar - zNear) + zNear;
//}

vec4 RealDepth(vec4 depth, vec2 nearFar)
{
	return depth * (nearFar.y - nearFar.x) + nearFar.x;
}


//float DepthFromClip(vec4 clipPos, float zNear, float zFar)
//{
//	return -((-clipPos.z) * (zFar - zNear) - 2.0 * zNear * zFar) / (zNear + zFar);
//}

//float NormalizedDepthFromClip(vec4 clipPos, vec2 nearFar)
//{
//	float zNear = nearFar.x;
//	float zFar = nearFar.y;

//	float depth =   -((-clipPos.z) * (zFar - zNear) - 2.0 * zNear * zFar) / (zNear + zFar);
//	return (depth - zNear) / (zFar - zNear);
//}

vec2 ShadowTexCoord(vec4 clipPos)
{
	return (clipPos.xy / clipPos.w) * 0.5 + vec2(0.5, 0.5);
}

float Shadow(vec4 shadowCoord, sampler2D shadowTex, float bais)
{
    vec3 shadowCoordWDiv = shadowCoord.xyz / shadowCoord.w;  
    return (texture(shadowTex, shadowCoordWDiv.xy).r < (shadowCoordWDiv.z- bais)) ? 0.0 : 1.0;
}

float Shadow(vec2 UV, sampler2D shadowTex, float depth)
{
    return (texture(shadowTex, UV).r < depth) ? 0.0 : 1.0;
}

float ShadowPCF(vec2 UV, sampler2D shadowTex, float depth, int samples, float width)
{
	float shadow = 0.0;
    float offset = (samples - 1.0) / 2.0;
   
    for (float x = -offset; x <= offset; x += 1.0) 
	{
        for (float y = -offset; y <= offset; y += 1.0)
		{
            vec2 tex = UV + width * vec2(x, y) / SHADOW_MAP_SIZE;
            shadow += (texture(shadowTex, tex).r < depth) ? 0.0 : 1.0;
        }
    }
    shadow /= samples * samples;
    return shadow;
}

float ShadowPCF(vec4 shadowCoord, sampler2D shadowTex, vec2 texSize, float bais, int samples, float width)
{
	vec3 shadowCoordWDiv = shadowCoord.xyz / shadowCoord.w;

    float shadow = 0.0;
    float offset = (samples - 1.0) / 2.0;
   
    for (float x = -offset; x <= offset; x += 1.0) 
	{
        for (float y = -offset; y <= offset; y += 1.0)
		{
            vec2 tex = shadowCoordWDiv.xy + width * vec2(x, y) / texSize;
            shadow += (texture(shadowTex, tex).r < (shadowCoordWDiv.z- bais)) ? 0.0 : 1.0;
        }
    }
    shadow /= samples * samples;
    return shadow;
}


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
