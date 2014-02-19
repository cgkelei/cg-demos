#ifndef Material_h__
#define Material_h__

#include <Core/Prerequisites.h>
#include <Math/ColorRGBA.h>
#include <Math/Matrix.h>
#include <Graphics/GraphicsCommon.h>
#include <Resource/Resource.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/RenderQueue.h>


namespace RcEngine {
	
static const int32_t MaxMaterialTextures = 16;

struct _ApiExport MaterialParameter
{
	String Name;
	bool IsSemantic;
	EffectParameterType Type;
	EffectParameterUsage Usage;
	EffectParameter* EffectParam;
};

class _ApiExport Material : public Resource
{
public:
	Material(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	virtual ~Material(void);

	const String& GetName() const						{ return mResourceName; }
	uint32_t GetQueueBucket() const						{ return mQueueBucket; }

	shared_ptr<Effect> GetEffect() const				{ return mEffect; }
	
	EffectTechnique* GetCurrentTechnique() const;
	void SetCurrentTechnique(const String& techName);
	void SetCurrentTechnique(uint32_t index);
			
	EffectParameter* GetCustomParameter(EffectParameterUsage usage) const;
	EffectParameter* GetCustomParameter(const String& name) const;

	void SetAmbientColor(const ColorRGBA& ambient)		{ mAmbient = ambient; }
	void SetDiffuseColor(const ColorRGBA& diffuse)		{ mDiffuse = diffuse; }
	void SetSpecularColor(const ColorRGBA& specular)	{ mSpecular = specular; }

	void SetTexture(const String& name, const shared_ptr<Texture>& texture);

	// Apply shader parameter before rendering, called by renderable
	void ApplyMaterial(const float4x4& world = float4x4::Identity());

	shared_ptr<Resource> Clone();

protected:

	void LoadImpl();
    void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:	
	
	String mMaterialName;
	shared_ptr<Effect> mEffect;
	unordered_map<String, shared_ptr<SamplerState> > mSamplerStates;
	unordered_map<String, TextureLayer> mTextures;			
	vector<MaterialParameter*> mCachedEffectParams;

	ColorRGBA mAmbient;
	ColorRGBA mDiffuse;
	ColorRGBA mSpecular;
	ColorRGBA mEmissive;
	float mPower;

	uint32_t mQueueBucket;
};


} // Namespace RcEngine

#endif // Material_h__

