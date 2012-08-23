#ifndef Mesh_h__
#define Mesh_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/Renderable.h>
#include <Math/BoundingSphere.h>
#include <IO/Stream.h>

namespace RcEngine {

class _ApiExport MeshPart : public Renderable
{
public:
	MeshPart(const String& name, const shared_ptr<Mesh>& mesh);
	~MeshPart();

	const String& GetName() const								{ return mName; }
	const BoundingSpheref& GetBoundingSphere() const			{ return mBoundingSphere; }
			
	const shared_ptr<RenderOperation>& GetRenderOperation() const 	{ return mRenderOperation; }

	uint32_t GetVertexCount() const				{ return mVertexCount; }
	uint32_t GetPrimitiveCount() const			{ return mPrimitiveCount; }
	uint32_t GetIndexCount() const				{ return mIndexCount; }
	uint32_t GetStartIndex() const				{ return mStartIndex; }
	uint32_t GetStartVertex() const			    { return mStartVertex;}
			
	uint32_t GetMaterialID() const				{ return mMaterialID; }
	void SetMaterialID(size_t id)				{ mMaterialID = id; }
	const shared_ptr<Material>& GetMaterial() const;
			
	void SetVertexBuffer(const shared_ptr<GraphicsBuffer>& vb, const shared_ptr<VertexDeclaration>& vd);
	const shared_ptr<GraphicsBuffer>& GetVertexBuffer() const			{ return mVertexBuffer; }
	const shared_ptr<VertexDeclaration>& GetVertexDeclaration() const	{ return mVertexDecl;}

	void SetIndexBuffer(const shared_ptr<GraphicsBuffer>& indexBuffer, IndexBufferType format);
	const shared_ptr<GraphicsBuffer>& GetIndexBuffer()	const	{ return mIndexBuffer; }
	IndexBufferType GetIndexFormat() const						{ return mIndexFormat;  }
			
	void GetWorldTransforms(Matrix4f* xform) const;
	uint32_t GetWorldTransformsCount() const;


public:
	static shared_ptr<MeshPart> Load(const shared_ptr<Mesh>& mesh, Stream& source);
	static void Save(const shared_ptr<MeshPart>& meshPart, Stream& dest);
			
private:
	weak_ptr<Mesh> mParentMesh;

	String mName;
	uint32_t mMaterialID;

	shared_ptr<GraphicsBuffer>    mVertexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;

	shared_ptr<GraphicsBuffer> mIndexBuffer;
	IndexBufferType			   mIndexFormat;

	BoundingSpheref mBoundingSphere;
		
	uint32_t mPrimitiveCount; // Only support triangle
	
	uint32_t mStartIndex;
	uint32_t mIndexCount;

	uint32_t mStartVertex;
	uint32_t mVertexCount;
};


class _ApiExport Mesh
{
public:
	Mesh(const String& name);
	virtual ~Mesh();

	const String& GetName() const								{ return mName; }
	const BoundingSpheref& GetBoundingSphere() const			{ return mBoundingSphere; }

	const Matrix4f& GetWorldMatrix() const						{ return mWorld; }
	void SetWorldMatrix(const Matrix4f& mat)					{ mWorld = mat; }

	const shared_ptr<Material>& GetMaterial(size_t i) const 	{ return mMaterials[i]; }

	uint32_t GetMeshPartCount() const							{ return mMeshParts.size(); }
	const shared_ptr<MeshPart>& GetMeshPart(size_t i) const		{ return mMeshParts[i]; }
			

	/**
	 * Set all mesh part to the same material
	 */
	void SetMaterial(const shared_ptr<Material>& mat);

	
	void SetMaterial( size_t materialIndex, const shared_ptr<Material>& mat);

	/**
	 * Set a material to the given mesh part
	 */
	void SetMeshPartMaterial(size_t meshPartIndex, const shared_ptr<Material>& mat);


	static shared_ptr<Mesh> Load(Stream& source);
	static void Save(const shared_ptr<Mesh>& mesh, Stream& dest);

private:
	String mName;
			
	BoundingSpheref mBoundingSphere;

	uint32_t mPrimitiveCount;
	uint32_t mVertexCount;

	vector<shared_ptr<MeshPart> > mMeshParts;  
	vector<shared_ptr<Material> > mMaterials;

	Matrix4f mWorld;
};


} // Namespace RcEngine

#endif // Mesh_h__
