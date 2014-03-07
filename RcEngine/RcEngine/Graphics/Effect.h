#ifndef Effect_h__
#define Effect_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Resource/Resource.h>

namespace RcEngine {	

class EffectParameter;
class EffectConstantBuffer;

class _ApiExport Effect : public Resource
{
public:
	Effect(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~Effect();

	void SetCurrentTechnique(uint32_t index);
	void SetCurrentTechnique(const String& techName);					
	
	inline EffectTechnique* GetCurrentTechnique() const 					{ return mCurrTechnique; }
	inline uint32_t GetNumTechniques() const								{ return mTechniques.size(); }
						
	EffectTechnique* GetTechniqueByName(const String& techName) const;
	EffectTechnique* GetTechniqueByIndex(uint32_t index) const;

	EffectParameter* GetParameterByName(const String& paraName) const;
	
	shared_ptr<Resource> Clone();

public_internal:
	/**
	 * Only used by sub render system shader parameter set internal.
	 */
	EffectParameter* FetchShaderParameter(const String& name, EffectParameterType type, bool array);

	EffectConstantBuffer* FetchConstantBuffer(const String& name, uint32_t bufferSize);

protected:
	void LoadImpl();
	void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:
	String mEffectName;

	EffectTechnique* mCurrTechnique;
	vector<EffectTechnique*> mTechniques;
	
	std::vector<EffectConstantBuffer*> mConstantBuffers;
	std::map<String, EffectParameter*> mParameters;
};


} // Namespace RcEngine
#endif // Effect_h__