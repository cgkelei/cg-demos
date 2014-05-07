[[Fragment=GBufferPS]]

#include "/ModelMaterialFactory.glsl"

// PS Inputs
in vec4 oPosVS;
in vec2 oTex;

#ifdef _NormalMap
	in mat3 oTangentToView;
#else
	in vec3 oNormalVS;
#endif

// PS Outputs
layout(location = 0) out vec4 oFragColor0; // Normal + Shininess
layout(location = 1) out vec4 oFragColor1; // Diffuse + Specular
	
void main() 
{	
	Material material;
	GetMaterial(oTex, material);
	
	// normal map
#ifdef _NormalMap
	vec3 normal = texture2D( NormalMap, oTex ).rgb * 2.0 - 1.0;
	normal = normalize(normal * oTangentToView);
#else
	vec3 normal = normalize(oNormalVS);
#endif	
	
	normal = normal * 0.5 + 0.5;
	
	oFragColor0 = vec4(normal, material.Shininess / 255.0);
	oFragColor1 = vec4(material.DiffuseAlbedo,  dot(material.SpecularAlbedo, vec3(0.2126, 0.7152, 0.0722)));	 // Specular luminance
}