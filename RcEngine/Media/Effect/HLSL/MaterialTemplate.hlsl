#ifdef _DiffuseMap
	Texture2D DiffuseMap;
#else
	float3 DiffuseColor;
#endif

#ifdef _SpecularMap
	Texture2D SpecularMap;
#else
	float3 SpecularColor;
#endif

#ifndef _GlossyMap
	float Shininess;
#endif	

#ifdef _NormalMap
	Texture2D NormalMap;
#endif

#if defined(_DiffuseMap) || defined(_SpecularMap) || defined(_NormalMap)
	SamplerState LinearSampler;
#endif