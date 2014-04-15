#include "RHView.h"

namespace RcEngine {

RHTextureSRV::RHTextureSRV( const shared_ptr<RHTexture>& texture )
{
	mViewDimension = SRV_Texture;
}


RHBufferSRV::RHBufferSRV( const shared_ptr<RHBuffer>& buffer )
{
	mViewDimension = SRV_Buffer;
}


RHTextureUAV::RHTextureUAV( const shared_ptr<RHTexture>& texture )
{
	mViewDimension = UAV_Texture;
}


RHBufferUAV::RHBufferUAV( const shared_ptr<RHBuffer>& buffer )
{
	mViewDimension = UAV_Buffer;
}

}
