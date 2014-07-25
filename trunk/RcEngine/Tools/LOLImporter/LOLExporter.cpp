#include "LOLExporter.h"
#include <fstream>
#include <algorithm>
#include <IO/FileStream.h>
#include <Core/Loger.h>
#include <IO/PathUtil.h>
#include <Graphics/VertexDeclaration.h>
#include <Math/MathUtil.h>

using namespace std;
using namespace RcEngine;


void CorrectName(String& matName)
{
	std::replace(matName.begin(), matName.end(), ':', '_');
}

LOLExporter::LOLExporter(void)
	: mDummyRootAdded(false)
{
}

LOLExporter::~LOLExporter(void)
{
}

void LOLExporter::ImportSkeleton( const char* sklFilename )
{
	errno_t err;
	size_t numRead;

	FILE* pFile = NULL;
	err = fopen_s(&pFile, sklFilename, "rb");
	if (err != 0)
	{
		printf("ERROR: could not open %s\n", sklFilename);
		exit(1);
	}

	static const char SkeletonFileType[] = "r3d2sklt";

	Header header;
	fread_s(&header, sizeof(header), sizeof(header), 1, pFile);

	if (strncmp(header.fileType, SkeletonFileType, 8) == 0)
	{
		mBones.resize(header.numElements);
		for (int i = 0; i < header.numElements; i++) 
		{
			LOLBone& bone = mBones[i];
			fread_s(&bone, sizeof(bone), sizeof(bone), 1, pFile);

			/*memset(bone.name, 0, sizeof(bone.name));
		
			numRead = fread_s(&bone.name, sizeof(bone.name), sizeof(bone.name), 1, pFile);
			numRead = fread_s(&bone.parent, sizeof(bone.parent), sizeof(bone.parent), 1, pFile);
			numRead = fread_s(&bone.scale, sizeof(bone.scale), sizeof(bone.scale), 1, pFile);

			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 4; j++) {
				fin >> bone.matrix[i][j];
				}
			}
			fout.write((const char*)&bone, sizeof(Bone));*/
			//bone.scale = 1.0f;
			printf("Bone: %s, Parent: %s\n", bone.name, (bone.parent != -1 ) ? mBones[bone.parent].name : "");
		}

		int32_t id;
		while ( fread_s(&id, 4, 4, 1, pFile) )
		{
			printf("boneid: %d\n", id);
		}

		//int boneSize = sizeof(Bone);
		//long int currPos = ftell(pFile);
		//fseek(pFile, 0, SEEK_END);
		//long int left = ftell(pFile) - currPos;
	}
	else
	{
		printf("Unsupported skeleton format!\n");
	}
	

	// if exit multiple bone with no parent, add a dummy root node 
	uint32_t numRootBones = std::count_if(mBones.begin(), mBones.end(), [&](const LOLBone& bone) { return bone.parent == -1;} );
	if (numRootBones > 1)
	{
		printf("\nAdd dummy root node!\n\n");

		LOLBone dummyRoot;
		sprintf_s(dummyRoot.name, "dummyRoot");
		
		dummyRoot.parent = -1;

		dummyRoot.matrix[0][0] = 1.0f; dummyRoot.matrix[0][1] = 0.0f; dummyRoot.matrix[0][2] = 0.0f; dummyRoot.matrix[0][3] = 0.0f;
		dummyRoot.matrix[1][0] = 0.0f; dummyRoot.matrix[1][1] = 1.0f; dummyRoot.matrix[1][2] = 0.0f; dummyRoot.matrix[1][3] = 0.0f;
		dummyRoot.matrix[2][0] = 0.0f; dummyRoot.matrix[2][1] = 0.0f; dummyRoot.matrix[2][2] = 1.0f; dummyRoot.matrix[2][3] = 0.0f;
	
		dummyRoot.scale = mBones.front().scale;

		mBones.insert(mBones.begin(), dummyRoot);

		for (size_t i = 1; i < mBones.size(); ++i)
		{
			LOLBone& bone = mBones[i];
			mBones[i].parent += 1;
			printf("Bone: %s, Parent: %s\n", bone.name, (bone.parent != -1 ) ? mBones[bone.parent].name : "");
		}

		mDummyRootAdded = true;
	}

	printf("\nBuild Skeleton \n");
	mSkeleton = std::make_shared<Skeleton>();
	mSkeleton->AddBone(mBones[0].name, nullptr);
	for (size_t i = 1; i < mBones.size(); ++i)
	{
		Bone* parent = mSkeleton->GetBone(mBones[mBones[i].parent].name);
		Bone* newBone = mSkeleton->AddBone(mBones[i].name, parent);
		
		printf("Bone: %s, Parent: %s\n", newBone->GetName().c_str(), parent->GetName().c_str());
	}

	for (size_t i = 0; i < mSkeleton->GetNumBones(); ++i)
	{
		const LOLBone& bone = mBones[i];
		Bone* engineBone = mSkeleton->GetBone(i);

		float4x4 rotation(
			bone.matrix[0][0], bone.matrix[0][1], bone.matrix[0][2], 0.0f,
			bone.matrix[1][0], bone.matrix[1][1], bone.matrix[1][2], 0.0f,
			bone.matrix[2][0], bone.matrix[2][1], bone.matrix[2][2], 0.0f,
			0.0f,			   0.0f,			  0.0f,			     1.0f);

		rotation = rotation.Transpose();
		Quaternionf rotQuat = QuaternionFromRotationMatrix(rotation);

		//engineBone->SetScale(float3(bone.scale, bone.scale, bone.scale));
		engineBone->SetWorldPosition(float3(bone.matrix[0][3], bone.matrix[1][3], bone.matrix[2][3]));
		engineBone->SetWorldRotation(rotQuat);
	}

	mOutputPath = sklFilename;
	PathUtil::SplitPath(mOutputPath, mOutputPath, mMeshName, mAnimationName);
}

