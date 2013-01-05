#version 330

uniform mat4 MVP;

in vec4 gl_Vertex;
out vec3 oTex;

void main()
{
	gl_Position = MVP * gl_Vertex;
	oTex = gl_Vertex.xyz;
}
