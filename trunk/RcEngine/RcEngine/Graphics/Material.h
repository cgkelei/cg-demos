#ifndef Material_h__
#define Material_h__

#include "Core/Prerequisites.h"
#include "Math/ColorRGBA.h"
#include "Graphics/DepthStencilState.h"
#include "Graphics/SamplerState.h"
#include "Graphics/BlendState.h"
#include "Graphics/RasterizerState.h"
#include "Graphics/EffectAnnotation.h"

namespace RcEngine
{
	//Forward declaration 
	namespace Content
	{
		class MaterialContentLoader;
	}

	namespace Render
	{	
		// using 
		using Math::ColorRGBA;
		using Content::MaterialContentLoader;

		static const int32_t MaxMaterialTextures = 16;

		class _ApiExport MaterialParameter
		{
		public:
			EffectParameterType Type;
			EffectParameterUsage Usage;
			bool IsSemantic;
			String Name;
			EffectAnnotationList Annotations;
			EffectParameter* EffectParam;
		};

		class _ApiExport Material 
		{
		public:
			Material(void);
			virtual ~Material(void);

			const shared_ptr<Effect>& GetEffect() const			{ return mEffect; }
			EffectTechnique* GetCurrentTechnique() const		{ return mCurrentTechnique; }
			
			MaterialParameter* GetCustomParameter(EffectParameterUsage usage);

			virtual void ApplyMaterial();


		public:
			static shared_ptr<Material> LoadFrom(MaterialContentLoader* loader);

		protected:			
			String mName;
			shared_ptr<Effect> mEffect;
			EffectTechnique* mCurrentTechnique;
			vector<MaterialParameter*> mCachedEffectParams;

			shared_ptr<DepthStencilState> mDepthStencilState;
			shared_ptr<BlendState> mBlendState;
			shared_ptr<RasterizerState> mRasterizerState;

			ColorRGBA mAmbient;
			ColorRGBA mDiffuse;
			ColorRGBA mSpecular;
			ColorRGBA mEmissive;
			float mPower;

			shared_ptr<Texture> mTextures[MaxMaterialTextures];
			unordered_map<int32_t, String> mTextureNames;
			
		};
	}
}

#endif // Material_h__