void LOLExporter::ImportMesh( const char* sknFilename )
{
	errno_t err;
	size_t numRead;

	FILE* pFile = NULL;
	err = fopen_s(&pFile, sknFilename, "rb");
	if (err != 0)
	{
		printf("ERROR: could not open %s\n", sknFilename);
		exit(1);
	}

	SkinModelHeader header;
	numRead = fread_s(&header, sizeof(SkinModelHeader), sizeof(SkinModelHeader), 1, pFile);

	mMaterials.resize(header.numMaterials);
	for (int i = 0; i < header.numMaterials; i++) 
	{
		SkinModelMaterial& mat = mMaterials[i];
		numRead = fread_s(&mat, sizeof(SkinModelMaterial), sizeof(SkinModelMaterial), 1, pFile);
	}

	numRead = fread_s(&mSkinModelData.numIndices, sizeof(int32_t), sizeof(int32_t), 1, pFile);
	numRead = fread_s(&mSkinModelData.numVertices, sizeof(int32_t), sizeof(int32_t), 1, pFile);
	
	/*long int pCurr = ftell(pFile);
	fseek(pFile, 0, SEEK_END);
	long int pEnd = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	fseek(pFile, pCurr, SEEK_SET);

	long int totalLeft = pEnd - pCurr;*/

	//uint32_t meshSize = sizeof(SkinModelVertex) * mSkinModelData.numVertices + sizeof(uint16_t) * mSkinModelData.numIndices;
	//long int remain = totalLeft - meshSize;

	mSkinModelData.indices.resize(mSkinModelData.numIndices);
	size_t indexBufferSize = mSkinModelData.numIndices * sizeof(uint16_t);
	numRead = fread_s(&mSkinModelData.indices[0], indexBufferSize, sizeof(uint16_t), mSkinModelData.numIndices, pFile);

	mSkinModelData.verteces.resize(mSkinModelData.numVertices);
	size_t vertexBufferSize = mSkinModelData.numVertices * sizeof(SkinModelVertex);
	numRead = fread_s(&mSkinModelData.verteces[0], vertexBufferSize, sizeof(SkinModelVertex), mSkinModelData.numVertices, pFile);

	// Check SkinModelVertex
	for (const SkinModelVertex& vertex : mSkinModelData.verteces)
	{
		float totalWeight = 0.0f;
		for (int i = 0; i < 4; ++i) 
		{
			if (vertex.boneIndex[i] >= mBones.size())
				printf("Bone Index Out of Range!");

			totalWeight += vertex.weights[i];
		}
		
		if ( fabsf(totalWeight - 1.0f) > 0.001)
			printf("Unnormalized Bone Weights!");

		if ( vertex.texcoords[0] < 0.0f || vertex.texcoords[0] > 1.0f ||
			 vertex.texcoords[1] < 0.0f || vertex.texcoords[1] > 1.0f )
			 printf("Texcoords Index Out of Range!");
	}

	mMeshPartBounds.resize(mMaterials.size());
	for ( size_t i = 0; i < mMaterials.size(); ++i )
	{
		BoundingBoxf meshPartBound = mMeshPartBounds[i];
		const SkinModelMaterial& material = mMaterials[i];
		for (int32_t j = material.startIndex; j < material.startIndex + material.numIndices; ++j)
		{
			uint16_t index = mSkinModelData.indices[j];
			const SkinModelVertex& vertex = mSkinModelData.verteces[index];
			meshPartBound.Merge(vertex.position);
		}
		
		mMeshBound.Merge(meshPartBound);
	}
}

