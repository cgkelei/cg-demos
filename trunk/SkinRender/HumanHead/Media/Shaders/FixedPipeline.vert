#version 330

uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;

in vec4 iPos;
in vec2 iTex;
in vec3 iNormal;

out vec3 oNormal;
out vec2 oTex;

void main()
{
	gl_Position = Projection * View * World * iPos;
	oNormal = mat3(World) * iNormal;
	oTex = iTex;
} 