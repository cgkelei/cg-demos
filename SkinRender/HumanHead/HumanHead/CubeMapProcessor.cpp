#include "CubeMapProcessor.h"
#include <stdint.h>
#include <fstream>

namespace {

//Note this currently assumes the D3D cube face ordering and orientation
#define CP_UDIR     0
#define CP_VDIR     1
#define CP_FACEAXIS 2

glm::vec3 Face2DMapping[6][3] = {
	//XPOS face
    {glm::vec3(0,  0, -1),   //u towards negative Z
	glm::vec3(0, -1,  0),   //v towards negative Y
	glm::vec3(1,  0,  0)},  //pos X axis  
	//XNEG face
	{glm::vec3(0,  0,  1),   //u towards positive Z
	glm::vec3(0, -1,  0),   //v towards negative Y
	glm::vec3(-1,  0,  0)},  //neg X axis       
	//YPOS face
	{glm::vec3(1, 0, 0),     //u towards positive X
	glm::vec3(0, 0, 1),     //v towards positive Z
	glm::vec3(0, 1 , 0)},   //pos Y axis  
	//YNEG face
	{glm::vec3(1, 0, 0),     //u towards positive X
	glm::vec3(0, 0 , -1),   //v towards negative Z
	glm::vec3(0, -1 , 0)},  //neg Y axis  
	//ZPOS face
	{glm::vec3(1, 0, 0),     //u towards positive X
	glm::vec3(0, -1, 0),    //v towards negative Y
	glm::vec3(0, 0,  1)},   //pos Z axis  
	//ZNEG face
	{glm::vec3(-1, 0, 0),    //u towards negative X
	glm::vec3(0, -1, 0),    //v towards negative Y
	glm::vec3(0, 0, -1)},   //neg Z axis  
};


/**
 * Convert cubemap face texel coordinates and face idx to 3D vector
 * note the U and V coords are integer coords and range from 0 to size-1
 * this routine can be used to generate a normalizer cube map
 */
glm::vec3 TexelCoordToVect(int faceID, float U, float V, int edgeLength, int fixupType)
{
	float nvcU, nvcV;
	glm::vec3 retVal;

	if (fixupType == CubeMapProcessor::EF_Stretch && edgeLength > 1)
	{
		// Code from Nvtt : http://code.google.com/p/nvidia-texture-tools/source/browse/trunk/src/nvtt/CubeSurface.cpp		
		// transform from [0..res - 1] to [-1 .. 1], match up edges exactly.
		nvcU = (2.0f * (float)U / ((float)edgeLength - 1.0f) ) - 1.0f;
		nvcV = (2.0f * (float)V / ((float)edgeLength - 1.0f) ) - 1.0f;
	}
	else
	{
		// Change from original AMD code
		// transform from [0..res - 1] to [- (1 - 1 / res) .. (1 - 1 / res)]
		// + 0.5f is for texel center addressing
		nvcU = (2.0f * ((float)U + 0.5f) / (float)edgeLength ) - 1.0f;
		nvcV = (2.0f * ((float)V + 0.5f) / (float)edgeLength ) - 1.0f;
	}

	if (fixupType == CubeMapProcessor::EF_Wrap && edgeLength > 1)
	{
		// Warp texel centers in the proximity of the edges. 
		float a = powf(float(edgeLength), 2.0f) / powf(float(edgeLength - 1), 3.0f); 
		nvcU = a * powf(nvcU, 3) + nvcU; 
		nvcV = a * powf(nvcV, 3) + nvcV; 

		// Get current vector
		//generate x,y,z vector (xform 2d NVC coord to 3D vector)

		retVal = glm::vec3(Face2DMapping[faceID][CP_UDIR]) * nvcU;
		retVal += glm::vec3(Face2DMapping[faceID][CP_VDIR]) * nvcV;
		retVal += glm::vec3(Face2DMapping[faceID][CP_FACEAXIS]);
		retVal = glm::normalize(retVal);
	}
	//else if (fixupType == CubeMapProcessor::EF_Bent && edgeLength > 1)
	//{
	//	// Method following description of Physically based rendering slides from CEDEC2011 of TriAce

	//	// Get vector at edge
	//	float EdgeNormalU[3];
	//	float EdgeNormalV[3];
	//	float EdgeNormal[3];
	//	float EdgeNormalMinusOne[3];

	//	// Recover vector at edge
	//	//U contribution
	//	VM_SCALE3(EdgeNormalU, sgFace2DMapping[a_FaceIdx][CP_UDIR], nvcU < 0.0 ? -1.0f : 1.0f);    
	//	//V contribution
	//	VM_SCALE3(EdgeNormalV, sgFace2DMapping[a_FaceIdx][CP_VDIR], nvcV < 0.0 ? -1.0f : 1.0f);
	//	VM_ADD3(EdgeNormal, EdgeNormalV, EdgeNormalU);
	//	//add face axis
	//	VM_ADD3(EdgeNormal, sgFace2DMapping[a_FaceIdx][CP_FACEAXIS], EdgeNormal);
	//	//normalize vector
	//	VM_NORM3(EdgeNormal, EdgeNormal);

	//	// Get vector at (edge - 1)
	//	float nvcUEdgeMinus1 = (2.0f * ((float)(nvcU < 0.0f ? 0 : a_Size-1) + 0.5f) / (float)a_Size ) - 1.0f;
	//	float nvcVEdgeMinus1 = (2.0f * ((float)(nvcV < 0.0f ? 0 : a_Size-1) + 0.5f) / (float)a_Size ) - 1.0f;

	//	// Recover vector at (edge - 1)
	//	//U contribution
	//	VM_SCALE3(EdgeNormalU, sgFace2DMapping[a_FaceIdx][CP_UDIR], nvcUEdgeMinus1);    
	//	//V contribution
	//	VM_SCALE3(EdgeNormalV, sgFace2DMapping[a_FaceIdx][CP_VDIR], nvcVEdgeMinus1);
	//	VM_ADD3(EdgeNormalMinusOne, EdgeNormalV, EdgeNormalU);
	//	//add face axis
	//	VM_ADD3(EdgeNormalMinusOne, sgFace2DMapping[a_FaceIdx][CP_FACEAXIS], EdgeNormalMinusOne);
	//	//normalize vector
	//	VM_NORM3(EdgeNormalMinusOne, EdgeNormalMinusOne);

	//	// Get angle between the two vector (which is 50% of the two vector presented in the TriAce slide)
	//	float AngleNormalEdge = acosf(VM_DOTPROD3(EdgeNormal, EdgeNormalMinusOne));

	//	// Here we assume that high resolution required less offset than small resolution (TriAce based this on blur radius and custom value) 
	//	// Start to increase from 50% to 100% target angle from 128x128x6 to 1x1x6
	//	float NumLevel = (logf(min(a_Size, 128))  / logf(2)) - 1;
	//	AngleNormalEdge = LERP(0.5 * AngleNormalEdge, AngleNormalEdge, 1.0f - (NumLevel/6) );

	//	float factorU = abs((2.0f * ((float)a_U) / (float)(a_Size - 1) ) - 1.0f);
	//	float factorV = abs((2.0f * ((float)a_V) / (float)(a_Size - 1) ) - 1.0f);
	//	AngleNormalEdge = LERP(0.0f, AngleNormalEdge, max(factorU, factorV) );

	//	// Get current vector
	//	//generate x,y,z vector (xform 2d NVC coord to 3D vector)
	//	//U contribution
	//	VM_SCALE3(a_XYZ, sgFace2DMapping[a_FaceIdx][CP_UDIR], nvcU);    
	//	//V contribution
	//	VM_SCALE3(tempVec, sgFace2DMapping[a_FaceIdx][CP_VDIR], nvcV);
	//	VM_ADD3(a_XYZ, tempVec, a_XYZ);
	//	//add face axis
	//	VM_ADD3(a_XYZ, sgFace2DMapping[a_FaceIdx][CP_FACEAXIS], a_XYZ);
	//	//normalize vector
	//	VM_NORM3(a_XYZ, a_XYZ);

	//	float RadiantAngle = AngleNormalEdge;
	//	// Get angle between face normal and current normal. Used to push the normal away from face normal.
	//	float AngleFaceVector = acosf(VM_DOTPROD3(sgFace2DMapping[a_FaceIdx][CP_FACEAXIS], a_XYZ));

	//	// Push the normal away from face normal by an angle of RadiantAngle
	//	slerp(a_XYZ, sgFace2DMapping[a_FaceIdx][CP_FACEAXIS], a_XYZ, 1.0f + RadiantAngle / AngleFaceVector);
	//}
	else
	{
		retVal = glm::vec3(Face2DMapping[faceID][CP_UDIR]) * nvcU;
		retVal += glm::vec3(Face2DMapping[faceID][CP_VDIR]) * nvcV;
		retVal += glm::vec3(Face2DMapping[faceID][CP_FACEAXIS]);
		retVal = glm::normalize(retVal);
	}

	return retVal;
}

void VectToTexelCoord(glm::vec3 XYZ, int edgeLength, int *oFaceIdx, int *oU, int *oV )
{
	float nvcU, nvcV;
	float maxCoord;
	int   faceIdx;
	int   u, v;

	//absolute value 3
	glm::vec3 abxXYZ = glm::abs(XYZ);

	if( (abxXYZ[0] >= abxXYZ[1]) && (abxXYZ[0] >= abxXYZ[2]) )
	{
		maxCoord = abxXYZ[0];

		if(XYZ[0] >= 0) 
		{
			faceIdx = CubeMapProcessor::CMF_Positive_X;            
		}    
		else
		{
			faceIdx = CubeMapProcessor::CMF_Negtive_X;                    
		}
	}
	else if ( (abxXYZ[1] >= abxXYZ[0]) && (abxXYZ[1] >= abxXYZ[2]) )
	{
		maxCoord = abxXYZ[1];

		if(XYZ[1] >= 0) 
		{
			faceIdx = CubeMapProcessor::CMF_Positive_Y;            
		}    
		else
		{
			faceIdx = CubeMapProcessor::CMF_Negtive_Y;                    
		}    
	}
	else  // if( (absXYZ[2] > absXYZ[0]) && (absXYZ[2] > absXYZ[1]) )
	{
		maxCoord = abxXYZ[2];

		if(XYZ[2] >= 0) //face = XPOS
		{
			faceIdx = CubeMapProcessor::CMF_Positive_Z;            
		}    
		else
		{
			faceIdx = CubeMapProcessor::CMF_Negtive_Z;                    
		}    
	}

	//divide through by max coord so face vector lies on cube face
	glm::vec3 onFaceXYZ = XYZ * (1.0f / maxCoord);

	nvcU = glm::dot(Face2DMapping[ faceIdx ][CP_UDIR], onFaceXYZ );
	nvcV = glm::dot(Face2DMapping[ faceIdx ][CP_VDIR], onFaceXYZ );

	// SL BEGIN
	// Modify original AMD code to return value from 0 to Size - 1
	u = (int)floor( (edgeLength - 1) * 0.5f * (nvcU + 1.0f) );
	v = (int)floor( (edgeLength - 1) * 0.5f * (nvcV + 1.0f) );
	// SL END

	if(oFaceIdx) *oFaceIdx = faceIdx;
	if(oU) *oU = u;
	if(oV) *oV = v;
}

float AreaElement( float x, float y )
{
	return atan2(x * y, sqrt(x * x + y * y + 1));
}

float SolidAngleTerm(int x, int y, int edgeLength)
{
	// Transform x,y to [-1, 1] range, offset by 0.5 to point to texel center.
	float U = (2.0f * (float(x) + 0.5f) / (float)edgeLength ) - 1.0f;
	float V = (2.0f * (float(y) + 0.5f) / (float)edgeLength ) - 1.0f;

	// Shift from a demi texel, mean 1.0f / a_Size with U and V in [-1..1]
	float InvResolution = 1.0f / edgeLength;

	// U and V are the -1..1 texture coordinate on the current face.
	// Get projected area for this texel
	float x0 = U - InvResolution;
	float y0 = V - InvResolution;
	float x1 = U + InvResolution;
	float y1 = V + InvResolution;
	float SolidAngle = AreaElement(x0, y0) - AreaElement(x0, y1) - AreaElement(x1, y0) + AreaElement(x1, y1);

	return SolidAngle;
}

// SH order use for approximation of irradiance cubemap is 4, mean 5*5 equals 25 coefficients
#define MAX_SH_ORDER 4
#define NUM_SH_COEFFICIENT (MAX_SH_ORDER + 1) * (MAX_SH_ORDER + 1)
#define CP_PI double(3.14159265358979323846264338327950288)

// See Peter-Pike Sloan paper for these coefficients
static double SHBandFactor[NUM_SH_COEFFICIENT] = { 1.0,
	2.0 / 3.0, 2.0 / 3.0, 2.0 / 3.0,
	1.0 / 4.0, 1.0 / 4.0, 1.0 / 4.0, 1.0 / 4.0, 1.0 / 4.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, // The 4 band will be zeroed
	- 1.0 / 24.0, - 1.0 / 24.0, - 1.0 / 24.0, - 1.0 / 24.0, - 1.0 / 24.0, - 1.0 / 24.0, - 1.0 / 24.0, - 1.0 / 24.0, - 1.0 / 24.0};


void EvalSHBasis(int order, const float* dir, double* res )
{
	// Can be optimize by precomputing constant.
	static const double SqrtPi = sqrt(CP_PI);

	double xx = dir[0];
	double yy = dir[1];
	double zz = dir[2];

	// x[i] == pow(x, i), etc.
	double x[MAX_SH_ORDER+1], y[MAX_SH_ORDER+1], z[MAX_SH_ORDER+1];
	x[0] = y[0] = z[0] = 1.;
	for (int i = 1; i < MAX_SH_ORDER+1; ++i)
	{
		x[i] = xx * x[i-1];
		y[i] = yy * y[i-1];
		z[i] = zz * z[i-1];
	}

	switch(order)
	{
	case 4:
		{
			res[16] = (3*sqrt(35/CP_PI)*xx*yy*(x[2] - y[2]))/4.;
			res[17] = (-3*sqrt(35/(2.*CP_PI))*(3*x[2]*yy - y[3])*zz)/4.;
			res[18] = (3*sqrt(5/CP_PI)*xx*yy*(-1 + 7*z[2]))/4.;
			res[19] = (-3*sqrt(5/(2.*CP_PI))*yy*zz*(-3 + 7*z[2]))/4.;
			res[20] = (3*(3 - 30*z[2] + 35*z[4]))/(16.*SqrtPi);
			res[21] = (-3*sqrt(5/(2.*CP_PI))*xx*zz*(-3 + 7*z[2]))/4.;
			res[22] = (3*sqrt(5/CP_PI)*(x[2] - y[2])*(-1 + 7*z[2]))/8.;
			res[23] = (-3*sqrt(35/(2.*CP_PI))*(x[3] - 3*xx*y[2])*zz)/4.;
			res[24] = (3*sqrt(35/CP_PI)*(x[4] - 6*x[2]*y[2] + y[4]))/16.;
		}
	case 3:
		{
			res[9]  = (sqrt(35/(2.*CP_PI))*(-3*x[2]*yy + y[3]))/4.;
			res[10] = (sqrt(105/CP_PI)*xx*yy*zz)/2.;
			res[11] = -(sqrt(21/(2.*CP_PI))*yy*(-1 + 5*z[2]))/4.;
			res[12] = (sqrt(7/CP_PI)*zz*(-3 + 5*z[2]))/4.;
			res[13] = -(sqrt(21/(2.*CP_PI))*xx*(-1 + 5*z[2]))/4.;
			res[14] = (sqrt(105/CP_PI)*(x[2] - y[2])*zz)/4.;
			res[15] = -(sqrt(35/(2.*CP_PI))*(x[3] - 3*xx*y[2]))/4.;
		}
	case 2:
		{
			res[4]  = (sqrt(15/CP_PI)*xx*yy)/2.;
			res[5]  = -(sqrt(15/CP_PI)*yy*zz)/2.;
			res[6]  = (sqrt(5/CP_PI)*(-1 + 3*z[2]))/4.;
			res[7]  = -(sqrt(15/CP_PI)*xx*zz)/2.;
			res[8]  = sqrt(15/CP_PI)*(x[2] - y[2])/4.;
		}
	case 1:
		{
			res[1]  = -(sqrt(3/CP_PI)*yy)/2.;
			res[2]  = (sqrt(3/CP_PI)*zz)/2.;
			res[3]  = -(sqrt(3/CP_PI)*xx)/2.;
		}
	case 0:
		res[0]  = (1/(2.*SqrtPi));
		break;
	default:
		throw std::exception("SH Order overflow");
	}
}


}

