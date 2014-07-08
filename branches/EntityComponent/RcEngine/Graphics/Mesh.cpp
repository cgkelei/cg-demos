#include <Graphics/Mesh.h>
#include <Graphics/Material.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/Animation.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/AnimationState.h>
#include <Graphics/Skeleton.h>
#include <Graphics/MeshPart.h>
#include <Core/Environment.h>
#include <Core/Exception.h>
#include <Graphics/Animation.h>
#include <IO/Stream.h>
#include <IO/FileSystem.h>
#include <IO/PathUtil.h>
#include <Math/MathUtil.h>
#include <Resource/ResourceManager.h>


namespace RcEngine {

Mesh::Mesh(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Mesh, creator, handle, name, group), mSkeleton(nullptr)
{
	printf("Create Mesh: %s\n", mResourceName.c_str());
}

Mesh::~Mesh()
{
	printf("Delete Mesh: %s\n", mResourceName.c_str());
}

void Mesh::LoadImpl()
{
	ResourceManager& resMan = ResourceManager::GetSingleton();

	FileSystem& fileSystem = FileSystem::GetSingleton();

	String currMeshDirectory = PathUtil::GetParentPath(mResourceName);

	shared_ptr<Stream> streamPtr = fileSystem.OpenStream(mResourceName, mGroup);
	Stream& source = *streamPtr;

	const uint32_t MeshId = ('M' << 24) | ('E' << 16) | ('S' << 8) | ('H');

	uint32_t header = source.ReadUInt();
	assert(header == MeshId);

	// read mesh name
	String meshName = source.ReadString();

	// read bounding box
	float3 min, max;
	source.Read(&min, sizeof(float3));
	source.Read(&max, sizeof(float3));
	mBoundingBox = BoundingBoxf(min, max);

	// read mesh part
	//uint32_t subMeshCount = source.ReadUInt();
	//mMeshParts.resize(subMeshCount);
	//for (uint32_t i = 0 ; i < subMeshCount; ++i)
	//{
	//	shared_ptr<MeshPart> subMesh = std::make_shared<MeshPart>(*this);
	//	subMesh->Load(source);

	//	// add mesh part material resource
	//	ResourceManager::GetSingleton().AddResource(RT_Material, subMesh->mMaterialName, mGroup);

	//	mMeshParts[i] = subMesh;
	//}

	uint32_t subMeshCount = source.ReadUInt();
	for (uint32_t i = 0 ; i < subMeshCount; ++i)
	{
		shared_ptr<MeshPart> subMesh = std::make_shared<MeshPart>(*this);
		subMesh->Load(source);

		String matPath;

		if (currMeshDirectory.empty())
			matPath = subMesh->mMaterialName;
		else 
			matPath = currMeshDirectory + "/" + subMesh->mMaterialName;

		// Hack: if material doesn't exit, not add it
		if (fileSystem.Exits(matPath, mGroup) == false)
			continue;

		// add mesh part material resource
		ResourceManager::GetSingleton().AddResource(RT_Material, matPath, mGroup);
		mMeshParts.push_back(subMesh);
	}
	
	// read bones
	mSkeleton = Skeleton::LoadFrom(source);
}

void Mesh::UnloadImpl()
{

}

shared_ptr<Resource> Mesh::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	assert(creator != nullptr);
	return std::make_shared<Mesh>(creator, handle, name, group);
}

shared_ptr<Resource> Mesh::Clone()
{
	shared_ptr<Mesh> retVal(new Mesh(mCreator, mResourceHandle, mResourceName, mGroup));

	retVal->mBoundingBox = mBoundingBox;
	retVal->mPrimitiveCount = mPrimitiveCount;
	retVal->mBoundingBox = mBoundingBox;
	retVal->mMeshParts = mMeshParts;
	mSkeleton = mSkeleton->Clone();

	return retVal;
}






} // Namespace RcEngine