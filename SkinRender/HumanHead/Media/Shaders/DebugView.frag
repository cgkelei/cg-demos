#version 120

uniform sampler2D ColorTex;
uniform int ViewDepth;

void main()
{
	vec3 color = vec3(1);

	if (ViewDepth > 0)
	{
		float depth = texture2D(ColorTex,  gl_TexCoord[0].xy).a;
	    color = vec3(depth, depth, depth);
	}
	else
	{
		color = texture2D(ColorTex,  gl_TexCoord[0].xy).rgb;
	}

	gl_FragColor = vec4(color, 1.0);
}