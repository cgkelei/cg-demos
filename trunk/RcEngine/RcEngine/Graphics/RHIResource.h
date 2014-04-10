#ifndef RHIResource_h__
#define RHIResource_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class _ApiExport RHIResouce
{
public:
	virtual ~RHIResouce();
};

class _ApiExport RHIBuffer : public RHIResouce
{
public:

};

class _ApiExport RHIVertexBuffer : public RHIBuffer
{

};

class _ApiExport RHIIndexBuffer : public RHIBuffer
{

};

class _ApiExport RHIUniformBuffer : public RHIBuffer
{

};

class _ApiExport StructuredBuffer : public RHIBuffer
{

};


class _ApiExport RHITexture : public RHIResouce
{
public:

};


class _ApiExport RHIShader : public RHIResouce
{
public:

};










class _ApiExport VertexBufferView 
{

};

class _ApiExport IndexBufferView
{

};


class _ApiExport VertexBuffer
{

};

class _ApiExport IndexBuffer
{

};

class _ApiExport UniformBuffer
{
public:
	UniformBuffer();
	~UniformBuffer();

private:

};

class _ApiExport StructedBuffer
{
public:

private:

};


}



#endif // RHIResource_h__
