#pragma once
#ifndef LOLExporter_h__
#define LOLExporter_h__

#include <vector>
#include <cstdint>

//Structure for header on skl files
struct Header 
{
	char fileType[8];
	int32_t numObjects;
	int32_t skeletonHash;
	int32_t numElements;
};

//Structure for a bone in skl files
struct Bone 
{
	char name[32];
	int32_t parent;
	float scale;
	float matrix[3][4];
};

//Structure for the Header in skn files
struct SkinModelHeader 
{
	int32_t magic;
	int32_t numObjects;
	int32_t numMaterials;
};

//Structure for a material block in skn files
struct SkinModelMaterial
{
	char name[64];
	int32_t startVertex;
	int32_t numVertices;
	int32_t startIndex;
	int32_t numIndices;
};

//Vertex block in skn files
struct SkinModelVertex {
	float position[3];
	uint8_t boneIndex[4];
	float weights[4];
	float normal[3];
	float texcoords[2];
};

struct SkinModelData
{
	int32_t numIndices;
	int32_t numVertices;
	std::vector<uint16_t> indices;
	std::vector<SkinModelVertex> verteces;
};

class LOLExporter
{
public:
	LOLExporter(void);
	~LOLExporter(void);

	void ImportNew(const char* sknFilename);

	void ImportSkeleton(const char* sklFilename);
	void ImportMesh(const char* sknFilename);
	void ImportAnimation(const char* animFilename);

	void BuildAndSaveBinary();

	void ExportObj(const char* objFilename);

private:

	SkinModelData mSkinModelData;
	std::vector<SkinModelMaterial> mMaterials;
	std::vector<Bone> mBones;
};

#endif // LOLExporter_h__
