#ifndef Effect_h__
#define Effect_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Resource/Resource.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {	

class EffectParameter;
class EffectUniformBuffer;

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

protected:
	void LoadImpl();
	void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

public_internal:
	EffectParameter* FetchSRVParameter(const String& name, EffectParameterType effectType);
	EffectParameter* FetchUAVParameter(const String& name, EffectParameterType effectType);
	EffectParameter* FetchUniformParameter(const String& name, EffectParameterType effectType, uint32_t elementSize);
	EffectUniformBuffer* FetchUniformBufferParameter(const String& name, uint32_t bufferSize);
	
protected:
	String mEffectName;

	EffectTechnique* mCurrTechnique;
	vector<EffectTechnique*> mTechniques;
	
	std::vector<EffectUniformBuffer*> mUniformBuffers;
	std::map<String, EffectParameter*> mParameters;
};

class _ApiExport EffectTechnique 
{
	friend class Effect;

public:
	EffectTechnique(Effect& effect);
	~EffectTechnique();

	inline const String& GetTechniqueName() const				{ return mName; }
	inline Effect& GetEffect()									{ return mEffect; }

	inline vector<EffectPass*>& GetPasses()					    { return mPasses; }
	inline const vector<EffectPass*>& GetPasses() const		    { return mPasses; }

	EffectPass* GetPassByName(const String& name) const;
	EffectPass* GetPassByIndex(uint32_t index) const;

protected:
	Effect& mEffect;

	String mName;
	bool mValid;
	vector<EffectPass*> mPasses;
};

class _ApiExport EffectPass
{
	friend class Effect;

public:
	EffectPass();

	inline const String& GetPassName() const	{ return mName; }

	void BeginPass();
	void EndPass();

protected:
	String mName;
	uint16_t mFrontStencilRef, mBackStencilRef;
	ColorRGBA mBlendColor;
	uint32_t mSampleMask;
	shared_ptr<RHBlendState> mBlendState;
	shared_ptr<RHDepthStencilState> mDepthStencilState;
	shared_ptr<RHRasterizerState> mRasterizerState;
	shared_ptr<RHShaderPipeline> mShaderProgram;
};


} // Namespace RcEngine
#endif // Effect_h__