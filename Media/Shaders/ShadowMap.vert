#version 330

//uniform mat4 World;
//uniform mat4 View;
//uniform mat4 Proj;

//uniform float Grow =  0.000001;

//in vec4 iPos;
//in vec2 iTex;
//in vec3 iNormal;

//out float oDepth;
//out vec2 oTex;

//void main()
//{
//	mat4 WorldView = View * World;

//	vec4 modelPos = iPos;
//	modelPos.xyz += normalize(iNormal) * Grow;  // scale vertex along normal
//	gl_Position = Proj * WorldView * modelPos;

//	vec4 hPos = Proj * WorldView * iPos;
//	oDepth = hPos.z / hPos.w;	// non linear depth
//	oTex = iTex;	
//}


uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;


uniform float Grow =  0.000001;
uniform float ZNear;
uniform float ZFar;


in vec4 iPos;
in vec2 iTex;
in vec3 iNormal;

out float oDepth;
out vec2 oTex;

void main()
{
	mat4 WorldView = View * World;

	vec4 modelPos = iPos;
	modelPos.xyz += normalize(iNormal) * Grow;  // scale vertex along normal
	gl_Position = Proj * WorldView * modelPos;

	vec4 viewPos = WorldView * iPos;
	oDepth = (-viewPos.z-ZNear)/(ZFar-ZNear); // will map near..far to 0..1
	oTex = iTex;	
}
