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
	PrimitiveType mPrimitiveType;
	vector< shared_ptr<RHBuffer> > mVertexStreams;

	IndexBufferType IndexType;
	shared_ptr<RHBuffer> mIndexBuffer;
	
	shared_ptr<RHVertexDeclaration> mVertexDecl; 

	uint32_t mVertexStart;
	uint32_t mVertexCount;

	uint32_t mIndexStart;
	uint32_t mIndexCount;

	uint32_t mNumInstances;
};


}



#endif // RHOperation_h__
