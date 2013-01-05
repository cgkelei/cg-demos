#version 330

uniform samplerCube EnvTex;

in vec3 oTex;

out vec4 FragColor;

void main()
{
	vec3 tap = texture(EnvTex, oTex).xyz;
	FragColor = vec4(tap, 1.0);
}