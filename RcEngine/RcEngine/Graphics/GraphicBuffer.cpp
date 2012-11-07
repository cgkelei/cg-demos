#include <Graphics/GraphicBuffer.h>

namespace RcEngine {


GraphicsBuffer::GraphicsBuffer(BufferUsage usage, uint32_t accessHint)
	: mBufferUsage(usage), mAccessHint(accessHint), mSizeInBytes(0)
{
}


GraphicsBuffer::~GraphicsBuffer(void)
{

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


} // Namespace RcEngine