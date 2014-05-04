#ifndef ModelMaterialFactory_h__
#define ModelMaterialFactory_h__

// Model material input
#ifdef _DiffuseMap
	uniform sampler2D DiffuseMap;
	// SamplerState binding
	#pragma DiffuseMap : MaterialSampler 
#else 
	uniform vec3 DiffuseColor;
#endif

#ifdef _SpecularMap
	uniform sampler2D SpecularMap;
	// SamplerState binding
	#pragma SpecularMap : MaterialSampler 
#else
	uniform vec3 SpecularColor;
#endif

#ifndef _GlossyMap
	uniform float Shininess;
#endif	

#ifdef _NormalMap
	uniform sampler2D NormalMap;
	// SamplerState binding
	#pragma NormalMap : MaterialSampler 
#endif

#endif // ModelMaterialFactory_h__
