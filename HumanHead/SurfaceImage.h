#pragma once
#include <vector>

#include <gl/glew.h>

//Data types processed by cube map processor
// note that UNORM data types use the full range 
// of the unsigned integer to represent the range [0, 1] inclusive
// the float16 datatype is stored as D3Ds S10E5 representation
#define CP_VAL_RGB8       0x8051
#define CP_VAL_RGBA8      0x8058
#define CP_VAL_BGRA       0x80E1
#define CP_VAL_RGBA16     0x805B
#define CP_VAL_Float16    0x881A 
#define CP_VAL_Float32    0x8814

#pragma comment(lib,"nvImage")

class SurfaceImage
{
public:
	SurfaceImage(void);
	~SurfaceImage(void);

	void Clear(void);
	void Init(int width, int height, int numChannels );

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	int GetNumChannels() const { return mNumChannels; }

	float* GetImageData() { return &mImgData[0]; }

	//copy data from external buffer into this CImageSurface
	void SetImageData(int srcType, int srcNumChannels, int srcPitch, void* srcDataPtr );

	void WritePfmFile(const char* fileName );

private:
	std::vector<float> mImgData;     //cubemap image data
	int mWidth;          //image width
	int mHeight;         //image height
	int mNumChannels;    //number of channels
};