CubeMapProcessor::CubeMapProcessor(void)
{
}


CubeMapProcessor::~CubeMapProcessor(void)
{
}


void CubeMapProcessor::SHProjectCubeMap( int order, bool useSolidAngleWeighting, int fixupType, double* pROut, double* pGOut, double* pBOut )
{
	double weightAccum = 0.0;
	double weight = 0.0;

	const int baisNum = (order+1) * (order+1);
	std::vector<double> SHBais(baisNum, 0.0);

	//pointers used to walk across the image surface
	float *normCubeRowStartPtr;
	float *srcCubeRowStartPtr;
	float *texelVect;

	for(int iCubeFace=0; iCubeFace < 6; iCubeFace++)
	{
		mNormCubeMap[iCubeFace].Clear();            
	}

	//Normalized vectors per cubeface and per-texel solid angle 
	BuildNormalizerSolidAngleCubemap(mSrcCubeMap[0].GetWidth(), mNormCubeMap, fixupType);

	const int NormCubeMapNumChannels = mNormCubeMap[0].GetNumChannels(); 
	const int SrcCubeMapNumChannels = mSrcCubeMap[0].GetNumChannels(); 
	const int SrcSize = mSrcCubeMap[0].GetWidth();

	for (int faceIdx = 0; faceIdx < 6; faceIdx++)
	{
		float* normImgData = mNormCubeMap[faceIdx].GetImageData();
		float* texImgData = mSrcCubeMap[faceIdx].GetImageData();

		for (int y = 0; y < SrcSize; y++)
		{
			normCubeRowStartPtr = &normImgData[NormCubeMapNumChannels * (y * SrcSize)];
			srcCubeRowStartPtr	= &texImgData[SrcCubeMapNumChannels * (y * SrcSize)];

			for (int x = 0; x < SrcSize; x++)
			{
				//pointer to direction and solid angle in cube map associated with texel
				texelVect = &normCubeRowStartPtr[NormCubeMapNumChannels * x];

				if(useSolidAngleWeighting)
				{
					//solid angle stored in 4th channel of normalizer/solid angle cube map
					weight = *(texelVect+3);
				}
				else
				{   //all taps equally weighted
					weight = 1.0;   
				}

				EvalSHBasis(order, texelVect, &SHBais[0]);

				// Convert to float64
				double R = srcCubeRowStartPtr[(SrcCubeMapNumChannels * x) + 0];
				double G = srcCubeRowStartPtr[(SrcCubeMapNumChannels * x) + 1];
				double B = srcCubeRowStartPtr[(SrcCubeMapNumChannels * x) + 2];

				for (int i = 0; i < baisNum; i++)
				{
					pROut[i] += R * SHBais[i] * weight;
					pGOut[i] += G * SHBais[i] * weight;
					pBOut[i] += B * SHBais[i] * weight;
				}

				weightAccum += weight;
			}
		}
	}

	//Normalization - The sum of solid angle should be equal to the solid angle of the sphere (4 PI), so
	// normalize in order our weightAccum exactly match 4 PI.
	for (int i = 0; i < baisNum; ++i)
	{
		pROut[i] *= 4.0 * CP_PI / weightAccum;
		pGOut[i] *= 4.0 * CP_PI / weightAccum;
		pBOut[i] *= 4.0 * CP_PI / weightAccum;
	}
}

