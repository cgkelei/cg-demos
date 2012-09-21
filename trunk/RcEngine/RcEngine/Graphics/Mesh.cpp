#include <Graphics/Mesh.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderJob.h>
#include <Graphics/Skeleton.h>
#include <Core/Context.h>
#include <Core/Exception.h>

#define InvalidMaterialID UINT32_MAX

namespace RcEngine {

MeshPart::MeshPart( const String& name, const shared_ptr<Mesh>& mesh )
:mName(name), mParentMesh(mesh), mMaterialID(InvalidMaterialID)
{
}

const shared_ptr<Material>& MeshPart::GetMaterial() const
{
	shared_ptr<Mesh> parent = mParentMesh.lock();
	return parent->GetMaterial(mMaterialID);
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


void MeshPart::Save( const shared_ptr<MeshPart>& meshPart, Stream& dest )
{
	// write submesh name
	dest.WriteString(meshPart->mName);

	// write submesh material name
	dest.WriteString(meshPart->GetMaterial()->GetName());

	// write vertices count
	dest.WriteUInt(meshPart->mVertexCount);

	// write vertex size
	dest.WriteUInt(meshPart->mVertexDecl->GetVertexSize());

	// write vertex declaration, element size
	dest.WriteUInt(meshPart->mVertexDecl->GetElementCount());

	// write each vertex element
	const std::vector<VertexElement>& elements = meshPart->mVertexDecl->GetElements();
	for (auto iter = elements.begin(); iter != elements.end(); ++iter)
	{
		const VertexElement& ve = *iter;
		dest.WriteUInt(ve.Offset);
		dest.WriteUInt(ve.Type);
		dest.WriteUInt(ve.Usage);
		dest.WriteUShort(ve.UsageIndex);
	}

	// write vertex buffer data
	uint32_t bufferSize = meshPart->mVertexBuffer->GetBufferSize();
	void* data = meshPart->mVertexBuffer->Map(0, bufferSize, BA_Read_Only);
	if (!data)
	{
		// Exception
	}
	dest.Write(data, bufferSize);
	meshPart->mVertexBuffer->UnMap();

	// write triangles count
	dest.WriteUInt(meshPart->mPrimitiveCount);
	bufferSize = meshPart->mIndexBuffer->GetBufferSize();
	data = meshPart->mIndexBuffer->Map(0, bufferSize, BA_Read_Only);
	if (data)
	{
		// Exception
	}
	dest.Write(data, bufferSize);
	meshPart->mIndexBuffer->UnMap();
}

shared_ptr<MeshPart> MeshPart::Load( const shared_ptr<Mesh>& mesh, Stream& source )
{
	String name = source.ReadString();

	// result meshpart
	shared_ptr<MeshPart> meshPart( new MeshPart(name, mesh) );

	//String matName = source.ReadString();

	// read bounding sphere
	Vector3f center;
	float radius;
	source.Read(&center, sizeof(Vector3f));
	radius = source.ReadFloat();
	meshPart->mBoundingSphere = BoundingSpheref(center, radius);

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

	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
			
	// create vertex declaration
	meshPart->mVertexDecl = factory.CreateVertexDeclaration(elements);
		
	if (vertexSize != meshPart->mVertexDecl->GetVertexSize())
	{
		ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, "Vertex size in model is not same \
			as the vertex declaration", "MeshPart::Load");
	}

	uint32_t vertexBufferSize = vertexSize * vertexCount;
			
	// create vertex buffer
	ElementInitData vInitData;
	vInitData.pData = NULL;
	vInitData.rowPitch = vertexBufferSize;
	vInitData.slicePitch = 0;
	meshPart->mVertexBuffer = factory.CreateVertexBuffer(BU_Static, 0, &vInitData);
	void* data = meshPart->mVertexBuffer->Map(0, vertexBufferSize, BA_Write_Only);
	source.Read(data, vertexBufferSize);
	meshPart->mVertexBuffer->UnMap();

	meshPart->mStartVertex = 0;
	meshPart->mVertexCount = vertexCount;
			
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
	meshPart->mIndexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);
	data = meshPart->mIndexBuffer->Map(0, indexBufferSize, BA_Write_Only);
	source.Read(data, indexBufferSize);
	meshPart->mIndexBuffer->UnMap();
			
	meshPart->mPrimitiveCount = indexCount / 3;
	meshPart->mIndexFormat = indexFormat;
	meshPart->mStartIndex = 0;
	meshPart->mIndexCount = indexCount;

	return meshPart;	
}

MeshPart::~MeshPart()
{
long v = mVertexBuffer.use_count();
GraphicsBuffer* bf = mVertexBuffer.get();
}

void MeshPart::GetRenderOperation( RenderOperation& op, uint32_t lodIndex )
{
	op.UseIndex = (mIndexCount != 0);

	if (op.UseIndex)
	{
		op.IndexBuffer = mIndexBuffer;
		op.IndexType = mIndexFormat;
		op.StartIndexLocation = 0;
	}

	op.StartVertexLocation = 0;
	op.BaseVertexLocation = 0;
	op.BindVertexStream(mVertexBuffer, mVertexDecl);
	op.PrimitiveType = PT_Triangle_List;
}

	

//////////////////////////////////////////////////////////////////////////
Mesh::Mesh( const String& name )
: mName(name)
{

}

Mesh::~Mesh()
{
long v = mMeshParts[0]->GetVertexBuffer().use_count();
}

void Mesh::SetMaterial( const shared_ptr<Material>& mat )
{
	mMaterials.clear();
	mMaterials.push_back(mat);

	for (auto iter = mMeshParts.begin(); iter != mMeshParts.end(); ++iter)
	{
		(*iter)->SetMaterialID(0);
	}
}

void Mesh::SetMaterial( size_t materialIndex, const shared_ptr<Material>& mat)
{
	assert(materialIndex < mMaterials.size());
	mMaterials[materialIndex] = mat;
}

void Mesh::SetMeshPartMaterial( size_t meshPartIndex, const shared_ptr<Material>& mat )
{
	mMaterials.push_back(mat);
	size_t matID = mMaterials.size() - 1;

	mMeshParts[meshPartIndex]->SetMaterialID(matID);
}

shared_ptr<Mesh> Mesh::Load( Stream& source )
{
	String meshName = source.ReadString();

	// result mesh
	shared_ptr<Mesh> mesh( new Mesh(meshName) );

	// read bounding sphere
	Vector3f center;
	float radius;
	source.Read(&center, sizeof(Vector3f));
	radius = source.ReadFloat();
	mesh->mBoundingSphere = BoundingSpheref(center, radius);

	// read bones
	

	uint32_t subMeshCount = source.ReadUInt();
	for (uint32_t i = 0 ; i < subMeshCount; ++i)
	{
		shared_ptr<MeshPart> subMesh = MeshPart::Load(mesh, source);
		mesh->mMeshParts.push_back(subMesh);
	}

	return mesh;
}

void Mesh::Save( const shared_ptr<Mesh>& mesh, Stream& dest )
{
	// write mesh name
	dest.WriteString(mesh->mName);

	// write submesh count
	dest.WriteUInt(mesh->mMeshParts.size());

	// write each submesh
	for (auto iter = mesh->mMeshParts.begin(); iter != mesh->mMeshParts.end(); ++iter)
	{
		MeshPart::Save((*iter), dest);
	}
}



} // Namespace RcEngine