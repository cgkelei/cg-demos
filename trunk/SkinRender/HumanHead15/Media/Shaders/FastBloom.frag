#version 120

uniform sampler2D InputTex;
uniform sampler2D BloomTex[2];

uniform float Exposure = 1.5;

void main()
{
	vec4 base = texture2D(InputTex, gl_TexCoord[0].xy);
	vec4 bloom1 = texture2D(BloomTex[0], gl_TexCoord[0].xy);
	vec4 bloom2 = texture2D(BloomTex[1], gl_TexCoord[0].xy);
	
	vec4 color = base + 0.0174 * bloom1 + 0.192 * bloom2;


	// simple tonemap
	color = 1.0 - exp2(-Exposure * color);

	gl_FragColor = vec4(color.xyz, 1.0);
}