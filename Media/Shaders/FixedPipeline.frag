#version 330

uniform sampler2D AlbedoTex;

in vec3 oNormal;
in vec2 oTex;

out vec4 FragColor;

void main()
{
	FragColor = texture2D(AlbedoTex, oTex);
}