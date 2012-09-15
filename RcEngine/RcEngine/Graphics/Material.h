#ifndef Material_h__
#define Material_h__

#include <Core/Prerequisites.h>
#include <Math/ColorRGBA.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/EffectAnnotation.h>

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

class _ApiExport Material 
{
public:
	Material(void);
	virtual ~Material(void);

	const String& GetName() const						{ return mName; }

	const shared_ptr<Effect>& GetEffect() const			{ return mEffect; }
	EffectTechnique* GetCurrentTechnique() const		{ return mCurrentTechnique; }
			
	MaterialParameter* GetCustomParameter(EffectParameterUsage usage);
	MaterialParameter* GetCustomParameter(const String& name);

	void SetAmbientColor(const ColorRGBA& ambient)		{ mAmbient = ambient; }
	void SetDiffuseColor(const ColorRGBA& diffuse)		{ mDiffuse = diffuse; }
	void SetSpecularColor(const ColorRGBA& specular)	{ mSpecular = specular; }

	void SetTexture(const String& texUint, const shared_ptr<Texture>& texture);
			
	virtual void ApplyMaterial();

	shared_ptr<Material> Clone();


public:
	static shared_ptr<Material> LoadFrom(Stream& source);

protected:			
	String mName;
	shared_ptr<Effect> mEffect;
	EffectTechnique* mCurrentTechnique;
	vector<MaterialParameter*> mCachedEffectParams;

	shared_ptr<DepthStencilState> mDepthStencilState;
	shared_ptr<BlendState> mBlendState;
	shared_ptr<RasterizerState> mRasterizerState;
	unordered_map<String, shared_ptr<SamplerState> > mSamplerStates;

	ColorRGBA mAmbient;
	ColorRGBA mDiffuse;
	ColorRGBA mSpecular;
	ColorRGBA mEmissive;
	float mPower;

	//shared_ptr<Texture> mTextures[MaxMaterialTextures];
	//unordered_map<int32_t, String> mTextureNames;
	unordered_map<String, shared_ptr<Texture> > mTextures;
			
};


} // Namespace RcEngine

#endif // Material_h__

