#include <Graphics/RenderView.h>
#include <Graphics/FrameBuffer.h>

namespace RcEngine {

RenderView::RenderView()
	: mAttachedFrameBuffer(NULL)
{

}

RenderView::~RenderView()
{

}


void RenderView::OnBind( FrameBuffer* fb, uint32_t att )
{

}


void RenderView::OnUnbind( FrameBuffer* fb, uint32_t att )
{

}



} // Namespace RcEngine
