[[Vertex=PassThroughVS]]

layout (location = 0) in vec3 iControlPoint;

out gl_PerVertex { 
	vec4 gl_Position; // Bezier Patch Control Point
};

void main()
{
	gl_Position = vec4(iControlPoint, 1.0);
}

[[TessControl=TCSTeapot]]

layout (vertices = 16) out;

uniform int TessLevel;

in gl_PerVertex { 
	vec4 gl_Position; 
} gl_in[];

out gl_PerVertex { 
	vec4 gl_Position;
} gl_out[];

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	if (gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = float(TessLevel);
		gl_TessLevelOuter[1] = float(TessLevel);
	    gl_TessLevelOuter[2] = float(TessLevel);
		gl_TessLevelOuter[3] = float(TessLevel);

		gl_TessLevelInner[0] = float(TessLevel);
		gl_TessLevelInner[1] = float(TessLevel);
	}
}

[[TessEval=TESTeapot]]

layout( quads, equal_spacing, cw ) in;

uniform mat4 World;
uniform mat3 ViewProj;

in gl_PerVertex { 
	vec4 gl_Position; 
} gl_in[];

// Output
layout (location = 0) out vec3 oPosWS;
layout (location = 1) out vec2 oTex;
layout (location = 2) out vec3 oNormalWS;

out gl_PerVertex { 
	vec4 gl_Position;
};

// Compute bernstein and partial derivatives of bernstein
void Bernstein(out float[4] b, out float[4] db, float t)
{
	float t1 = (1.0 - t);
	float t12 = t1 * t1;
	
	// Bernstein polynomials
	b[0] = t12 * t1;
	b[1] = 3.0 * t12 * t;
	b[2] = 3.0 * t1 * t * t;
	b[3] = t * t * t;

	// Derivatives
	db[0] = -3.0 * t1 * t1;
	db[1] = -6.0 * t * t1 + 3.0 * t12;
	db[2] = -3.0 * t * t + 6.0 * t * t1;
	db[3] = 3.0 * t * t;
}

#define CP(i, j) gl_in[i*4+j].gl_Position

void main()
{
	float bu[4], bv[4], dbu[4], dbv[4];
	Bernstein(bu, dbu, gl_TessCoord.x);
	Bernstein(bv, dbv, gl_TessCoord.y);

	vec4 p = vec4(0);
	vec4 du = vec4(0);
	vec4 dv = vec4(0);

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			p += bu[i]*bv[j]*gl_in[i*4+j].gl_Position;
			du += bu[i]*bv[j]*gl_in[i*4+j].gl_Position;
			dv += bu[i]*bv[j]*gl_in[i*4+j].gl_Position;
		}
    vec4 du = CP(0,0)*dbu[0]*bv[0] + CP(01*dbu[0]*bv[1] + CP(02*dbu[0]*bv[2] + CP(03*dbu[0]*bv[3] +
			  CP(1,0)*dbu[1]*bv[0] + CP(11*dbu[1]*bv[1] + CP(12*dbu[1]*bv[2] + CP(13*dbu[1]*bv[3] +
			  CP(2,0)*dbu[2]*bv[0] + CP(21*dbu[2]*bv[1] + CP(22*dbu[2]*bv[2] + CP(23*dbu[2]*bv[3] +
			  CP(3,0)*dbu[3]*bv[0] + CP(31*dbu[3]*bv[1] + CP(32*dbu[3]*bv[2] + CP(33*dbu[3]*bv[3];
	
	vec4 dv = CP(0,0)*bu[0]*dbv[0] + CP(01*bu[0]*dbv[1] + CP(02*bu[0]*dbv[2] + CP(03*bu[0]*dbv[3] +
			  CP(1,0)*bu[1]*dbv[0] + CP(11*bu[1]*dbv[1] + CP(12*bu[1]*dbv[2] + CP(13*bu[1]*dbv[3] +
			  CP(2,0)*bu[2]*dbv[0] + CP(21*bu[2]*dbv[1] + CP(22*bu[2]*dbv[2] + CP(23*bu[2]*dbv[3] +
			  CP(3,0)*bu[3]*dbv[0] + CP(31*bu[3]*dbv[1] + CP(32*bu[3]*dbv[2] + CP(33*bu[3]*dbv[3];
}
