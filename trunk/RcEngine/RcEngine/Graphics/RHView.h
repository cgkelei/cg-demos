#ifndef RHView_h__
#define RHView_h__

#include <Core/Prerequisites.h>
#include <Graphics/PixelFormat.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

class _ApiExport RHShaderResourceView
{
public:
	virtual ~RHShaderResourceView() {}
};

class _ApiExport RHUnorderedAccessView 
{
public:
	virtual ~RHUnorderedAccessView() {}
};

}

#endif // RHView_h__
