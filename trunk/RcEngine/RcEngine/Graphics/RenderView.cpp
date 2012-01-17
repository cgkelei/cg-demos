#include "RenderView.h"
#include "FrameBuffer.h"

namespace RcEngine {
namespace RenderSystem {

RenderView::RenderView()
	: mAttachedFrameBuffer(NULL)
{

}

RenderView::~RenderView()
{

}


void RenderView::OnBind( FrameBuffer* fb, unsigned int att )
{

}


void RenderView::OnUnbind( FrameBuffer* fb, unsigned int att )
{

}


} // RenderSystem
} // RcEngine
