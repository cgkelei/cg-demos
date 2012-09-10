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

	void ExportXML( OutModel& outModel);
	void ExportBinary( OutModel& outModel );
	void GetBoundingBox(const aiScene* scene, aiVector3D* min, aiVector3D* max);

	
	void ExportModel( OutModel& outModel, const String& outName );

private:
	void CollectMeshes(OutModel& outModel, aiNode* rootNode);
	void CollectBones(OutModel& outModel);
	void CollectBonesFinal(vector<aiNode*>& bones, const set<aiNode*>& necessary, aiNode* node);

	void BuildAndSaveModel(OutModel& outModel);

private:
	aiScene* mAIScene;

	String mName;
	vector<shared_ptr<MaterialData> > mMaterials;
	vector<shared_ptr<MeshPartData> > mMeshParts;
};