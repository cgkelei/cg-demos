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
	Effect(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~Effect();

	EffectTechnique* GetCurrentTechnique() const						{ return mCurrTechnique; }
	void SetCurrentTechnique(const String& techName);					
	void SetCurrentTechnique(uint32_t index);

	const vector<EffectTechnique*>& GetTechniques()	const				{ return mTechniques; }
	EffectTechnique* GetTechniqueByName(const String& techName);
	EffectTechnique* GetTechniqueByIndex(uint32_t index);

	const EffectParameterMap& GetAllParameters() const					{ return mParameters; }
	EffectParameter* GetParameterByName(const String& paraName) const;
	
	shared_ptr<Resource> Clone();

public_internal:
	/**
	 * Only used by sub render system shader parameter set internal.
	 */
	EffectParameter* FetchShaderParameter(const String& name, EffectParameterType type, bool array);

protected:
	void LoadImpl();
	void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:
	String mEffectName;
	EffectParameterMap mParameters;
	vector<EffectTechnique*> mTechniques;
	EffectTechnique* mCurrTechnique;
};


} // Namespace RcEngine
#endif // Effect_h__