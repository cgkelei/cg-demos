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
#include "Graphics/Skeleton.h"
#include "Math/ColorRGBA.h"
#include "Math/MathUtil.h"

#pragma comment(lib, "assimp.lib")

using namespace std;
using namespace RcEngine;

struct MaterialData;
struct MeshPartData;

struct OutModel
{
	OutModel() 
		: RootBone(NULL), RootNode(NULL), TotalVertices(0), TotalIndices(0) 
	{

	}

	String OutName;
	vector<aiMesh*> Meshes;
	vector<aiNode*> MeshNodes;
	vector<aiNode*> Bones;
	vector<aiAnimation*> Animations;
	aiNode* RootNode;
	aiNode* RootBone;
	uint32_t TotalVertices;
	uint32_t TotalIndices;
	vector<shared_ptr<MeshPartData> > MeshParts;
	BoundingSpheref MeshBoundingSphere;
	shared_ptr<Skeleton> Skeleton;
};

class AssimpProcesser
{
public:
	AssimpProcesser(void);
	~AssimpProcesser(void);

	bool Process(const char* filePath);

private:
	void ProcessScene(const aiScene* scene);

	shared_ptr<MaterialData> ProcessMaterial(aiMaterial* material);

	void ProcessAnimations();

	void ExportXML( OutModel& outModel);
	void ExportBinary( OutModel& outModel );
	void GetBoundingBox(const aiScene* scene, aiVector3D* min, aiVector3D* max);

	
	void ExportModel( OutModel& outModel, const String& outName );

private:
	void CollectMaterials();
	void CollectMeshes(OutModel& outModel, aiNode* rootNode);
	void CollectBones(OutModel& outModel);
	void CollectBonesFinal(vector<aiNode*>& bones, const set<aiNode*>& necessary, aiNode* node);
	void CollectAnimations(OutModel& model, aiScene* scene);

	void BuildAndSaveModel(OutModel& outModel);
	void BuildAndSaveAnimations(OutModel& model);

	void BuildSkeleton(OutModel& model);


private:
	aiScene* mAIScene;

	String mName;
	vector<shared_ptr<MaterialData> > mMaterials;
	vector<shared_ptr<MeshPartData> > mMeshParts;

	OutModel mModel;
};