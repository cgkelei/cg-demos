#ifndef Visual_h__
#define Visual_h__

#include <Core/Prerequisites.h>
#include <Scene/SceneObject.h>
#include <Graphics/Renderable.h>
#include <GamePlay/Component.h>
#include <Math/BoundingBox.h>
#include <Math/Quaternion.h>

namespace RcEngine {

class VisualPart;
class Skeleton;
class BoneSceneNode;

class _ApiExport Visual : public Component, public SceneObject
{
	friend class VisualPart;

public:
	Visual(const String& name, const shared_ptr<Mesh>& mesh);
	~Visual();

	inline uint32_t GetNumVisualParts() const				{ return mVisualParts.size(); }
	inline VisualPart* GetVisualPart(uint32_t index) const	{ return mVisualParts[index]; }
	
	inline bool HasSkeleton() const							{ return mSkeleton != nullptr; }
	inline shared_ptr<Skeleton> GetSkeleton() const			{ return mSkeleton; }

	// Inherit from component
	const String& GetComponentName() const = 0;
	void DestroyComponent();

	// Inherit from SceneObject
	const BoundingBoxf& GetWorldBoundingBox() const;

	void OnUpdateRenderQueue(RenderQueue* renderQueue, const Camera& cam, RenderOrder order);

	// Create a SceneNode take bone as parent
	BoneSceneNode* CreateBoneSceneNode(const String& nodeName, const String& boneName);

protected:
	void UpdateAnimation();

public:
	static const String ComponentName;

protected:
	shared_ptr<Mesh> mMesh;
	shared_ptr<Skeleton> mSkeleton;

	std::vector<VisualPart*> mVisualParts;

	mutable BoundingBoxf mWorldBoundingBox;

	// Matrix palette for skeleton animation
	uint32_t mNumSkinMatrices;
	float4x4* mMatrixPalette;
};

class _ApiExport VisualPart : public Renderable
{
public:
	VisualPart(Visual& parent, const shared_ptr<MeshPart>& meshPart);
	~VisualPart() {}

	const shared_ptr<Material>& GetMaterial() const { return mMaterial; }
	
	// Get local bounding box of mesh part
	const BoundingBoxf& GetBoundingBox() const;

	void SetMaterial( const shared_ptr<Material>& mat );

	const shared_ptr<RenderOperation>& GetRenderOperation() const;
	void GetWorldTransforms(float4x4* xWorld, uint32_t& numPalette, float4x4*& xPalette);

protected:
	Visual& mParent;
	shared_ptr<MeshPart> mMeshPart;
	shared_ptr<RenderOperation> mRenderOperation;
	shared_ptr<Material> mMaterial;
};


}


#endif // Visual_h__
