#ifndef Material_h__
#define Material_h__

#include <Core/Prerequisites.h>
#include <Math/ColorRGBA.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/EffectAnnotation.h>
#include <Graphics/EffectParameter.h>
#include <Resource/Resource.h>

namespace RcEngine {
	
static const int32_t MaxMaterialTextures = 16;

struct _ApiExport MaterialParameter
{
	EffectParameterType Type;
	EffectParameterUsage Usage;
	bool IsSemantic;
	String Name;
	EffectParameter* EffectParam;
};

class _ApiExport Material : public Resource
{
public:
	Material(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	virtual ~Material(void);

	const String& GetName() const						{ return mName; }

	const shared_ptr<Effect>& GetEffect() const			{ return mEffect; }
	EffectTechnique* GetCurrentTechnique() const		{ return mCurrentTech; }

	void SetCurrentTechnique(const String& techName);
	void SetCurrentTechnique(uint32_t index);
			
	EffectParameter* GetCustomParameter(EffectParameterUsage usage) const;
	EffectParameter* GetCustomParameter(const String& name) const;

	void SetAmbientColor(const ColorRGBA& ambient)		{ mAmbient = ambient; }
	void SetDiffuseColor(const ColorRGBA& diffuse)		{ mDiffuse = diffuse; }
	void SetSpecularColor(const ColorRGBA& specular)	{ mSpecular = specular; }

	void SetTexture(const String& texUint, const shared_ptr<Texture>& texture);

	bool Transparent() const { return mTransparent; }
			
	void ApplyMaterial();

	shared_ptr<Resource> Clone();

protected:

	void LoadImpl();
    void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:	
	
	String mMaterialName;

	shared_ptr<Effect> mEffect;
	EffectTechnique* mCurrentTech;

	vector<MaterialParameter*> mCachedEffectParams;

	unordered_map<String, shared_ptr<SamplerState> > mSamplerStates;

	ColorRGBA mAmbient;
	ColorRGBA mDiffuse;
	ColorRGBA mSpecular;
	ColorRGBA mEmissive;
	float mPower;

	bool mTransparent;

	unordered_map<String, TextureLayer > mTextures;
			
};


} // Namespace RcEngine

#endif // Material_h__

