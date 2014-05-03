[[Vertex=DeferedVS]]

// uniform
uniform mat4 WorldView;
uniform mat4 Projection;
uniform mat4 InvProj;

#if defined(DIRLIGHT)
	in vec2 iPos;
#elif defined(POINTLIGHT) || defined(SPOTLIGHT)  
	in vec3 iPos;
#endif

out vec3 oViewRay;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
#if defined(DIRLIGHT)

	gl_Position = vec4(iPos, 0.0, 1.0);

	vec4 posVS = gl_Position * InvProj;
	oViewRay = vec3(posVS.xy / posVS.z, 1.0);       // Proj to Z=1 plane
	
#elif defined(POINTLIGHT) || defined(SPOTLIGHT)   
	
	vec4 viewPos = vec4(iPos, 1.0) * WorldView;
	oViewRay = viewPos.xyz;
	
	gl_Position = viewPos * Projection;
#endif

}

[[Fragment=DirectionalLightingPS]]

#extension GL_EXT_texture_array : enable

#include "/LightingUtil.glsl"
#include "/ShadowUtil.glsl"
#include "/DeferredUtil.glsl"
	
uniform vec2 CameraNearFar;

uniform vec4 LightDirVS;	
uniform vec3 LightColor;

uniform mat4 InvView;			// Inv view matrix of current camera

uniform bool ShadowEnabled;
uniform bool VisiualizeCascades;

uniform sampler2D GBuffer0;	
uniform sampler2D DepthBuffer;	

in vec2 oTex;
in vec3 oViewRay;

out vec4 oFragColor;

const vec4 vCascadeColorsMultiplier[4] = vec4[4]
(
    vec4 ( 1.5, 0.0, 0.0, 1.0 ),
    vec4 ( 0.0, 1.5, 0.0, 1.0 ),
    vec4 ( 0.0, 0.0, 1.5, 1.0 ),
    vec4 ( 1.5, 1.5, 0.0, 1.0 )
);
	
void main()
{
	vec4 final = vec4(0.0);
	ivec2 sampleIndex = ivec2(gl_FragCoord.xy);

	// Convert non-linear depth to view space linear depth
	float linearDepth = LinearizeDepth( texelFetch(DepthBuffer, sampleIndex, 0).x, CameraNearFar.x, CameraNearFar.y );
	// View space lit position
	vec3 positionVS = PositionVSFromDepth(oViewRay, linearDepth);
	
	vec4 visibility = vec4(1.0);
	if (ShadowEnabled)
	{
		vec4 posWS = vec4(positionVS, 1.0) * InvView;
		
		int iCascadeSelected = 0;
		float percentLit = EvalCascadeShadow(posWS, iCascadeSelected);
		
		visibility = vec4(percentLit, percentLit, percentLit, percentLit);
		if (VisiualizeCascades)
			visibility *= vCascadeColorsMultiplier[iCascadeSelected];
	}
	
	// Fetch GBuffer
	vec4 tap0 = texture2D(GBuffer0, oTex);
				
	float shininess = tap0.a * 255.0;  			// specular power
	vec3 N = normalize(tap0.xyz * 2.0 - 1.0);   // view space normal
	vec3 L = normalize(-LightDirVS.xyz);		// light vector
		
	float nDotl = dot(L, N);
	if (nDotl > 0.0)
	{
		vec3 V = normalize(-positionVS);
		vec3 H = normalize(V + L);
		
		vec3 diffuse = LightColor * nDotl;
		
		// Frensel in moved to calculate in shading pass
		vec3 specular = CalculateSpecular(N, H, shininess) * LightColor * nDotl;
		
		final = vec4(diffuse, Luminance(specular));
	}
		
	oFragColor = final * visibility; 
}	

[[Fragment=PointLightingPS]]

#include "/LightingUtil.glsl"
#include "/DeferredUtil.glsl"

uniform vec2 CameraNearFar;

uniform vec4 LightPosVS;		// w dimension is spot light inner cone cos angle
uniform vec4 LightDirVS;		// w dimension is spot light outer cone cos angle
uniform vec3 LightColor;
uniform vec3 LightFalloff; 

