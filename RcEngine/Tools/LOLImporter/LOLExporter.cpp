#include "LOLExporter.h"
#include <fstream>
#include <algorithm>
#include <IO/FileStream.h>
#include <Core/Loger.h>

using namespace std;
using namespace RcEngine;

LOLExporter::LOLExporter(void)
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
			Bone& bone = mBones[i];
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

			printf("Bone: %s, Parent: %s\n", bone.name, (bone.parent != -1 ) ? mBones[bone.parent].name : "");
		}

		/*int boneSize = sizeof(Bone);
		long int currPos = ftell(pFile);
		fseek(pFile, 0, SEEK_END);
		long int left = ftell(pFile) - currPos;*/
	}
	else
	{
		printf("Unsupported skeleton format!\n");
	}
	
	fclose(pFile);
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

	SkinAnimationHeader header;
	fread_s(&header, sizeof(SkinAnimationHeader), sizeof(SkinAnimationHeader), 1, pFile);

	if (strncmp(header.fileType, AninationFileType, 8) == 0)
	{
		mAnimationClipData.mAnimationTracks.resize(header.numBones);
		for (int32_t i = 0; i < header.numBones; ++i)
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

			anmTrack.KeyFrames.resize(header.numFrames);
			for (int32_t i = 0; i < header.numFrames; ++i)
			{
				fread_s(&anmTrack.KeyFrames[0], sizeof(AnimationClipData::KeyFrame), sizeof(AnimationClipData::KeyFrame), 1, pFile);
			}
		}
	}
	else
	{
		printf("Unsupported skeleton format!\n");
	}
	
	fclose(pFile);
}

void LOLExporter::BuildAndSaveBinary()
{

}

int main()
{
	LOLExporter exporter;

	exporter.ImportSkeleton("blitzcrank.skl");
	exporter.ImportMesh("blitzcrank.skn");
	exporter.ImportAnimation("blitzcrank_dance.anm");
	exporter.ExportObj("blitzcrank_i.obj");

	return 1;
}
