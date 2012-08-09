#include "GraphicBuffer.h"

namespace RcEngine {
namespace Render {

GraphicsBuffer::GraphicsBuffer(BufferUsage usage, uint32_t accessHint)
	: mBufferUsage(usage), mAccessHint(accessHint)
{
}


GraphicsBuffer::~GraphicsBuffer(void)
{
	int a = 0;
}

BufferUsage GraphicsBuffer::GetBufferUsage() const
{
	return mBufferUsage;
}

uint32_t GraphicsBuffer::GetAccessHint() const
{
	return mAccessHint;
}

uint32_t GraphicsBuffer::GetBufferSize() const
{
	return mSizeInBytes;
}

} // RenderSystem
} // RcEngine