#ifndef CubeMapProcessor_h__
#define CubeMapProcessor_h__

#include <glm/glm.hpp>
#include "SurfaceImage.h"

class CubeMapProcessor
{
public:
	enum EdgeFixup 
	{
		EF_Stretch,
		EF_Wrap,
		EF_Bent
	};

	enum CubeMapFace
	{
		CMF_Positive_X = 0,
		CMF_Negtive_X = 1,
		CMF_Positive_Y = 2,
		CMF_Negtive_Y = 3,
		CMF_Positive_Z = 4,
		CMF_Negtive_Z= 5,
	};

public:
	CubeMapProcessor(void);
	~CubeMapProcessor(void);

	void Init(int inputSize, int outputSize, int numChannels);
	void SetInputFaceData(int faceIdx, int srcType, int srcNumChannels, int srcPitch, 
		void *srcDataPtr, float maxClamp, float degamma, float scale );

	void SetInputFaceData(int faceIdx, int srcType, int srcNumChannels, int srcPitch, void* srcDataPtr );

	void SHProjectCubeMap(int order, bool useSolidAngleWeighting, int fixupType, double* pROut, double* pGOut, double* pBOut);
	void SHIrrandianceFilterCubeMap(int order, bool useSolidAngleWeighting, int fixupType);

	void Dump( const char* file, SurfaceImage* cubeMap );

private:
	void BuildNormalizerSolidAngleCubemap(int size, SurfaceImage* surface, int fixupType);
	void BuildNormalizerCubemap(int size, SurfaceImage* surface, int fixupType);

public:
	SurfaceImage mDstCubeMap[6];
	SurfaceImage mSrcCubeMap[6]; //normalizer cube map and solid angle lookup table
	SurfaceImage mNormCubeMap[6];

	int mInputSize, mOutputSize, mNumChannels;
	

};

#endif // CubeMapProcessor_h__