void LOLExporter::ImportNew( const char* sknFilename )
{
	/*FILE* pFile = fopen(sknFilename, "rb");
	if (!pFile)
	{
		printf("ERROR: could not open %s\n", sknFilename);
		exit(1);
	}

	Header header;
	fread(&header, sizeof(Header), 1, pFile);

	char boneName[16];
	fseek(pFile, 0, SEEK_SET);
	fseek(pFile, 0x22FC, SEEK_CUR);
	
	size_t result, numBones = 0;
	do
	{
		memset(boneName, 0, sizeof(boneName));
		result = fread(boneName, 1, 8, pFile);
		
		if (result != 8)
			break;
		
		size_t allReadBits = 8;
		while (boneName[allReadBits-1] != '\0')
		{
			result = fread(boneName + allReadBits, 1, 4, pFile);
			allReadBits += 4;
		}
		
		printf("Bone %d: %s\n", numBones++, boneName);
	
	} while(true);*/

	//char boneName[8];
	//int boneParent;
	//float scale;
	//for (int i = 0; i < header.numElements; i++) {
	//	memset(boneName,0,32);
	//	fread(boneName, 32, 1, pFile);
	//	fread(&boneParent, sizeof(boneParent), 1, pFile);
	//	fread(&scale, sizeof(scale), 1, pFile);
	//	/*for (int i = 0; i < 3; i++) {
	//		for (int j = 0; j < 4; j++) {
	//			fin >> bone.matrix[i][j];
	//		}
	//	}*/
	//	//fout.write((const char*)&bone, sizeof(Bone));
	//}
}

void LOLExporter::ExportObj( const char* objFilename )
{
	//exports this skin model as a .obj file
	ofstream fout(objFilename);
	for (int i = 0; i < mSkinModelData.verteces.size(); i++) {
		fout << "v " << mSkinModelData.verteces[i].position[0] << " " << mSkinModelData.verteces[i].position[1] << " " << mSkinModelData.verteces[i].position[2] << endl;
		fout << "vn " << mSkinModelData.verteces[i].normal[0] << " " << mSkinModelData.verteces[i].normal[1] << " " << mSkinModelData.verteces[i].normal[2] << endl;
		fout << "vt " << mSkinModelData.verteces[i].texcoords[0] << " " << 1-mSkinModelData.verteces[i].texcoords[1] << endl;
	}
	if (mMaterials.size()) {
		fout << "g mat_" << mMaterials[0].name << std::endl;
	}
	for (int i = 0;  i < mSkinModelData.numIndices/3; i++) {
		int a = mSkinModelData.indices[i*3] + 1;
		int b = mSkinModelData.indices[i*3 + 1] + 1;
		int c = mSkinModelData.indices[i*3 + 2] + 1; 
		fout << "f " << a << '/' << a << '/' << a << " " << b << '/' << b << '/' << b << " " << c << '/' << c << '/' << c << endl;
	}
}

