#include "GraphicBuffer.h"

namespace RcEngine {
namespace RenderSystem {

GraphicBuffer::GraphicBuffer(BufferUsage usage, uint32 accessHint)
	: mBufferUsage(usage), mAccessHint(accessHint)
{
}


GraphicBuffer::~GraphicBuffer(void)
{
}

BufferUsage GraphicBuffer::GetBufferUsage() const
{
	return mBufferUsage;
}

uint32 GraphicBuffer::GetAccessHint() const
{
	return mAccessHint;
}

uint32 GraphicBuffer::GetBufferSize() const
{
	return mSizeInBytes;
}

} // RenderSystem
} // RcEngine