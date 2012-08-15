#include <Graphics/RenderView.h>
#include <Graphics/FrameBuffer.h>

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


} // Namespace Render
} // Namespace RcEngine