void LOLExporter::ImportAnimation( const char* animFilename )
{
	errno_t err;
	size_t numRead;

	FILE* pFile = NULL;
	err = fopen_s(&pFile, animFilename, "rb");
	if (err != 0)
	{
		printf("ERROR: could not open %s\n", animFilename);
		exit(1);
	}

	static const char AninationFileType[] = "r3d2anmd";

	fread_s(&mAnimationHeader, sizeof(SkinAnimationHeader), sizeof(SkinAnimationHeader), 1, pFile);

	if (strncmp(mAnimationHeader.fileType, AninationFileType, 8) == 0)
	{
		mAnimationClipData.mAnimationTracks.resize(mAnimationHeader.numBones);
		for (int32_t i = 0; i < mAnimationHeader.numBones; ++i)
		{
			AnimationClipData::AnimationTrack& anmTrack = mAnimationClipData.mAnimationTracks[i];

			/**
			 *  name:           str         name of bone
			 *  boneType:       int         type of bone (2 = root, 0 = ordinary, )
			 *	quat:           float[4]    quaternion of bone
			 *	pos:            float[3]    postion of bone
			 */

			fread_s(&anmTrack.BoneName, 32, 32, 1, pFile);
			fread_s(&anmTrack.BoneType, 4, 4, 1, pFile);

			anmTrack.KeyFrames.resize(mAnimationHeader.numFrames);
			for (int32_t i = 0; i < mAnimationHeader.numFrames; ++i)
			{
				fread_s(&anmTrack.KeyFrames[i], sizeof(AnimationClipData::KeyFrame), sizeof(AnimationClipData::KeyFrame), 1, pFile);
			}
		}
	}
	else
	{
		printf("Unsupported skeleton format!\n");
	}
	
	fclose(pFile);

	String dummy;
	PathUtil::SplitPath(animFilename, dummy, mAnimationName, dummy);
}

