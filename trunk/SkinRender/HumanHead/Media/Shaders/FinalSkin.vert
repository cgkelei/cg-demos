#version 330

#define LIGHTCOUNT 3

uniform mat4 World;
uniform mat4 ViewProj;
uniform mat4 ShadowMatrix[LIGHTCOUNT];

in vec4 iPos;
in vec2 iTex;
in vec3 iNormal;

out vec3 oWorldPos;
out vec3 oWorldNormal;
out vec4 oShadowCoord[LIGHTCOUNT];
out vec2 oTex;

void main()
{
	gl_Position = ViewProj * World * iPos;
	
	vec4 worldPos = World * iPos;

	oTex = iTex;
	oWorldPos = worldPos.xyz;

	// transform normal to world space
    oWorldNormal = normalize(mat3(World) * iNormal);

	// Find the position of this pixel in light space
	for(int i = 0; i < LIGHTCOUNT; ++i)
	{
		oShadowCoord[i] = ShadowMatrix[i] * worldPos;
	}
}

//in vec4 gl_Vertex;
//in vec3 gl_Normal;
//in vec4 gl_MultiTexCoord0;

//void main()
//{
//	// unwrap the texture coordinate to fill the screen
//	gl_Position = ViewProj * World * gl_Vertex;
	
//	vec4 worldPos = World * gl_Vertex;

//	oTex = gl_MultiTexCoord0.xy;
//	oWorldPos = worldPos.xyz;

//	// transform normal to world space
//    oWorldNormal = normalize(mat3(World) * gl_Normal);

//	// Find the position of this pixel in light space
//	for(int i = 0; i < LIGHTCOUNT; ++i)
//	{
//		oShadowCoord[i] = ShadowMatrix[i] * worldPos;
//	}
//}