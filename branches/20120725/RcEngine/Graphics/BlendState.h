#ifndef BlendState_h__
#define BlendState_h__

#include "Core/Prerequisites.h"
#include "Graphics/GraphicsCommon.h"

namespace RcEngine
{
	namespace Render
	{
		class _ApiExport BlendStateDesc
		{
		public:
			struct RenderTargetBlendDesc {

				RenderTargetBlendDesc()
					: BlendEnable(false), SrcBlend(ABF_One), DestBlend(ABF_Zero), BlendOp(BOP_Add),
					  SrcBlendAlpha(ABF_One), DestBlendAlpha(ABF_Zero), BlendOpAlpha(BOP_Add), RenderTargetWriteMask(CWM_All)
				{

				}
				
				bool				BlendEnable;
				AlphaBlendFactor    SrcBlend;
				AlphaBlendFactor    DestBlend;
				BlendOperation		BlendOp;
				AlphaBlendFactor    SrcBlendAlpha;
				AlphaBlendFactor    DestBlendAlpha;
				BlendOperation		BlendOpAlpha;
				uint8_t				RenderTargetWriteMask;
			};

		public:
			BlendStateDesc();
			bool AlphaToCoverageEnable;
			bool IndependentBlendEnable;
			RenderTargetBlendDesc RenderTarget[8];
		};

		inline BlendStateDesc::BlendStateDesc()
			: AlphaToCoverageEnable(false), IndependentBlendEnable(false)
		{

		}

		class _ApiExport BlendState
		{
		public:
			BlendState(const BlendStateDesc& desc) 
				: mDesc(desc)
			{

			}

			const BlendStateDesc& GetDesc() const					{ return mDesc; }

		protected:
			BlendStateDesc mDesc;
		};
	}
}
#endif // BlendState_h__
