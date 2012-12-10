uniform mat4 WorldViewProjMat;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = WorldViewProjMat * gl_Vertex;
}