void CubeMapProcessor::SHIrrandianceFilterCubeMap(int order,  bool useSolidAngleWeighting, int fixupType )
{
	double weightAccum = 0.0;
	double weight = 0.0;

	//pointers used to walk across the image surface
	float *normCubeRowStartPtr;
	float *dstCubeRowStartPtr;
	float *texelVect;
	
	const int baisNum = (order+1) * (order+1);
	std::vector<double> SHBais(baisNum, 0.0);

	//First step - Generate SH coefficient for the diffuse convolution
	std::vector<double> SHr(baisNum, 0.0);
	std::vector<double> SHg(baisNum, 0.0);
	std::vector<double> SHb(baisNum, 0.0);

	SHProjectCubeMap(order, useSolidAngleWeighting, fixupType, &SHr[0], &SHg[0], &SHb[0]);

	//Second step - Generate cubemap from SH coefficient

	for(int iCubeFace=0; iCubeFace < 6; iCubeFace++)
	{
		mNormCubeMap[iCubeFace].Clear();            
	}

	//Normalized vectors per cubeface and per-texel solid angle 
	BuildNormalizerSolidAngleCubemap(mSrcCubeMap[0].GetWidth(), mNormCubeMap, fixupType);


	const int DstSize = mDstCubeMap[0].GetWidth();

	const int NormCubeMapNumChannels = mNormCubeMap[0].GetNumChannels();
	const int DstCubeMapNumChannels = mDstCubeMap[0].GetNumChannels();

	for (int faceIdx = 0; faceIdx < 6; faceIdx++)
	{
		float* normImgData = mNormCubeMap[faceIdx].GetImageData();
		float* destImgData = mDstCubeMap[faceIdx].GetImageData();

		for (int y = 0; y < DstSize; y++)
		{
			normCubeRowStartPtr = &normImgData[NormCubeMapNumChannels * (y * DstSize)];
			dstCubeRowStartPtr	= &destImgData[DstCubeMapNumChannels * (y * DstSize)];

			for (int x = 0; x < DstSize; x++)
			{
				//pointer to direction and solid angle in cube map associated with texel
				texelVect = &normCubeRowStartPtr[NormCubeMapNumChannels * x];

				EvalSHBasis(order, texelVect, &SHBais[0]);

				// get color value
				float R = 0.0f, G = 0.0f, B = 0.0f;

				for (int i = 0; i < baisNum; ++i)
				{
					R += (float)(SHr[i] * SHBais[i] * SHBandFactor[i]);
					G += (float)(SHg[i] * SHBais[i] * SHBandFactor[i]);
					B += (float)(SHb[i] * SHBais[i] * SHBandFactor[i]);
				}

				dstCubeRowStartPtr[(DstCubeMapNumChannels * x) + 0] = R;
				dstCubeRowStartPtr[(DstCubeMapNumChannels * x) + 1] = G;
				dstCubeRowStartPtr[(DstCubeMapNumChannels * x) + 2] = B;  

				if (DstCubeMapNumChannels > 3)
				{
					dstCubeRowStartPtr[(DstCubeMapNumChannels * x) + 3] = 1.0f;
				}
			}
		}
	}
}

