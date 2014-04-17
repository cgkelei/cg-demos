#include <Graphics/RHFactory.h>

namespace RcEngine {

RHFactory::RHFactory()
{

}

shared_ptr<RHBlendState> RHFactory::CreateBlendState( const RHBlendStateDesc& desc )
{
	if (mBlendStatePool.find(desc) == mBlendStatePool.end())
	{
		mBlendStatePool[desc] = CreateBlendStateImpl(desc);
	}

	return mBlendStatePool[desc];
}

shared_ptr<RHSamplerState> RHFactory::CreateSamplerState( const RHSamplerStateDesc& desc )
{
	if (mSamplerStatePool.find(desc) == mSamplerStatePool.end())
	{
		mSamplerStatePool[desc] = CreateSamplerState(desc);
	}

	return mSamplerStatePool[desc];
}

shared_ptr<RHRasterizerState> RHFactory::CreateRasterizerState( const RHRasterizerStateDesc& desc )
{
	if (mRasterizerStatePool.find(desc) == mRasterizerStatePool.end())
	{
		mRasterizerStatePool[desc] = CreateRasterizerStateImpl(desc);
	}

	return mRasterizerStatePool[desc];
}

shared_ptr<RHDepthStencilState> RHFactory::CreateDepthStencilState( const RHDepthStencilStateDesc& desc )
{
	if (mDepthStecilStatePool.find(desc) == mDepthStecilStatePool.end())
	{
		mDepthStecilStatePool[desc] = CreateDepthStencilStateImpl(desc);
	}

	return mDepthStecilStatePool[desc];
}

}