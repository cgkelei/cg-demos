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
  in mesh file. MeshPart can load material by material resource manager if needed. We do that
  because all the same mesh will load from disk only once, multiple Entity can shared same mesh,
  so material reference stores in entity, and different entity with same mesh model can have different
  material.
*/


class _ApiExport Mesh : public Resource
{
public:
	Mesh(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group );
	virtual ~Mesh();

	const String& GetName() const								{ return mResourceName; }
	const BoundingBoxf& GetBoundingBox() const			    { return mBoundingBox; }


	uint32_t GetNumMeshPart() const								{ return mMeshParts.size(); }
	const shared_ptr<MeshPart>& GetMeshPart(size_t i) const		{ return mMeshParts[i]; }
	
	shared_ptr<Skeleton> GetSkeleton() const					{ return mSkeleton; }

	bool HasAnimation() const									{ return !mAninationClips.empty(); }
	const vector<String>& GetAnimations() const					{ return mAninationClips; }

	uint32_t GetPrimitiveCount() const							{ return mPrimitiveCount; }
	uint32_t GetVertexCount() const								{ return mVertexCount; }

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

	vector<String> mAninationClips;

};


} // Namespace RcEngine

#endif // Mesh_h__
