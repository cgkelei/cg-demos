#pragma once
#ifndef _RenderPass__H
#define _RenderPass__H

#include "Prerequisites.h"
#include "ColorRGBA.h"
#include "RenderState.h"

namespace Engine
{
	class _ApiExport RenderPass
	{
	private:
		// non copycable
		RenderPass( const RenderPass& );
		RenderPass& operator = (const RenderPass& );

	public:
		explicit RenderPass( RenderEffect& effect )
			: mEffetct( effect ), 
			  mFrontStencilReference(0),
			  mBackStencilReference(0)
		{

		}

		void Load( const XMLNodePtr& node, uint32 techIndex, uint32 passIndex );

		RenderPassPtr Clone(RenderEffect& effect);
	
		// Set the state contained in a pass to the device.
		bool Bind(void);
		bool Unbind(void);
			
		/// Gets the index of this Pass in the parent Technique
		uint16 GetIndex(void) const { return mIndex; }
		String GetName(void) const  { return mName; }

		const RasterizerStatePtr& GetRasterizerState() const		{ return mRasterizerState; }
		const DepthStencilStatePtr& GetDepthStencilState() const	{ return mDepthStencilState; }
		const BlendStatePtr& GetBlendState() const					{ return mBlendState; }
		const RasterizerStatePtr& GetRasterizerState() const		{ return mRasterizerState; }

		bool IsValidate() const { return mIsValiate; }


	protected:
		
		RenderEffect& mEffetct;
		
		uint16 mIndex; // pass index
		String mName; // optional name for the pass

		RasterizerStatePtr		 mRasterizerState;
		DepthStencilStatePtr     mDepthStencilState;
		BlendStatePtr			 mBlendState;
		uint32					 mFrontStencilReference;
		uint32					 mBackStencilReference;

		bool mIsValiate;

	    

#pragma warning(disable : 4251)

		std::vector<EffectAnnotationPtr> mAnnotations;

		std::vector<ShaderDesc>  mShaderDescs;

#pragma warning(default : 4251)

		
	};
}


#endif