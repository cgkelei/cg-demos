#ifndef Effect_h__
#define Effect_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/EffectTechnique.h>
#include <Graphics/EffectParameter.h>


namespace RcEngine {	

class EffectParameter;

class _ApiExport Effect
{
public:
	typedef std::map<String, EffectParameter*> EffectParameterMap;

public:
	Effect();
	~Effect();

	vector<EffectTechnique*>& GetTechniques()	{ return mTechniques; }
	EffectTechnique* GetTechniqueByName(const String& techName);
	EffectTechnique* GetTechniqueByIndex(uint32_t index);

	EffectParameter* GetParameterByName(const String& paraName) const;


	/**
	 * Only used by sub render system shader parameter set internal.
	 */
	EffectParameter* AddShaderParameterInternal(const String& name, EffectParameterType type, bool array);

public:
	static shared_ptr<Effect> LoadFrom(Stream& source);

protected:
	String mName;
	EffectParameterMap mParameters;
	vector<EffectTechnique*> mTechniques;
};


} // Namespace RcEngine
#endif // Effect_h__