#version 330

uniform mat4 World;
uniform mat4 View;
uniform mat4 Projection;

uniform float Grow =  0.000001;

in vec4 iPos;
in vec2 iTex;
in vec3 iNormal;

out float oDepth;
out vec2 oTex;

void main()
{
	vec4 modelPos = iPos;
	modelPos.xyz += normalize(iNormal) * Grow;  // scale vertex along normal
	gl_Position = Projection * View * World * iPos;

	vec4 hPos = Projection * View * World * iPos;
	oDepth = hPos.z / hPos.w;	// non linear depth
	oTex = iTex;	
}

//in vec4 gl_Vertex;
//in vec3 gl_Normal;
//in vec4 gl_MultiTexCoord0;

//void main()
//{
//	vec4 modelPos = gl_Vertex;
//	modelPos.xyz += normalize(gl_Normal) * Grow;  // scale vertex along normal
//	gl_Position = Projection * View * World * gl_Vertex;

//	vec4 hPos = Projection * View * World * gl_Vertex;
//	oDepth = hPos.z / hPos.w;	// non linear depth
//	oTex = gl_MultiTexCoord0.xy;	
//}