void CubeMapProcessor::BuildNormalizerSolidAngleCubemap( int size, SurfaceImage* surface, int fixupType )
{
	int iCubeFace, u, v;

	//iterate over cube faces
	for(iCubeFace=0; iCubeFace < 6; iCubeFace++)
	{
		surface[iCubeFace].Clear();
		surface[iCubeFace].Init(size, size, 4);  //First three channels for norm cube, and last channel for solid angle

		//fast texture walk, build normalizer cube map
		float* texelPtr = surface[iCubeFace].GetImageData();

		const int imgWidth = surface[iCubeFace].GetWidth();
		const int imgHeight = surface[iCubeFace].GetHeight();

		for(v=0; v < imgHeight; v++)
		{
			for(u=0; u < imgWidth; u++)
			{
				glm::vec3 dir = TexelCoordToVect(iCubeFace, (float)u, (float)v, size, fixupType);
				
				*(texelPtr + 0) = dir[0];
				*(texelPtr + 1) = dir[1];
				*(texelPtr + 2) = dir[2];
				*(texelPtr + 3) = SolidAngleTerm(u, v, size);

				texelPtr += surface[iCubeFace].GetNumChannels();
			}         
		}
	}
}

void CubeMapProcessor::BuildNormalizerCubemap( int size, SurfaceImage* surface, int fixupType )
{
	int iCubeFace, u, v;

	//iterate over cube faces
	for(iCubeFace=0; iCubeFace < 6; iCubeFace++)
	{
		surface[iCubeFace].Clear();
		surface[iCubeFace].Init(size, size, 3);  //First three channels for norm cube, and last channel for solid angle

		//fast texture walk, build normalizer cube map
		float* texelPtr = surface[iCubeFace].GetImageData();

		const int imgWidth = surface[iCubeFace].GetWidth();
		const int imgHeight = surface[iCubeFace].GetHeight();

		for(v=0; v < imgHeight; v++)
		{
			for(u=0; u < imgWidth; u++)
			{
				glm::vec3 dir = TexelCoordToVect(iCubeFace, (float)u, (float)v, size, fixupType);

				*(texelPtr + 0) = dir[0];
				*(texelPtr + 1) = dir[0];
				*(texelPtr + 2) = dir[0];

				texelPtr += surface[iCubeFace].GetNumChannels();
			}         
		}
	}
}

