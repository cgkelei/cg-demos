#include <Graphics/Mesh.h>
#include <Graphics/Material.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/Animation.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/AnimationState.h>
#include <Graphics/Skeleton.h>
#include <Graphics/MeshPart.h>
#include <Core/Context.h>
#include <Core/Exception.h>
#include <Graphics/Animation.h>
#include <IO/Stream.h>
#include <IO/FileSystem.h>
#include <Math/MathUtil.h>
#include <Resource/ResourceManager.h>


namespace RcEngine {

Mesh::Mesh( uint32_t resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(ResourceTypes::Mesh, creator, handle, name, group), mSkeleton(nullptr)
{

}

Mesh::~Mesh()
{

}

void Mesh::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	shared_ptr<Stream> streamPtr = fileSystem.OpenStream(mName, mGroup);
	Stream& source = *streamPtr;

	// read mesh name
	String meshName = source.ReadString();

	// read bounding sphere
	Vector3f center;
	float radius;
	source.Read(&center, sizeof(Vector3f));
	radius = source.ReadFloat();
	mBoundingSphere = BoundingSpheref(center, radius);

	// read mesh part
	uint32_t subMeshCount = source.ReadUInt();
	mMeshParts.resize(subMeshCount);
	for (uint32_t i = 0 ; i < subMeshCount; ++i)
	{
		shared_ptr<MeshPart> subMesh = std::make_shared<MeshPart>(*this);
		subMesh->Load(source);

		mMeshParts[i] = subMesh;
	}
	
	// read bones
	mSkeleton = Skeleton::LoadFrom(source);
	
	// read animation clips
	uint32_t animClipCount = source.ReadUInt();
	mAninationClips.resize(animClipCount);
	for (uint32_t i = 0; i < animClipCount; i++)
	{
		mAninationClips[i] = source.ReadString();
	}
}

void Mesh::UnloadImpl()
{

}

shared_ptr<Resource> Mesh::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	assert(creator != nullptr);
	return std::make_shared<Mesh>(ResourceTypes::Mesh, creator, handle, name, group);
}






} // Namespace RcEngine