void LOLExporter::BuildAndSaveBinary()
{
	const uint32_t MeshId = ('M' << 24) | ('E' << 16) | ('S' << 8) | ('H');

	FileStream stream;
	stream.Open(mOutputPath + mMeshName + ".mesh", FILE_WRITE);

	String objFile = mOutputPath + mMeshName + ".obj";
	ExportObj(objFile.c_str());

	// Write mesh id
	stream.WriteUInt(MeshId);

	// write mesh name
	stream.WriteString(mMeshName);

	// write mesh bounding box
	stream.Write(&mMeshBound.Min, sizeof(float3));
	stream.Write(&mMeshBound.Max, sizeof(float3));

	// write mesh part count
	stream.WriteUInt( mMaterials.size() );
	stream.WriteUInt( mBones.size() );
	stream.WriteUInt( 1 );
	stream.WriteUInt( 1 );

	// Write mesh part 
	for (size_t mpi = 0; mpi < mMaterials.size(); ++mpi)
	{
		const SkinModelMaterial& meshPart = mMaterials[mpi];

		// write sub mesh name
		stream.WriteString("Dummy");	

		String MaterialName = meshPart.name;
		CorrectName(MaterialName);

		// write material name
		stream.WriteString(MaterialName + ".material.xml");
		std::cout << "MaterialName: " << MaterialName << std::endl;

		// write sub mesh bounding sphere
		stream.Write(&mMeshPartBounds[mpi].Min, sizeof(float3));
		stream.Write(&mMeshPartBounds[mpi].Max, sizeof(float3));

		stream.WriteUInt(0);
		stream.WriteUInt(0);

		// write vertex count and vertex size
		stream.WriteUInt(meshPart.startIndex);
		stream.WriteUInt(meshPart.numIndices);
		stream.WriteInt(0);
	}

	// Write skeleton
	for (size_t iBone = 0; iBone < mBones.size(); ++iBone)
	{
		Bone* bone = mSkeleton->GetBone(iBone);
		Bone* parentBone = static_cast<Bone*>(bone->GetParent());

		float3 pos = bone->GetPosition();
		float3 scale = bone->GetScale();
		Quaternionf rot = bone->GetRotation();

		stream.WriteString(bone->GetName());
		stream.WriteInt(parentBone ? parentBone->GetBoneIndex() : -1);

		stream.Write(&pos, sizeof(float3));
		stream.Write(&rot, sizeof(Quaternionf));
		stream.Write(&scale, sizeof(float3));
	}

	uint32_t offset = 0;
	std::vector<VertexElement> vertexElements;
	vertexElements.push_back( VertexElement(offset, VEF_Float3, VEU_Position, 0) ); offset += 12;
	vertexElements.push_back( VertexElement(offset, VEF_Float4, VEU_BlendWeight, 0) );  offset += 16;
	vertexElements.push_back( VertexElement(offset, VEF_UInt4, VEU_BlendIndices, 0) );  offset += 16;
	vertexElements.push_back( VertexElement(offset, VEF_Float3, VEU_Normal, 0) );  offset += 12;
	vertexElements.push_back( VertexElement(offset, VEF_Float2, VEU_TextureCoordinate, 0) );  offset += 8;


	// Write vertex and index buffer
	stream.WriteUInt(mSkinModelData.numVertices); // Vertex Count
	stream.WriteUInt(vertexElements.size());   // Vertex Size
	for (const VertexElement& ve : vertexElements)
	{
		stream.WriteUInt(ve.Offset);
		stream.WriteUInt(ve.Type);
		stream.WriteUInt(ve.Usage);
		stream.WriteUShort(ve.UsageIndex);
	}

	int dummyRootNode = mDummyRootAdded ? 1 : 0;
	for (const SkinModelVertex& vertex : mSkinModelData.verteces)
	{
		stream.Write(&vertex.position, sizeof(vertex.position));
		stream.Write(&vertex.weights, sizeof(vertex.weights));
		stream.WriteUInt(vertex.boneIndex[0] + dummyRootNode);
		stream.WriteUInt(vertex.boneIndex[1] + dummyRootNode);
		stream.WriteUInt(vertex.boneIndex[2] + dummyRootNode);
		stream.WriteUInt(vertex.boneIndex[3] + dummyRootNode);
		stream.Write(&vertex.normal, sizeof(vertex.normal));
		stream.Write(&vertex.texcoords, sizeof(vertex.texcoords));
	}

	stream.WriteUInt(mSkinModelData.numIndices);
	stream.WriteUInt(IBT_Bit16);
	stream.Write(&mSkinModelData.indices[0], mSkinModelData.indices.size() * sizeof(uint16_t));
	//if (0/*SwapWindOrder*/)
	//{
	//	for (size_t j = 0; j < mSkinModelData.indices.size() / 3; ++j)
	//	{
	//		stream.WriteUShort(mSkinModelData.indices[3*j+0]);
	//		stream.WriteUShort(mSkinModelData.indices[3*j+2]);
	//		stream.WriteUShort(mSkinModelData.indices[3*j+1]);
	//	}
	//}
	//else
	//{
	//	for (size_t j = 0; j < mSkinModelData.indices.size(); ++j)
	//		stream.WriteUShort(mSkinModelData.indices[j]);
	//}

	stream.Close();

	String clipName = mAnimationName  + ".anim";

	FileStream clipStream;
	clipStream.Open(mOutputPath + clipName, FILE_WRITE);

	double frameRate = 1.0 / mAnimationHeader.fps;
	double Duration = frameRate * mAnimationClipData.mAnimationTracks.size();
	clipStream.WriteFloat((float)Duration);
	clipStream.WriteUInt(mAnimationClipData.mAnimationTracks.size());

	for (const AnimationClipData::AnimationTrack& track : mAnimationClipData.mAnimationTracks)
	{
		// write track name
		String trackName = track.BoneName;
		clipStream.WriteString(trackName);
		// write track key frame count
		clipStream.WriteUInt(track.KeyFrames.size());

		double keyTime = 0.0f;
		const float3 scale(1, 1, 1);
		for (const AnimationClipData::KeyFrame& key : track.KeyFrames)
		{
			// write key time
			clipStream.WriteFloat((float)keyTime);
			clipStream.Write(&key.Translation, sizeof(float3));
			clipStream.Write(&key.Rotation, sizeof(Quaternionf));
			clipStream.Write(&scale, sizeof(float3));

			keyTime += frameRate;
		}
	}
	
	clipStream.Close();
}

int main()
{
	LOLExporter exporter;

	exporter.ImportSkeleton("blitzcrank_boxer.skl");
	exporter.ImportMesh("blitzcrank_boxer.skn");
	exporter.ImportAnimation("blitzcrank_boxer_dance.anm");
	exporter.BuildAndSaveBinary();

	return 1;
}
