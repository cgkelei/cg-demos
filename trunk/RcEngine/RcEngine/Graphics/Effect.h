#ifndef Effect_h__
#define Effect_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Resource/Resource.h>


namespace RcEngine {	

class EffectParameter;

class _ApiExport Effect : public Resource
{
public:
	typedef std::map<String, EffectParameter*> EffectParameterMap;

public:
	Effect(uint32_t resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~Effect();

	vector<EffectTechnique*>& GetTechniques()	{ return mTechniques; }
	EffectTechnique* GetTechniqueByName(const String& techName);
	EffectTechnique* GetTechniqueByIndex(uint32_t index);

	EffectParameter* GetParameterByName(const String& paraName) const;


	/**
	 * Only used by sub render system shader parameter set internal.
	 */
	EffectParameter* AddShaderParameterInternal(const String& name, EffectParameterType type, bool array);

protected:
	void LoadImpl();
	void UnloadImpl();


public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:
	String mEffectName;
	EffectParameterMap mParameters;
	vector<EffectTechnique*> mTechniques;
};


} // Namespace RcEngine
#endif // Effect_h__