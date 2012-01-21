#pragma once
#ifndef GraphicBuffer_h__
#define GraphicBuffer_h__

#include "GraphicsCommon.h"

namespace RcEngine {
namespace Render {

class _ApiExport GraphicBuffer
{
public:
	GraphicBuffer(BufferUsage usage, uint32 accessHint);
	~GraphicBuffer(void);

	BufferUsage GetBufferUsage() const;
	uint32		GetAccessHint() const;
	uint32		GetBufferSize() const;

	virtual void* Map(uint32 offset, uint32 length, BufferAccess options) = 0;
	virtual void UnMap() = 0;

protected:
	BufferUsage mBufferUsage;
	uint32 mAccessHint;
	uint32 mSizeInBytes;
};

} // RenderSystem
} // RcEngine
#endif // GraphicBuffer_h__