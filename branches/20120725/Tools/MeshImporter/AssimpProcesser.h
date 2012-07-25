#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Math/Matrix.h"
#include "Content/MeshContentLoader.h"

#pragma comment(lib, "assimp.lib")

using namespace std;
using namespace RcEngine::Content;
using namespace RcEngine::Math;

class Bone {
public:

	std::string Name;
	Matrix4f Offset, LocalTransform, GlobalTransform, OriginalLocalTransform;
	Bone* Parent;
	vector<Bone*> Children;

	Bone() :Parent(0){}
	~Bone(){ for(size_t i(0); i< Children.size(); i++) delete Children[i]; }
};



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

	void ExportXML(const char* output, MeshContent* meshContent);
	void ExportBinary(const char* output, MeshContent* meshContent);


private:
	Bone* CreateBoneTrees(aiNode* pNode, Bone* parent);
	void CalculateBoneToWorldTransform(Bone* pNode);

	void UpdateTransforms(Bone* pNode);

	void SaveSkeleton(fstream& stream, Bone* parent);

private:
	unordered_map<string, Bone*> mNodeByName;
	vector<Bone*> mBones;
	unordered_map<string, unsigned int> mBonesToIndex;
	Bone* mSkeleton;/** Root node of the internal scene structure */
	vector<Matrix4f> mTransforms;


};