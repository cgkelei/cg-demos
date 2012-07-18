#pragma once

#include <iostream>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Content/MeshContentLoader.h"


#pragma comment(lib, "assimp.lib")

using namespace RcEngine::Content;

class AssimpProcesser
{
public:
	AssimpProcesser(void);
	~AssimpProcesser(void);

	bool Process(const char* filePath);

private:
	void ProcessScene(const aiScene* scene);
	void ProcessMesh( aiMesh* mesh, MeshContent* meshContent );
	void ProcessMaterial(aiMaterial* material, MeshContent* meshContent );

	void GetBoundingBox(const aiScene* scene, aiVector3D* min, aiVector3D* max);

	void Export(const char* output, MeshContent* meshContent);

};