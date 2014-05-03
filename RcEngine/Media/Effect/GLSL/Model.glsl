[[Vertex=ShadowMapVS]]
#include "/ModelCommon.glsl"

uniform mat4 WorldView;	
uniform mat4 Projection;

#ifdef _AlphaTest
	out vec2 oTex;
#endif

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
#ifdef _Skinning
	mat4 Skin = CalculateSkinMatrix();
	gl_Position = vec4(iPos, 1.0) * Skin * WorldView * Projection;
#else
	gl_Position = vec4(iPos, 1.0) * WorldView * Projection;
#endif

#ifdef _AlphaTest
	oTex = iTex;
#endif
}

[[Fragment=ShadowMapVSM]]

out vec2 oFragDepth;

void main()
{
	oFragDepth.x = gl_FragCoord.z;
	oFragDepth.y = gl_FragCoord.z * gl_FragCoord.z;
}

[[Vertex=GeneralVS]]
#include "/ModelCommon.glsl"

// Shader uniforms	
uniform mat4 WorldView;	
uniform mat4 Projection;

// VS Outputs
out vec4 oPosVS;
out vec2 oTex;

#ifdef _NormalMap
	out mat3 oTangentToView;
#else
	out vec3 oNormalVS;
#endif

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
#ifdef _Skinning

	mat4 Skin = CalculateSkinMatrix();
	mat4 SkinWorldView = Skin * WorldView;
	
	oPosVS = vec4(iPos, 1.0)* SkinWorldView;
	vec3 normal = normalize(iNormal * mat3(SkinWorldView));

#else

	oPosVS = vec4(iPos, 1.0) * WorldView;
	vec3 normal = normalize(iNormal * mat3(WorldView));

#endif

	// calculate tangent and binormal.
#ifdef _NormalMap
	#ifdef _Skinning
		vec3 tangent = normalize(iTangent * mat3(SkinWorldView));
		vec3 binormal = normalize(iBinormal * mat3(SkinWorldView));
	#else
		vec3 tangent = normalize(iTangent * mat3(WorldView));
		vec3 binormal = normalize(iBinormal * mat3(WorldView));
	#endif

	// actualy this is a world to tangent matrix, because we always use V * Mat.
	oTangentToView = mat3( tangent, binormal, normal);

	// transpose to get tangent to world matrix
	oTangentToView = transpose(oTangentToView);
#else
	oNormalVS = normal;
#endif
	
	// texcoord
	oTex = iTex;
	gl_Position = oPosVS * Projection;
}

[[Fragment=GBufferPS]]

// PS Inputs
in vec4 oPosVS;
in vec2 oTex;

#ifdef _NormalMap
	in mat3 oTangentToView;
#else
	in vec3 oNormalVS;
#endif

// PS Outputs
out vec4 FragColor0; // Normal + shininess
out vec4 FragColor1; // Diffuse + Specular
	
void main() 
{	
	// diffuse material
#ifdef _DiffuseMap
	vec4 diffuseTap = texture2D(DiffuseMap, oTex);
	vec3 albedo = diffuseTap.rgb;
	#ifdef _AlphaTest
		if( diffuseTap.a < 0.01 ) discard;
	#endif	
#else
	vec3 albedo = DiffuseColor;
#endif

	// specular material
#ifdef _SpecularMap
	vec4 specularTap = texture2D(SpecularMap, oTex);
	//vec3 specular = specularTap.rgb;
	vec3 specular = specularTap.rrr;
	#ifdef _GlossyMap
		float shininess = specularTap.a;
	#else 
		float shininess = Shininess / 255.0;
	#endif
#else
	vec3 specular = SpecularColor;
	float shininess = Shininess / 255.0;
#endif
	
	// normal map
#ifdef _NormalMap
	vec3 normal = texture2D( NormalMap, oTex ).rgb * 2.0 - 1.0;
	normal = normalize(normal * oTangentToView);
#else
	vec3 normal = normalize(oNormalVS);
#endif	
	
	normal = normal * 0.5 + 0.5;
	
	FragColor0 = vec4(normal.xyz, shininess);
	FragColor1 = vec4(albedo.rgb,  dot(specular, vec3(0.2126, 0.7152, 0.0722)));	 // Specular luminance
}
