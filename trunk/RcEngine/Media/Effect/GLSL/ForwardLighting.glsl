[[Fragment=DirectionalLightingPS]]
#pragma optimize(off)
#include "/ModelMaterialFactory.glsl"
#include "/LightingUtil.glsl"

uniform vec4 LightDirVS;
uniform vec3 LightColor;

// shader input
in vec4 oPosVS;
in vec2 oTex;

#ifdef _NormalMap
	in mat3 oTangentToView;
#else
    in vec3 oNormalVS;
#endif

layout(location = 0) out vec4 oFragColor;

void main()
{
	Material material;
	GetMaterial(oTex, material);
        
    // normal map
#ifdef _NormalMap
    vec3 N = texture2D( NormalMap, oTex ).rgb * 2.0 - 1.0;
    N = normalize(N * oTangentToView);
#else
    vec3 N = normalize(oNormalVS);
#endif          
        
    vec3 L = normalize(-LightDirVS.xyz);
    vec3 V = normalize(-oPosVS.xyz);
    vec3 H = normalize(V + L);
                          
    vec3 final = vec3(0);   
    float NdotL = dot(L, N);
    if (NdotL > 0.0)
    {	
		float normTerm = (material.Shininess + 2.0) / 8.0;
        float fresnel = CalculateFresnel(material.SpecularAlbedo, L, H);

		// Diffuse + Specular
        final = (material.DiffuseAlbedo + normTerm * CalculateSpecular(N, H, material.Shininess) * fresnel) * LightColor * NdotL;
	}

	// Ambient
	final += material.DiffuseAlbedo * 0.15;

    oFragColor = vec4(material.DiffuseAlbedo, final.r);
}