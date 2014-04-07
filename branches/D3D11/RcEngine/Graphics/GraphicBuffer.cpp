#include <Graphics/GraphicBuffer.h>

namespace RcEngine {


GraphicsBuffer::GraphicsBuffer(BufferUsage usage, uint32_t accessHint)
	: mBufferUsage(usage), mAccessHint(accessHint), mSizeInBytes(0)
{
}


GraphicsBuffer::~GraphicsBuffer(void)
{

}

} // Namespace RcEngine