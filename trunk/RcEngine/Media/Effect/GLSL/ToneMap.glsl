
uniform sampler2D HDRBuffer;

// Varying
layout (location = 0) in vec2 iTex;

// Output
layout (location = 0) out vec4 oFragColor;

void main()
{
	ivec2 samplePos = ivec2(gl_FragCoord.xy);
	oFragColor = texelFetch(HDRBuffer, samplePos, 0);
}