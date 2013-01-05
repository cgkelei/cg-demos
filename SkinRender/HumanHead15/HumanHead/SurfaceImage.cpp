#include "SurfaceImage.h"
#include "pfm.h"
#include "Utility.h"

//--------------------------------------------------------------------------------------
// convert D3D 16 bit float to standard 32 bit float
// Format:
// 
// 1 sign bit in MSB, (s) 
// 5 bits of biased exponent, (e) 
// 10 bits of fraction, (f), with an additional hidden bit 
// A float16 value, v, made from the format above takes the following meaning:
//
// (a) if e == 31 and f != 0, then v is NaN regardless of s 
// (b) if e == 31 and f == 0, then v = (-1)^s * infinity (signed infinity) 
// (c) if 0 < e < 31, then v = (-1)^s * 2^(e-15) * (1.f) 
// (d) if e == 0 and f != 0, then v = (-1)^s * 2^(e-14) * (0.f) (denormalized numbers) 
// (e) if e == 0 and f == 0, then v = (-1)^s *0 (signed zero) 
//
//--------------------------------------------------------------------------------------
float CPf16Tof32(unsigned short aVal)
{
	unsigned int signVal = (aVal >> 15);              //sign bit in MSB
	unsigned int exponent = ((aVal >> 10) & 0x01f);   //next 5 bits after signbit
	unsigned int mantissa = (aVal & 0x03ff);          //lower 10 bits
	unsigned int rawfloatData;                      //raw binary float data

	//convert s10e5  5-bit exponent to IEEE754 s23e8  8-bit exponent
	if(exponent == 31)
	{  // infinity or Nan depending on mantissa
		exponent = 255;
	}
	else if(exponent == 0) 
	{  //  denormalized floats  mantissa is treated as = 0.f
		exponent = 0;
	}
	else
	{  //change 15base exponent to 127base exponent 
		//normalized floats mantissa is treated as = 1.f
		exponent += (127 - 15);
	}

	//convert 10-bit mantissa to 23-bit mantissa
	mantissa <<= (23 - 10);

	//assemble s23e8 number using logical operations
	rawfloatData = (signVal << 31) |  (exponent << 23) | mantissa ;

	//treat raw data as a 32 bit float
	return *((float *) &rawfloatData );
}


//--------------------------------------------------------------------------------------
// convert standard 32 bit float to D3D 16 bit float
//
// 16-bit float format:
// 
// 1 sign bit in MSB, (s) 
// 5 bits of biased exponent, (e) 
// 10 bits of fraction, (f), with an additional hidden bit 
// A float16 value, v, made from the format above takes the following meaning:
//
// (a) if e == 31 and f != 0, then v is NaN regardless of s 
// (b) if e == 31 and f == 0, then v = (-1)s*infinity (signed infinity) 
// (c) if 0 < e < 31, then v = (-1)s*2(e-15)*(1.f) 
// (d) if e == 0 and f != 0, then v = (-1)s*2(e-14)*(0.f) (denormalized numbers) 
// (e) if e == 0 and f == 0, then v = (-1)s*0 (signed zero) 
//--------------------------------------------------------------------------------------
unsigned short CPf32Tof16(float aVal)
{
	unsigned int rawf32Data = *((unsigned int *)&aVal); //raw binary float data

	unsigned int signVal = (rawf32Data >> 31);              //sign bit in MSB
	unsigned int exponent = ((rawf32Data >> 23) & 0xff);    //next 8 bits after signbit
	unsigned int mantissa = (rawf32Data & 0x7fffff);        //mantissa = lower 23 bits

	unsigned short rawf16Data;

	//convert IEEE754 s23e8 8-bit exponent to s10e5  5-bit exponent      
	if(exponent == 255 ) 
	{//special case 32 bit float is inf or NaN, use mantissa as is
		exponent = 31;
	}
	else if(exponent < ((127-15)-10)  ) 
	{//special case, if  32-bit float exponent is out of 16-bit float range, then set 16-bit float to 0
		exponent = 0;
		mantissa = 0;
	}
	else if(exponent >= (127+(31-15)) )
	{  // max 15based exponent for s10e5 is 31
		// force s10e5 number to represent infinity by setting mantissa to 0
		//  and exponent to 31
		exponent = 31;
		mantissa = 0;
	}
	else if( exponent <= (127-15) )
	{  //convert normalized s23e8 float to denormalized s10e5 float

		//add implicit 1.0 to mantissa to convert from 1.f to use as a 0.f mantissa
		mantissa |= (1<<23);

		//shift over mantissa number of bits equal to exponent underflow
		mantissa = mantissa >> (1 + ((127-15) - exponent));

		//zero exponent to treat value as a denormalized number
		exponent = 0;
	}
	else
	{  //change 127base exponent to 15base exponent 
		// no underflow or overflow of exponent 
		//normalized floats mantissa is treated as= 1.f, so 
		// no denormalization or exponent derived shifts to the mantissa         
		exponent -= (127 - 15);
	}

	//convert 23-bit mantissa to 10-bit mantissa
	mantissa >>= (23 - 10);

	//assemble s10e5 number using logical operations
	rawf16Data = (signVal << 15) | (exponent << 10) | mantissa;

	//return re-assembled raw data as a 32 bit float
	return rawf16Data;
}


