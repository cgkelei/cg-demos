#pragma once
#ifndef _RenderTechnique__H
#define _RenderTechnique__H

#include "Prerequisites.h"
#include "ColorRGBA.h"
#include "GraphicsCommon.h"

namespace Engine
{
	class _ApiExport RenderTechnique
	{
	private:
		// non copycable
		RenderTechnique( const RenderTechnique& );
		RenderTechnique& operator = (const RenderTechnique& );


	public:
		RenderTechnique(RenderEffect& effect);
		
		String GetName() const		     { return mName; }
		RenderEffect& GetEffect() const  { return mParent; }

		uint32 GetNumAnnotations() const { return static_cast<uint32>(mAnnotations.size()); }
		const EffectAnnotationPtr& GetAnnotation( uint32 index ) const  { assert( index < GetNumAnnotations() ); return mAnnotations[index]; }
		
		uint32 GetNumPasses() const      { return static_cast<uint32>(mPasses.size()); }
		const RenderPassPtr& GetPass( uint32 index ) const  { assert( index < GetNumPasses() ); return mPasses[index]; }
		
		bool IsValidate() const          { return mIsValidate; }
		bool IsTransparent() const		 { return mIsTransparent; }

		float GetWeight() const			 { return mWeight; }

		//void SetAmbient(float red, float green, float blue);
		//void SetAmbient(const ColorRGBA& ambient);
		//void SetDiffuse(float red, float green, float blue, float alpha);
		//void SetDiffuse(const ColorRGBA& diffuse);
		//void SetSpecular(float red, float green, float blue, float alpha);
		//void SetSpecular(const ColorRGBA& specular);
		//void SetShininess(float val);
		//void SetSelfIllumination(float red, float green, float blue);
		//void SetSelfIllumination(const ColorRGBA& selfIllum);
		//void SetDepthCheckEnabled(bool enabled);
		//void SetDepthWriteEnabled(bool enabled);
		//void SetDepthFunction( CompareFunction func );
		//void SetCullingMode( CullingMode mode );
		//void SetShadingMode( ShadingType mode );
		//void SetSourceBlend(BlendFactor srcBlend);
		//void SetDestinationBlend(BlendFactor dstBlend);
		//void SetLightingEnabled(bool enabled);
		//

	protected:


		typedef std::vector<RenderPassPtr> Passes;
		typedef Passes::iterator PassIter;
		
		
#pragma warning(disable : 4251)

		Passes mPasses;
		std::vector<EffectAnnotationPtr> mAnnotations;

#pragma warning(default : 4251)

		RenderEffect& mParent; 
		String mName;
		bool mIsTransparent;

		bool mIsValidate;

		float mWeight;

	};
}




#endif