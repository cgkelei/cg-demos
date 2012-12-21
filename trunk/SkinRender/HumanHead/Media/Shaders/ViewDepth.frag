#version 120

uniform sampler2D DepthMap;

void main()
{
	float depth = texture2D(DepthMap,  gl_TexCoord[0].xy).r;
	gl_FragColor = vec4(depth, depth, depth, 1.0);
}