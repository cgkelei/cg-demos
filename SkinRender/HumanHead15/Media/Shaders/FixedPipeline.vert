#version 330

uniform mat4 World;
uniform mat4 ViewProj;

in vec4 iPos;
in vec2 iTex;
in vec3 iNormal;

out vec3 oNormal;
out vec2 oTex;


void main()
{
	gl_Position = ViewProj* World * iPos;
	oNormal = mat3(World) * iNormal;
	oTex = iTex;
} 

//in vec4 gl_Vertex;
//in vec3 gl_Normal;
//in vec4 gl_MultiTexCoord0;

//void main()
//{
//	gl_Position = Projection * View * World * gl_Vertex;
//	oNormal = mat3(World) * gl_Normal;
//	oTex = gl_MultiTexCoord0.xy;
//} 