void CubeMapProcessor::Init( int inputSize, int outputSize, int numChannels )
{
	mInputSize = inputSize;
	mOutputSize = outputSize;
	mNumChannels = numChannels;

	//Iterate over faces for input images
	for(int i=0; i<6; i++)
	{
		mSrcCubeMap[i].Init(inputSize, inputSize, numChannels );
		mDstCubeMap[i].Init(outputSize, outputSize, numChannels ); 
	}
}

void CubeMapProcessor::SetInputFaceData( int faceIdx, int srcType, int srcNumChannels, int srcPitch, void *srcDataPtr, float maxClamp, float degamma, float scale )
{
	
}

void CubeMapProcessor::SetInputFaceData( int faceIdx, int srcType, int srcNumChannels, int srcPitch, void* srcDataPtr )
{
	mSrcCubeMap[faceIdx].SetImageData(srcType, srcNumChannels, srcPitch, srcDataPtr);
}

namespace {



	union Pixel32
	{
		Pixel32()
		{
			b = 255;
			g = 255;
			r = 255;
			a = 255;
		}
		
		Pixel32(uint8_t bi, uint8_t gi, uint8_t ri, uint8_t ai = 255)
		{
			b = bi;
			g = gi;
			r = ri;
			a = ai;
		}

		uint8_t integer;

