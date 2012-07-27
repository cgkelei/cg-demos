#pragma once

#include <iostream>
#include <fstream>
#include <set>
#include <unordered_map>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Core/Prerequisites.h"
#include "Graphics/GraphicsCommon.h"
#include "Math/ColorRGBA.h"
#include "Math/MathUtil.h"

#pragma comment(lib, "assimp.lib")

using namespace std;
using namespace RcEngine;
using namespace RcEngine::Math;
using namespace RcEngine::Render;



class Bone {
public:

	std::string Name;
	Matrix4f Offset, LocalTransform, GlobalTransform, OriginalLocalTransform;
	Bone* Parent;
	vector<Bone*> Children;

	Bone() :Parent(0){}
	~Bone(){ for(size_t i(0); i< Children.size(); i++) delete Children[i]; }
};

struct MaterialData;
struct MeshPartData;
struct OutModel;

class AssimpProcesser
{
public:
	AssimpProcesser(void);
	~AssimpProcesser(void);

	bool Process(const char* filePath);

private:
	void ProcessScene(const aiScene* scene);

	shared_ptr<MaterialData> ProcessMaterial(aiMaterial* material);
	shared_ptr<MeshPartData> ProcessMeshPart(aiMesh* mesh);

	void ExportXML(const String& fileName);

	void GetBoundingBox(const aiScene* scene, aiVector3D* min, aiVector3D* max);

private:
	Bone* CreateBoneTrees(aiNode* pNode, Bone* parent);
	void CalculateBoneToWorldTransform(Bone* pNode);

	void UpdateTransforms(Bone* pNode);
	void SaveSkeleton(fstream& stream, Bone* parent);


private:
	void CollectMeshes(OutModel& outModel, aiNode* rootNode);
	void CollectBones(OutModel& outModel);

private:
	aiScene* mAIScene;


	unordered_map<String, Bone*> mNodeByName;
	vector<Bone*> mBones;
	unordered_map<string, unsigned int> mBonesToIndex;
	Bone* mSkeleton;
	vector<Matrix4f> mTransforms;

	String mName;
	vector<shared_ptr<MaterialData> > mMaterials;
	vector<shared_ptr<MeshPartData> > mMeshParts;
};