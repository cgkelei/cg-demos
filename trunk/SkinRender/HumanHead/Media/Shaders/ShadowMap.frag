#version 330

in float oDepth;
in vec2 oTex;

out vec4 FragColor;

void main()
{
	FragColor = vec4(oDepth * 0.5 + 0.5, oTex, 1.0);
}
