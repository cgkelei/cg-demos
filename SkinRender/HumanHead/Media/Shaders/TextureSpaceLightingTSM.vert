#version 330

uniform mat4 World;
uniform mat4 LightView;
uniform mat4 LightProj;


in vec4 iPos;
in vec2 iTex;
in vec3 iNormal;

out vec3 oWorldPos;
out vec3 oWorldNormal;
out vec4 oShadowCoord;
out float oLightDist;
out vec2 oTex;

void main()
{
	// unwrap the texture coordinate to fill the screen
	gl_Position = vec4(iTex * 2.0 - 1.0, 0.0, 1.0);
	
	vec4 worldPos = World * iPos;

	oTex = iTex;
	oWorldPos = worldPos.xyz;

	// transform normal to world space
    oWorldNormal = normalize(mat3(World) * iNormal);
	
	oShadowCoord = LightView * worldPos;
	oLightDist = -oShadowCoord.z;				// view space depth
	oShadowCoord = LightProj * oShadowCoord;
}