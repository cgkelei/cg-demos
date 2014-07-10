#ifndef Mesh_h__
#define Mesh_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/BoundingBox.h>
#include <Math/Matrix.h>
#include <Resource/Resource.h>


namespace RcEngine {

class Skeleton;
class MeshPart;

/**
  MeshPart don't store a material reference, it only store a material name which is define 
  in mesh file. We do that because all the same mesh will load from disk only once, multiple
  Entity can shared same mesh, so material reference stores in entity(SubEntity), and different
  entity with same mesh model can have different material. But every different mesh copy may need
  a different Skeleton copy, it's different for every single entity.
*/

class _ApiExport Mesh : public Resource
{
public:
	Mesh(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group );
	virtual ~Mesh();

	const String& GetName() const								{ return mResourceName; }
	const BoundingBoxf& GetBoundingBox() const					{ return mBoundingBox; }

	uint32_t GetNumMeshPart() const								{ return mMeshParts.size(); }
	const shared_ptr<MeshPart>& GetMeshPart(size_t i) const		{ return mMeshParts[i]; }
	
	shared_ptr<Skeleton> GetSkeleton() const					{ return mSkeleton; }

	uint32_t GetPrimitiveCount() const							{ return mPrimitiveCount; }
	uint32_t GetVertexCount() const								{ return mVertexCount; }

	virtual shared_ptr<Resource> Clone();

protected:
	void LoadImpl();
	void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

private:		
	BoundingBoxf mBoundingBox;

	uint32_t mPrimitiveCount;
	uint32_t mVertexCount;

	vector<shared_ptr<MeshPart> > mMeshParts;  

	shared_ptr<Skeleton> mSkeleton;
};

class _ApiExport MeshPart
{
	friend class Mesh;

public:
	MeshPart(Mesh& mesh);
	~MeshPart();

	inline const String& GetName() const						{ return mName; }
	inline const BoundingBoxf& GetBoundingBox() const			{ return mBoundingBox; }

	inline uint32_t GetVertexCount() const						{ return mVertexCount; }
	inline uint32_t GetPrimitiveCount() const					{ return mPrimitiveCount; }
	inline uint32_t GetIndexCount() const						{ return mIndexCount; }
	inline uint32_t GetStartIndex() const						{ return mIndexStart; }
	inline uint32_t GetStartVertex() const						{ return mVertexStart;}

	const String& GetMaterialName() const						{ return mMaterialName; }

	void GetRenderOperation( RenderOperation& op, uint32_t lodIndex );

	void SetVertexBuffer(const shared_ptr<GraphicsBuffer>& vb, const shared_ptr<VertexDeclaration>& vd);
	const shared_ptr<GraphicsBuffer>& GetVertexBuffer() const			{ return mVertexBuffer; }
	const shared_ptr<VertexDeclaration>& GetVertexDeclaration() const	{ return mVertexDecl;}

	void SetIndexBuffer(const shared_ptr<GraphicsBuffer>& indexBuffer, IndexBufferType format);
	const shared_ptr<GraphicsBuffer>& GetIndexBuffer()	const	{ return mIndexBuffer; }
	IndexBufferType GetIndexFormat() const						{ return mIndexFormat;  }

	void Load(Stream& source);
	void Save(Stream& source);

private:
	Mesh& mParentMesh;

	String mName;
	String mMaterialName;

	shared_ptr<GraphicsBuffer>    mVertexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;

	shared_ptr<GraphicsBuffer> mIndexBuffer;
	IndexBufferType			   mIndexFormat;

	BoundingBoxf mBoundingBox;

	uint32_t mPrimitiveCount; // Only support triangle

	uint32_t mIndexStart;
	uint32_t mIndexCount;

	uint32_t mVertexStart;
	uint32_t mVertexCount;
};

} // Namespace RcEngine

#endif // Mesh_h__