uniform sampler2D GBuffer0;	
uniform sampler2D DepthBuffer;	
  
in vec3 oTex;
in vec3 oViewRay;

out vec4 oFragColor;

void main()
{
	vec4 final = vec4(0.0);

	vec2 tex = oTex.xy / oTex.z;
	tex = tex * 0.5 + 0.5;
	
	// Convert non-linear depth to view space linear depth
	float linearDepth = LinearizeDepth( texture2D(DepthBuffer, tex).x, CameraNearFar.x, CameraNearFar.y );
	// View space lit position
	vec3 positionVS = PositionVSFromDepth(oViewRay, linearDepth);
	
	vec3 L = normalize(LightPosVS.xyz - positionVS);
	
	// Fetch GBuffer
	vec4 tap0 = texture2D(GBuffer0, tex);

	// Decode view space normal
	vec3 N = normalize(tap0.xyz * 2.0 - 1.0); 
	float shininess = tap0.a * 255.0;
	
	float NdotL = dot(L, N);
	if (NdotL > 0.0)
	{
		vec3 V = normalize(-positionVS);
		vec3 H = normalize(V + L);
		
		// calculate attenuation
		float attenuation = CalcAttenuation(LightPosVS.xyz, positionVS, LightFalloff);
		
		vec3 diffuseLight = LightColor * NdotL * attenuation;	
		vec3 specularLight = CalculateSpecular(N, H, shininess) * diffuseLight; // Frensel in moved to calculate in shading pass
		
		final = vec4(diffuseLight, Luminance(specularLight));
	}
		
	oFragColor = final; 
}	

[[Fragment=SpotLightingPS]]

#include "/LightingUtil.glsl"
#include "/ShadowUtil.glsl"
#include "/DeferredUtil.glsl"

uniform vec4 LightPosVS;		// w dimension is spot light inner cone cos angle
uniform vec4 LightDirVS;		// w dimension is spot light outer cone cos angle
uniform vec3 LightColor;
uniform vec3 LightFalloff; 

uniform sampler2D GBuffer0;	
uniform sampler2D DepthBuffer;	

uniform vec2 CameraNearFar;
uniform mat4 InvView;			// Inv view matrix of current camera  
  
uniform bool ShadowEnabled; 
  
in vec3 oTex;
in vec3 oViewRay;

out vec4 oFragColor;

void main()
{
	vec4 final = vec4(0.0);

	vec2 tex = oTex.xy / oTex.z;
	tex = tex * 0.5 + 0.5;

	// Convert non-linear depth to view space linear depth
	float linearDepth = LinearizeDepth( texture2D(DepthBuffer, tex).x, CameraNearFar.x, CameraNearFar.y );
	// View space lit position
	vec3 positionVS = PositionVSFromDepth(oViewRay, linearDepth);
	
	float spot = SpotLighting(LightPosVS.xyz, LightDirVS.xyz, vec2(LightPosVS.w, LightDirVS.w), positionVS);
	if(spot > 0.0)
	{
		vec3 L = normalize(LightPosVS.xyz - positionVS);
		
		// Fetch GBuffer
		vec4 tap0 = texture2D(GBuffer0, tex);

		// Decode view space normal
		vec3 N = normalize(tap0.xyz * 2.0 - 1.0); 
		float shininess = tap0.a * 255.0;
	
		float nDotl = dot(L, N);
		if (nDotl > 0.0)
		{
			vec3 V = normalize(-positionVS);
			vec3 H = normalize(V + L);
		
			// calculate attenuation
			float attenuation = spot * CalcAttenuation(LightPosVS.xyz, positionVS, LightFalloff);	
			vec3 diffuse = LightColor * nDotl * attenuation;
		
			// Frensel in moved to calculate in shading pass
			vec3 specular = CalculateSpecular(N, H, shininess) * LightColor * nDotl * attenuation;
		
			final = vec4(diffuse, Luminance(specular));
		}
		
		float visibility = 1.0;
		if (ShadowEnabled)
			visibility = EvalCascadeShadow(vec4(positionVS, 1.0) * InvView);
	
		final *= visibility;
	}
		
	oFragColor = final; 
}	