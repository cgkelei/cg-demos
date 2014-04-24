#ifndef RHOperation_h__
#define RHOperation_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

class _ApiExport RHOperation
{
public:
	RHOperation();
	~RHOperation();

	void SetVertexBuffer(uint32_t streamUnit, shared_ptr<RHBuffer> vertexBuffer);
	void SetIndexBuffer(shared_ptr<RHBuffer> indexBuffer, IndexBufferType indexType);
	
	// glDrawArrays, glDrawArraysInstanced
	void SetIndexRange(uint32_t indexStart, uint32_t indexCount, uint32_t numInstance = 0);
	
	// glDrawElements, glDrawElementsInstanced
	void SetVertexRange(uint32_t vertexStart, uint32_t vertexCount, uint32_t numInstance = 0);


public:
	PrimitiveType PrimitiveType;
	vector< shared_ptr<RHBuffer> > VertexStreams;

	IndexBufferType IndexType;
	shared_ptr<RHBuffer> IndexBuffer;
	
	shared_ptr<RHVertexDeclaration> VertexDecl; 

	// Non-index mode
	uint32_t VertexStart;
	uint32_t VertexCount;

	// Index mode 
	uint32_t IndexStart;
	uint32_t IndexCount;

	//int32_t BaseVertex; // indices[i] + basevertex

	// Instance 
	uint32_t NumInstances;
};


}



#endif // RHOperation_h__
