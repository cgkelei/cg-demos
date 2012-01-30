#include "GraphicBuffer.h"

namespace RcEngine {
namespace Render {

GraphicsBuffer::GraphicsBuffer(BufferUsage usage, uint32 accessHint)
	: mBufferUsage(usage), mAccessHint(accessHint)
{
}


GraphicsBuffer::~GraphicsBuffer(void)
{
}

BufferUsage GraphicsBuffer::GetBufferUsage() const
{
	return mBufferUsage;
}

uint32 GraphicsBuffer::GetAccessHint() const
{
	return mAccessHint;
}

uint32 GraphicsBuffer::GetBufferSize() const
{
	return mSizeInBytes;
}

} // RenderSystem
} // RcEngine