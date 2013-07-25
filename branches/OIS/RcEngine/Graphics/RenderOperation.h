#pragma once
#ifndef RenderOperation_h__
#define RenderOperation_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {


class _ApiExport RenderOperation
{
public:
	enum StreamType
	{
		ST_Geometry,
		ST_Instance
	};

	struct StreamUnit
	{
		shared_ptr<GraphicsBuffer> Stream;
		shared_ptr<VertexDeclaration> VertexDecl;
		uint32_t Frequency;
		StreamType Type;

	};

	typedef std::vector<StreamUnit> StreamSlots;

public:
	RenderOperation(void);
	~RenderOperation();

	PrimitiveType GetPrimitiveType() const							{ return PrimitiveType; }
	void SetPrimitiveType(PrimitiveType type)						{ PrimitiveType = type; }

	uint32_t GetVertexCount() const;

	uint32_t GetStartVertexLocation() const;
	void SetStartVertexLocation(uint32_t loc);
	uint32_t GetBaseVertexLocation() const;
	void SetBaseVertexLocation(uint32_t loc);
	uint32_t GetStartIndexLocation() const;
	void SetStartIndexLocation(uint32_t loc);

	uint32_t GetStreamCount() const;
	const StreamUnit& GetStreamUnit(uint32_t index) const;

	void BindVertexStream(const shared_ptr<GraphicsBuffer>& buffer, const shared_ptr<VertexDeclaration>& vd,
		StreamType type = ST_Geometry, uint32_t freq = 1);
	void BindIndexStream(const shared_ptr<GraphicsBuffer>& buffer, IndexBufferType type);

	bool UseIndices() const;
	uint32_t GetIndicesCount() const;

	IndexBufferType GetIndexType() const									{ return IndexType; }
	const shared_ptr<GraphicsBuffer>& GetIndexStream() const				{ return IndexBuffer; }			

public:
	PrimitiveType PrimitiveType;
	StreamSlots VertexStreams;
	bool UseIndex;
	shared_ptr<GraphicsBuffer> IndexBuffer;
	IndexBufferType IndexType;

	uint32_t StartVertexLocation;
	uint32_t StartIndexLocation;
	int32_t  BaseVertexLocation;
	uint32_t StartInstanceLocation;
};


} // Namespace RcEngine


#endif // RenderOperation_h__