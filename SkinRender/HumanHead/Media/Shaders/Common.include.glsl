#define saturate(value) clamp(value, 0.0, 1.0)


float ViewDepthFromClip(vec4 clipPos, float zNear, float zFar)
{
	return ((-clipPos.z) * (zFar - zNear) - 2.0 * zNear * zFar) / (zNear + zFar);
}

float NormalizedDepth(float z, float zNear, float zFar)
{
	return (z - zNear) / (zFar - zNear);
}

float Shadow(vec4 shadowCoord, sampler2D shadowTex, float bais)
{
    vec3 shadowCoordWDiv = shadowCoord.xyz / shadowCoord.w;  
    return (texture(shadowTex, shadowCoordWDiv.xy).r < (shadowCoordWDiv.z- bais)) ? 0.0 : 1.0;
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
