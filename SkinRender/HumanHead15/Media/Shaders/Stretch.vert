#version 330

uniform mat4 WorldMat;

in vec4 iPos;
in vec2 iTex;
in vec3 iNormal;

out vec3 oWorldPos;


void main()
{
	// unwrap the texture coordinate to fill the screen
	gl_Position = vec4(iTex * 2.0 - 1.0, 0.0, 1.0);

	// transform to world space
	oWorldPos = vec3(WorldMat * iPos);
}