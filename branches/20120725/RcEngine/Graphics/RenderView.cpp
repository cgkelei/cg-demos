#include "RenderView.h"
#include "FrameBuffer.h"

namespace RcEngine {
namespace Render {

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


} // RenderSystem
} // RcEngine
