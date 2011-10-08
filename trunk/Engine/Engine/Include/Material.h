#pragma once
#ifndef _Material__H
#define _Material__H

#include "Prerequisites.h"
#include "GraphicsCommon.h"
#include "ColorRGBA.h"

namespace Engine
{
	
	struct TextureLayer 
	{
		String TextureName;

	};


	class _ApiExport Material
	{
	public:
		Material();
		virtual ~Material();

		bool IsTransparent() const  { return mIsTransparent; }

		void SetAmbient(float red, float green, float blue);
		void SetAmbient(const ColorRGBA& ambient);
		void SetDiffuse(float red, float green, float blue, float alpha);
		void SetDiffuse(const ColorRGBA& diffuse);
		void SetSpecular(float red, float green, float blue, float alpha);
		void SetSpecular(const ColorRGBA& specular);
		void SetShininess(float val);
		void SetSelfIllumination(float red, float green, float blue);
		void SetSelfIllumination(const ColorRGBA& selfIllum);
		void SetDepthCheckEnabled(bool enabled);
		void SetDepthWriteEnabled(bool enabled);
		void SetDepthFunction( CompareFunction func );
		void SetCullingMode( CullingMode mode );
		void SetShadingMode( ShadingType mode );
		void SetSourceBlend(BlendFactor srcBlend);
		void SetDestinationBlend(BlendFactor dstBlend);
		void SetLightingEnabled(bool enabled);

	protected:
		
		RenderEffect* mEffect;
		bool mIsTransparent;

	private:
	
	};
}



#endif