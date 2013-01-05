#version 330

#define LIGHTCOUNT 3

uniform mat4 World;
uniform mat4 ViewProj;
uniform mat4 LightView[LIGHTCOUNT];
uniform mat4 LightProj[LIGHTCOUNT];

in vec4 iPos;
in vec2 iTex;
in vec3 iNormal;

out vec3 oWorldPos;
out vec3 oWorldNormal;
out vec2 oTex;
out vec4 oShadowCoord[LIGHTCOUNT];
out float oLightDist[LIGHTCOUNT];

void main()
{
	vec4 worldPos = World * iPos;
	
	oWorldNormal = mat3(World) * iNormal;
	oTex = iTex;
	oWorldPos = worldPos.xyz;

	// Find the position of this pixel in light space
	for(int i = 0; i < LIGHTCOUNT; ++i)
	{
		oShadowCoord[i] = LightView[i] * worldPos;
		oLightDist[i] = -oShadowCoord[i].z;				// view space depth
		oShadowCoord[i] = LightProj[i] * oShadowCoord[i];
	}

	gl_Position = ViewProj* worldPos;

} 