//--------------------------------------------------------------------------------------
//size of data types in bytes
//--------------------------------------------------------------------------------------
int CPTypeSizeOf(int type)
{
	switch(type)
	{
	case CP_VAL_RGB8:
	case CP_VAL_RGBA8:    
	case CP_VAL_BGRA:    
		return 1;
	case CP_VAL_RGBA16:
	case CP_VAL_Float16:
		return 2;
		break;
	case CP_VAL_Float32:
		return 4;
		break;
	default:
		return 1;
		break;
	}
}


//--------------------------------------------------------------------------------------
//get value of data pointed to by a_Ptr given type information
//--------------------------------------------------------------------------------------
float CPTypeGetVal(int type, void *ptr)
{
	switch(type)
	{
	case CP_VAL_RGB8:
	case CP_VAL_RGBA8:
	case CP_VAL_BGRA:
		return (1.0f/255.0f) * *((unsigned char *)ptr);
	case CP_VAL_RGBA16:
		return (1.0f/65535.0f) * *((unsigned short *)ptr);
	case CP_VAL_Float16:
		return CPf16Tof32( *((unsigned short *)ptr) );
	case CP_VAL_Float32:
		return *((float *) ptr);

	default:
		return 0;
		break;
	}
}


SurfaceImage::SurfaceImage(void)
{
}


SurfaceImage::~SurfaceImage(void)
{
}

void SurfaceImage::Clear( void )
{
	mWidth = 0;                      
	mHeight = 0;                     
	mNumChannels = 0;                 
	mImgData.clear();
}

void SurfaceImage::Init( int width, int height, int numChannels )
{
	mWidth = width;               
	mHeight = height;             
	mNumChannels = numChannels;    

	mImgData.resize( mWidth * mHeight * mNumChannels );
}

void SurfaceImage::SetImageData( int srcType, int srcNumChannels, int srcPitch, void* srcDataPtr )
{
	int i, j, k;

	float* dstDataWalk = &mImgData[0];
	unsigned char* srcDataWalk = (unsigned char*)srcDataPtr;

	int srcValueSize = CPTypeSizeOf(srcType);
	int srcTexelStep = srcValueSize * srcNumChannels;
	int numChannelsSet = (std::min)(srcNumChannels, mNumChannels);
	int srcChannelSelect;

	//loop over rows
	for(j=0; j < mHeight; j++)
	{
		//pointer arithmetic to offset pointer by pitch in bytes
		srcDataWalk = ( (unsigned char *)srcDataPtr + (j * srcPitch) );

		//loop over texels within row
		for(i=0; i < mWidth; i++)
		{
			srcChannelSelect = 0;

			//loop over channels within texel
			for(k=0; k<numChannelsSet; k++)
			{
				if(srcType == CP_VAL_BGRA || srcType == CP_VAL_RGB8) //swap channels 0, and 2 if in BGRA format
				{
					*(dstDataWalk + k) = CPTypeGetVal(srcType, srcDataWalk + srcChannelSelect);
				}
				else
				{
					/**(dstDataWalk + k) = CPTypeGetVal(srcType, srcDataWalk + srcChannelSelect);*/

					switch(k)
					{
					case 0:
						*(dstDataWalk + 2) = CPTypeGetVal(srcType, srcDataWalk + srcChannelSelect);
						break;
					case 2:
						*(dstDataWalk + 0) = CPTypeGetVal(srcType, srcDataWalk + srcChannelSelect);
						break;
					default:
						*(dstDataWalk + k) = CPTypeGetVal(srcType, srcDataWalk + srcChannelSelect);
						break;
					}
				}

				srcChannelSelect += srcValueSize;
			}   

			dstDataWalk += mNumChannels;
			srcDataWalk += srcTexelStep;
		}    
	}        
}

void SurfaceImage::WritePfmFile( const char* fileName )
{
	/*WritePfm(fileName, mWidth,mHeight, mNumChannels, &mImgData[0]);*/

	Utility::WriteTGARGBA(fileName, mWidth, mHeight, &mImgData[0]);
}
