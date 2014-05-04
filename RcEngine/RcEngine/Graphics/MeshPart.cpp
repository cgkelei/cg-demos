#include <Graphics/MeshPart.h>
#include <Graphics/Mesh.h>
#include <Graphics/Material.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderOperation.h>
#include <IO/Stream.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Core/Exception.h>

#define InvalidMaterialID UINT32_MAX

namespace RcEngine {

MeshPart::MeshPart(Mesh& mesh)
	: mParentMesh(mesh), mVertexBuffer(nullptr), mIndexBuffer(nullptr), 
	  mIndexCount(0), mPrimitiveCount(0), mIndexStart(0), mVertexStart(0)
{

}

void MeshPart::SetVertexBuffer( const shared_ptr<GraphicsBuffer>& vb, const shared_ptr<VertexDeclaration>& vd )
{
	mVertexBuffer = vb;
	mVertexDecl = vd;
}


void MeshPart::SetIndexBuffer( const shared_ptr<GraphicsBuffer>& indexBuffer, IndexBufferType format )
{
	mIndexBuffer = indexBuffer;
	mIndexFormat = format;
}


void MeshPart::Load(  Stream& source )
{
	// read name
	mName = source.ReadString();

	// read material name
	mMaterialName = source.ReadString(); 

	// read bounding box
	float3 min, max;
	source.Read(&min, sizeof(float3));
	source.Read(&max, sizeof(float3));
	mBoundingBox = BoundingBoxf(min, max);

	// read vertex count
	uint32_t vertexCount = source.ReadUInt();
	uint32_t vertexSize =  source.ReadUInt();

	// read vertex declaration
	uint32_t veCount = source.ReadUInt();
	vector<VertexElement> elements;
	elements.resize(veCount);

	for (size_t i = 0; i < veCount; ++i)
	{
		elements[i].Offset = source.ReadUInt();
		elements[i].Type =  static_cast<VertexElementFormat>(source.ReadUInt());
		elements[i].Usage =  static_cast<VertexElementUsage>(source.ReadUInt());
		elements[i].UsageIndex = source.ReadUShort();
	}

	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	// create vertex declaration
	mVertexDecl = factory->CreateVertexDeclaration(&elements[0], elements.size());

	if (vertexSize != mVertexDecl->GetVertexSize())
	{
		ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, "Vertex size in model is not same \
															as the vertex declaration", "MeshPart::Load");
	}

	uint32_t vertexBufferSize = vertexSize * vertexCount;

	// create vertex buffer
	mVertexBuffer = factory->CreateVertexBuffer(vertexBufferSize, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Vertex, nullptr);

	void* data = mVertexBuffer->Map(0, vertexBufferSize, RMA_Write_Discard);
	source.Read(data, vertexBufferSize);
	mVertexBuffer->UnMap();

	mVertexStart = 0;
	mVertexCount = vertexCount;

	uint32_t indexCount = source.ReadUInt();
	IndexBufferType indexFormat = (IndexBufferType)source.ReadUInt();
	uint32_t indexBufferSize;
	if (indexFormat == IBT_Bit32)
	{
		indexBufferSize = sizeof(uint32_t) * indexCount;		
	}else
	{
		indexBufferSize = sizeof(uint16_t) * indexCount;		
	}

	// create index buffer
	ElementInitData iInitData;
	iInitData.pData = NULL;
	iInitData.rowPitch = indexBufferSize;
	iInitData.slicePitch = 0;
	mIndexBuffer = factory->CreateIndexBuffer(indexBufferSize, EAH_GPU_Read | EAH_CPU_Write, BufferCreate_Index, &iInitData);
	data = mIndexBuffer->Map(0, indexBufferSize, RMA_Write_Discard);
	source.Read(data, indexBufferSize);
	mIndexBuffer->UnMap();

	mPrimitiveCount = indexCount / 3;
	mIndexFormat = indexFormat;
	mIndexStart = 0;
	mIndexCount = indexCount;
}


void MeshPart::Save( Stream& source )
{

}


MeshPart::~MeshPart()
{

}

void MeshPart::GetRenderOperation( RenderOperation& op, uint32_t lodIndex )
{
	op.PrimitiveType = PT_Triangle_List;
	op.BindVertexStream(0, mVertexBuffer);
	op.VertexDecl = mVertexDecl;

	if (mIndexCount > 0)
	{
		// use indices buffer
		op.BindIndexStream(mIndexBuffer, mIndexFormat);
		op.SetIndexRange(mIndexStart, mIndexCount);
		op.VertexStart = mVertexStart;
	}
	else
	{
		op.SetVertexRange(mVertexStart, mVertexCount);
	}	
}

}
