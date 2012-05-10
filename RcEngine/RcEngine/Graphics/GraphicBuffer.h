#pragma once
#ifndef GraphicBuffer_h__
#define GraphicBuffer_h__

#include "GraphicsCommon.h"

namespace RcEngine {
namespace Render {

class _ApiExport GraphicsBuffer
{
public:
	GraphicsBuffer(BufferUsage usage, uint32_t accessHint);
	~GraphicsBuffer(void);

	BufferUsage GetBufferUsage() const;
	uint32_t		GetAccessHint() const;
	uint32_t		GetBufferSize() const;

	virtual void* Map(uint32_t offset, uint32_t length, BufferAccess options) = 0;
	virtual void UnMap() = 0;

protected:
	BufferUsage mBufferUsage;
	uint32_t mAccessHint;
	uint32_t mSizeInBytes;
};

} // RenderSystem
} // RcEngine
#endif // GraphicBuffer_h__