		struct
		{
#ifdef BIG_ENDIAN
			uint8_t a, r, g, b;
#else // BIG_ENDIAN
			uint8_t b, g, r, a;
#endif // BIG_ENDIAN
		};
	};

	// TGA Header struct to make it simple to dump a TGA to disc.

#if defined(_MSC_VER) || defined(__GNUC__)



#pragma pack(push, 1)
#pragma pack(1)               // Dont pad the following struct.
#endif

	struct TGAHeader
	{
		uint8_t   idLength,           // Length of optional identification sequence.
			paletteType,        // Is a palette present? (1=yes)
			imageType;          // Image data type (0=none, 1=indexed, 2=rgb,
		// 3=grey, +8=rle packed).
		uint16_t  firstPaletteEntry,  // First palette index, if present.
			numPaletteEntries;  // Number of palette entries, if present.
		uint8_t   paletteBits;        // Number of bits per palette entry.
		uint16_t  x,                  // Horiz. pixel coord. of lower left of image.
			y,                  // Vert. pixel coord. of lower left of image.
			width,              // Image width in pixels.
			height;             // Image height in pixels.
		uint8_t   depth,              // Image color depth (bits per pixel).
			descriptor;         // Image attribute flags.
	};

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif

	bool
		WriteTGA(const std::string &filename,
		const Pixel32 *pxl,
		uint16_t width,
		uint16_t height)
	{
		std::ofstream file(filename.c_str(), std::ios::binary);
		if (file)
		{
			TGAHeader header;
			memset(&header, 0, sizeof(TGAHeader));
			header.imageType  = 2;
			header.width = width;
			header.height = height;
			header.depth = 32;
			header.descriptor = 0x20;

			file.write((const char *)&header, sizeof(TGAHeader));
			file.write((const char *)pxl, sizeof(Pixel32) * width * height);

			return true;
		}
		return false;
	}

}


