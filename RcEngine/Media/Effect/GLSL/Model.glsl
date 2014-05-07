[[Vertex=ModelVS]]

#include "/ModelVertexFactory.glsl"

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

out gl_PerVertex {
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