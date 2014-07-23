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

	Header header;
	numRead = fread_s(&header, sizeof(header), sizeof(header), 1, pFile);

	mBones.resize(header.numElements);
	for (int i = 0; i < header.numElements; i++) 
	{
		Bone& bone = mBones[i];
		numRead = fread_s(&bone, sizeof(bone), sizeof(bone), 1, pFile);

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

}

void LOLExporter::BuildAndSaveBinary()
{
	const uint32_t MeshId = ('M' << 24) | ('E' << 16) | ('S' << 8) | ('H');

	String outFile;

	FileStream stream;
	stream.Open(outFile, FILE_WRITE);
	//ExportLog::LogMsg(0, "Build mesh: %s\n", mesh.Name.c_str());

	
	{
		// Write mesh id
		stream.WriteUInt(MeshId);

		// write mesh name
		//stream.WriteString(mesh.Name);

		// write mesh bounding box
		//stream.Write(&mesh.Bound.Min, sizeof(float3));
		//stream.Write(&mesh.Bound.Max, sizeof(float3));

		// write mesh part count
		stream.WriteUInt(mMaterials.size());

		for (uint32_t i = 0; i < mMaterials.size(); ++i)
		{
			MeshPartData& meshPart = *(mesh.MeshParts[mpi]);
			shared_ptr<VertexDeclaration> vertexDecl = GetVertexDeclaration(meshPart.Vertices[0].Flag);
			uint32_t vertexSize = vertexDecl->GetVertexSize();

			// write sub mesh name
			stream.WriteString(meshPart.Name);	

			// write material name
			stream.WriteString(meshPart.MaterialName + ".material.xml");

			// write sub mesh bounding sphere
			stream.Write(&meshPart.Bound.Min, sizeof(float3));
			stream.Write(&meshPart.Bound.Max, sizeof(float3));

			// write vertex count and vertex size
			stream.WriteUInt(meshPart.Vertices.size());
			stream.WriteUInt(vertexSize);

			const std::vector<VertexElement>& elements = vertexDecl->GetVertexElements();

			// write vertex declaration, elements count
			stream.WriteUInt(elements.size());

			for (auto iter = elements.begin(); iter != elements.end(); ++iter)
			{
				const VertexElement& ve = *iter;
				stream.WriteUInt(ve.Offset);
				stream.WriteUInt(ve.Type);
				stream.WriteUInt(ve.Usage);
				stream.WriteUShort(ve.UsageIndex);
			}

			uint32_t bufferSize = meshPart.Vertices.size() * vertexSize;
			/*	stream.WriteUInt(bufferSize);*/

			size_t checkSize = 0;
			for (size_t vi = 0; vi < meshPart.Vertices.size(); ++vi)
			{
				Vertex& vertex = meshPart.Vertices[vi];
				uint32_t vertexFlag = vertex.Flag;

				if (vertexFlag & Vertex::ePosition)
				{
					stream.Write(&vertex.Position, sizeof(float3));
					checkSize += sizeof(float3);
				}

				if (vertexFlag & Vertex::eBlendWeight)
				{
					assert(vertex.BlendWeights.size() == 4);
					stream.Write(&vertex.BlendWeights[0], sizeof(float) * 4);
					checkSize += sizeof(float) * 4;
				}

				if (vertexFlag & Vertex::eBlendIndices)
				{
					assert(vertex.BlendIndices.size() == 4);
					stream.WriteUInt(vertex.BlendIndices[0]);
					stream.WriteUInt(vertex.BlendIndices[1]);
					stream.WriteUInt(vertex.BlendIndices[2]);
					stream.WriteUInt(vertex.BlendIndices[3]);
					checkSize += sizeof(uint32_t) * 4;
				}

				if (vertexFlag & Vertex::eNormal)
				{
					stream.Write(&vertex.Normal, sizeof(float3));
					checkSize += sizeof(float3);
				}

				if (vertexFlag & Vertex::eTexcoord0)
				{
					stream.Write(&vertex.Tex0, sizeof(float2));
					checkSize += sizeof(float2);
				}

				if (vertexFlag & Vertex::eTexcoord1)
				{
					stream.Write(&vertex.Tex1, sizeof(float2));
					checkSize += sizeof(float2);
				}

				if (vertexFlag & Vertex::eTangent)
				{
					stream.Write(&vertex.Tangent, sizeof(float3));
					checkSize += sizeof(float3);
				}

				if (vertexFlag & Vertex::eBinormal)
				{
					stream.Write(&vertex.Binormal, sizeof(float3));
					checkSize += sizeof(float3);
				}				
			}
			assert(checkSize == bufferSize);

			// write indices count
			stream.WriteUInt(meshPart.Indices.size());

			if (meshPart.Indices.size() < (std::numeric_limits<uint32_t>::max)())
			{
				stream.WriteUInt(IBT_Bit16);
				if (g_ExportSettings.SwapWindOrder)
				{
					for (size_t i = 0; i < meshPart.Indices.size() / 3; ++i)
					{
						stream.WriteUShort(meshPart.Indices[3*i+0]);
						stream.WriteUShort(meshPart.Indices[3*i+2]);
						stream.WriteUShort(meshPart.Indices[3*i+1]);
					}
				}
				else
				{
					for (size_t i = 0; i < meshPart.Indices.size(); ++i)
						stream.WriteUShort(meshPart.Indices[i]);
				}
			}
			else
			{
				stream.WriteUInt(IBT_Bit32);
				if (g_ExportSettings.SwapWindOrder)
				{
					for (size_t i = 0; i < meshPart.Indices.size() / 3; ++i)
					{
						stream.WriteUInt(meshPart.Indices[3*i+0]);
						stream.WriteUInt(meshPart.Indices[3*i+2]);
						stream.WriteUInt(meshPart.Indices[3*i+1]);
					}
				}
				else
				{
					stream.Write(&(meshPart.Indices[0]), sizeof(char) * meshPart.Indices.size());
				}
			}
		}
	}

	for (size_t mi = 0; mi < mSceneMeshes.size(); ++mi)
	{
		MeshData& mesh  = *(mSceneMeshes[mi]);

		

		// Write mesh id
		stream.WriteUInt(MeshId);

		// write mesh name
		stream.WriteString(mesh.Name);

		// write mesh bounding box
		stream.Write(&mesh.Bound.Min, sizeof(float3));
		stream.Write(&mesh.Bound.Max, sizeof(float3));

		// write mesh part count
		stream.WriteUInt(mesh.MeshParts.size());

		for (size_t mpi = 0; mpi < mesh.MeshParts.size(); ++mpi)
		{
			MeshPartData& meshPart = *(mesh.MeshParts[mpi]);
			shared_ptr<VertexDeclaration> vertexDecl = GetVertexDeclaration(meshPart.Vertices[0].Flag);
			uint32_t vertexSize = vertexDecl->GetVertexSize();

			// write sub mesh name
			stream.WriteString(meshPart.Name);	

			// write material name
			stream.WriteString(meshPart.MaterialName + ".material.xml");

			// write sub mesh bounding sphere
			stream.Write(&meshPart.Bound.Min, sizeof(float3));
			stream.Write(&meshPart.Bound.Max, sizeof(float3));

			// write vertex count and vertex size
			stream.WriteUInt(meshPart.Vertices.size());
			stream.WriteUInt(vertexSize);

			const std::vector<VertexElement>& elements = vertexDecl->GetVertexElements();

			// write vertex declaration, elements count
			stream.WriteUInt(elements.size());

			for (auto iter = elements.begin(); iter != elements.end(); ++iter)
			{
				const VertexElement& ve = *iter;
				stream.WriteUInt(ve.Offset);
				stream.WriteUInt(ve.Type);
				stream.WriteUInt(ve.Usage);
				stream.WriteUShort(ve.UsageIndex);
			}

			uint32_t bufferSize = meshPart.Vertices.size() * vertexSize;
			/*	stream.WriteUInt(bufferSize);*/

			size_t checkSize = 0;
			for (size_t vi = 0; vi < meshPart.Vertices.size(); ++vi)
			{
				Vertex& vertex = meshPart.Vertices[vi];
				uint32_t vertexFlag = vertex.Flag;

				if (vertexFlag & Vertex::ePosition)
				{
					stream.Write(&vertex.Position, sizeof(float3));
					checkSize += sizeof(float3);
				}

				if (vertexFlag & Vertex::eBlendWeight)
				{
					assert(vertex.BlendWeights.size() == 4);
					stream.Write(&vertex.BlendWeights[0], sizeof(float) * 4);
					checkSize += sizeof(float) * 4;
				}

				if (vertexFlag & Vertex::eBlendIndices)
				{
					assert(vertex.BlendIndices.size() == 4);
					stream.WriteUInt(vertex.BlendIndices[0]);
					stream.WriteUInt(vertex.BlendIndices[1]);
					stream.WriteUInt(vertex.BlendIndices[2]);
					stream.WriteUInt(vertex.BlendIndices[3]);
					checkSize += sizeof(uint32_t) * 4;
				}

				if (vertexFlag & Vertex::eNormal)
				{
					stream.Write(&vertex.Normal, sizeof(float3));
					checkSize += sizeof(float3);
				}

				if (vertexFlag & Vertex::eTexcoord0)
				{
					stream.Write(&vertex.Tex0, sizeof(float2));
					checkSize += sizeof(float2);
				}

				if (vertexFlag & Vertex::eTexcoord1)
				{
					stream.Write(&vertex.Tex1, sizeof(float2));
					checkSize += sizeof(float2);
				}

				if (vertexFlag & Vertex::eTangent)
				{
					stream.Write(&vertex.Tangent, sizeof(float3));
					checkSize += sizeof(float3);
				}

				if (vertexFlag & Vertex::eBinormal)
				{
					stream.Write(&vertex.Binormal, sizeof(float3));
					checkSize += sizeof(float3);
				}				
			}
			assert(checkSize == bufferSize);

			// write indices count
			stream.WriteUInt(meshPart.Indices.size());

			if (meshPart.Indices.size() < (std::numeric_limits<uint32_t>::max)())
			{
				stream.WriteUInt(IBT_Bit16);
				if (g_ExportSettings.SwapWindOrder)
				{
					for (size_t i = 0; i < meshPart.Indices.size() / 3; ++i)
					{
						stream.WriteUShort(meshPart.Indices[3*i+0]);
						stream.WriteUShort(meshPart.Indices[3*i+2]);
						stream.WriteUShort(meshPart.Indices[3*i+1]);
					}
				}
				else
				{
					for (size_t i = 0; i < meshPart.Indices.size(); ++i)
						stream.WriteUShort(meshPart.Indices[i]);
				}
			}
			else
			{
				stream.WriteUInt(IBT_Bit32);
				if (g_ExportSettings.SwapWindOrder)
				{
					for (size_t i = 0; i < meshPart.Indices.size() / 3; ++i)
					{
						stream.WriteUInt(meshPart.Indices[3*i+0]);
						stream.WriteUInt(meshPart.Indices[3*i+2]);
						stream.WriteUInt(meshPart.Indices[3*i+1]);
					}
				}
				else
				{
					stream.Write(&(meshPart.Indices[0]), sizeof(char) * meshPart.Indices.size());
				}
			}
		}

		if (g_ExportSettings.ExportSkeleton && mesh.MeshSkeleton && mesh.MeshSkeleton->GetNumBones())
		{
			// write bone count
			auto& bones = mesh.MeshSkeleton->GetBones();
			stream.WriteUInt(bones.size());
			ExportLog::LogMsg(0, "mesh: %d\n has %d bones", mesh.Name.c_str(), bones.size());
			for (size_t iBone = 0; iBone < bones.size(); ++iBone)
			{
				Bone* bone = bones[iBone];
				Bone* parentBone = static_cast<Bone*>(bones[iBone]->GetParent());

				String parentName = "";
				if (parentBone)
					parentName = parentBone->GetName();

				float3 pos = bone->GetPosition();
				float3 scale = bone->GetScale();
				Quaternionf rot = bone->GetRotation();

				stream.WriteString(bone->GetName());
				stream.WriteString(parentName);

				stream.Write(&pos, sizeof(float3));
				stream.Write(&rot, sizeof(Quaternionf));
				stream.Write(&scale, sizeof(float3));
			}

			// write animation clips
			//const String& boneRootName = mesh.MeshSkeleton->GetRootBone()->GetName();
			//if (g_ExportSettings.ExportAnimation && mAnimations.count(boneRootName))
			//{
			//	AnimationData& animationData = mAnimations[boneRootName];

			//	stream.WriteUInt(animationData.AnimationClips.size());

			//	// animtion clip are write to each animation file
			//	for (const auto& kv : animationData.AnimationClips)
			//	{
			//		String clipName = kv.first + ".anim";
			//		const AnimationClipData& clip = kv.second;

			//		// write clip file in mesh 
			//		stream.WriteString(clipName);

			//		FileStream clipStream;
			//		clipStream.Open(mOutputPath + clipName, FILE_WRITE);

			//		clipStream.WriteString(kv.first);
			//		clipStream.WriteFloat(clip.Duration);
			//		clipStream.WriteUInt(clip.mAnimationTracks.size());

			//		for (const AnimationClipData::AnimationTrack& track : clip.mAnimationTracks)
			//		{
			//			// write track name
			//			clipStream.WriteString(track.Name);
			//			// write track key frame count
			//			clipStream.WriteUInt(track.KeyFrames.size());

			//			for (const AnimationClipData::KeyFrame& key : track.KeyFrames)
			//			{
			//				// write key time
			//				clipStream.WriteFloat(key.Time);
			//				clipStream.Write(&key.Translation, sizeof(float3));
			//				clipStream.Write(&key.Rotation, sizeof(Quaternionf));
			//				clipStream.Write(&key.Scale, sizeof(float3));
			//			}
			//		}

			//		clipStream.Close();
			//	}
			//}
			//else
			//{
			//	stream.WriteUInt(0);  // No Animations
			//}
		}
		else
		{ 
			stream.WriteUInt(0);	// No Skeleton
		}

		stream.Close();
	}
}

int main()
{
	LOLExporter exporter;

	exporter.ImportSkeleton("Lulu_darkcandy.skl");
	exporter.ImportMesh("Lulu_darkcandy.skn");
	exporter.ExportObj("Lulu_darkcandy.obj");

	return 1;
}