void CubeMapProcessor::Dump( const char* file, SurfaceImage* cubeMap )
{
	const int size = cubeMap[0].GetWidth();
	const int numChannel = cubeMap[0].GetNumChannels();
	const int width = size * 3;
	const int height = size * 4;
	

	std::vector<Pixel32> data(width * height);
	float* faceData;

	faceData = cubeMap[CMF_Positive_Y].GetImageData();
	for (int y = 0; y < size; ++y)
	{
		for (int  x = size; x < size + size; ++x)
		{
			data[y * width + x].r = uint8_t(faceData[(y*size + x - size)*numChannel + 0] * 255);
			data[y * width + x].g = uint8_t(faceData[(y*size + x - size)*numChannel + 1] * 255);
			data[y * width + x].b = uint8_t(faceData[(y*size + x - size)*numChannel + 2] * 255);
		}
	}

	faceData = cubeMap[CMF_Negtive_X].GetImageData();
	for (int y = size; y < size + size; ++y)
	{
		for (int  x = 0; x < size; ++x)
		{
			data[y * width + x].r = uint8_t(faceData[((y-size)*size + x)*numChannel + 0] * 255);
			data[y * width + x].g = uint8_t(faceData[((y-size)*size + x)*numChannel + 1] * 255);
			data[y * width + x].b = uint8_t(faceData[((y-size)*size + x)*numChannel + 2] * 255);
		}
	}

	faceData = cubeMap[CMF_Negtive_Z].GetImageData();
	for (int y = size; y < size + size; ++y)
	{
		for (int  x = size; x < size + size; ++x)
		{
			data[y * width + x].r = uint8_t(faceData[((y-size)*size + x - size)*numChannel + 0] * 255);
			data[y * width + x].g = uint8_t(faceData[((y-size)*size + x - size)*numChannel + 1] * 255);
			data[y * width + x].b = uint8_t(faceData[((y-size)*size + x - size)*numChannel + 2] * 255);
		}
	}

	faceData = cubeMap[CMF_Positive_X].GetImageData();
	for (int y = size; y < size + size; ++y)
	{
		for (int  x = size + size; x < width; ++x)
		{
			data[y * width + x].r = uint8_t(faceData[((y-size)*size + x - size - size)*numChannel + 0] * 255);
			data[y * width + x].g = uint8_t(faceData[((y-size)*size + x - size - size)*numChannel + 1] * 255);
			data[y * width + x].b = uint8_t(faceData[((y-size)*size + x - size - size)*numChannel + 2] * 255);
		}
	}

	faceData = cubeMap[CMF_Negtive_Y].GetImageData();
	for (int y = size * 2; y < size * 3; ++y)
	{
		for (int  x = size; x < size + size; ++x)
		{
			data[y * width + x].r = uint8_t(faceData[((y-size*2)*size + x - size)*numChannel + 0] * 255);
			data[y * width + x].g = uint8_t(faceData[((y-size*2)*size + x - size)*numChannel + 1] * 255);
			data[y * width + x].b = uint8_t(faceData[((y-size*2)*size + x - size)*numChannel + 2] * 255);
		}
	}

	faceData = cubeMap[CMF_Positive_Z].GetImageData();
	for (int y = size * 3; y < height; ++y)
	{
		for (int  x = size; x < size + size; ++x)
		{
			data[y * width + x].r = uint8_t(faceData[((size - 1 - (y-size*3))*size + x - size)*numChannel + 0] * 255);
			data[y * width + x].g = uint8_t(faceData[((size - 1 - (y-size*3))*size + x - size)*numChannel + 1] * 255);
			data[y * width + x].b = uint8_t(faceData[((size - 1 - (y-size*3))*size + x - size)*numChannel + 2] * 255);
		}
	}

	WriteTGA(file, &data[0], width, height);
}
