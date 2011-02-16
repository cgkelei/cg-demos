#ifndef _Texture___H
#define _Texture___H

#include "Image.h"

class Texture
{
public:
	enum  TextureType
	{
		Texture_1D,
		Texture_2D,
		Texture_3D,
		Texture_Cube
	};

	enum FilterType
	{
		NEAREST,          // nearest neighbor
		LINEAR,           // linear filtering
	};

	enum WrapType
	{
		CLAMP,
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_BORDER,
		CLAMP_EDGE,
		MAX_WRAP_TYPES
	};

public:
	Texture (const char* textureName, Image* pImage);
	virtual ~Texture ();

	virtual void setFilterType(FilterType filter);
	FilterType getFilterType() const;

protected: 
	FilterType		m_fiterType;
	

};

#endif