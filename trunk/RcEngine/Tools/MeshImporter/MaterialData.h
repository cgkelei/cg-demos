#ifndef MaterialData_h__
#define MaterialData_h__

#include "Core/Prerequisites.h"
#include "Math/ColorRGBA.h"

struct MaterialData
{
	String Name;
	ColorRGBA Ambient;
	ColorRGBA Diffuse;
	ColorRGBA Specular;
	ColorRGBA Emissive;
	float Power;

	unordered_map<String, String> Textures;
};

#endif // MaterialData_h__