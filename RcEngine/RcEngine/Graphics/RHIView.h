#ifndef RHIView_h__
#define RHIView_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class _ApiExport BufferView
{
public:
	BufferView();
	virtual ~BufferView();

	virtual void* Map() = 0;
	virtual void UnMap() = 0; 

protected:

};

class _ApiExport VertexBufferView
{
public:


};

class _ApiExport IndexBufferView
{
public:


};

class _ApiExport UniformBufferView
{
public:

};

class _ApiExport ShaderResourceView
{


};

class _ApiExport UnorderedAccessView
{

};


class _ApiExport StructuredBufferSRV : public ShaderResourceView
{
public:


};

class _ApiExport StructuredBufferUAV : public UnorderedAccessView
{
public:

};

class _ApiExport TextureSRV : public ShaderResourceView
{
public:

};

class _ApiExport TextureUAV : public UnorderedAccessView
{
public:


};







}

#endif // RHIView_h__
