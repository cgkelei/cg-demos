#version 330

in float oDepth;
in vec2 oTex;

out vec4 FragColor;

void main()
{
	FragColor = vec4(oDepth, oTex, 1.0);
	//FragColor = vec4(oDepth, oDepth, oDepth, 1.0);
	//FragColor = vec4(oDepth * 0.5 + 0.5, oTex, 1.0);
}
