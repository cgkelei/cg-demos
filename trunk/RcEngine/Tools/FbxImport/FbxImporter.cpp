#include "FbxImporter.h"
#include <Graphics/GraphicsCommon.h>
#include <Graphics/VertexDeclaration.h>
#include <Core/XMLDom.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <IO/FileStream.h>
#include <set>

using namespace std;

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(mFBXSdkManager->GetIOSettings()))
#endif

#define MAXBONES_PER_VERTEX 4

namespace {

uint32_t CalculateVertexSize(uint32_t vertexFlag)
{
	uint32_t size = 0;

	size += (vertexFlag & FbxProcesser::Vertex::ePosition) ? 12 : 0;
	size += (vertexFlag & FbxProcesser::Vertex::eBlendWeight) ? 16 : 0;
	size += (vertexFlag & FbxProcesser::Vertex::eBlendIndices) ? 16 : 0;
	size += (vertexFlag & FbxProcesser::Vertex::eNormal) ? 12 : 0;
	size += (vertexFlag & FbxProcesser::Vertex::eTexcoord0) ? 8 : 0;
	size += (vertexFlag & FbxProcesser::Vertex::eTexcoord1) ? 8 : 0;
	size += (vertexFlag & FbxProcesser::Vertex::eTangent) ? 12 : 0;
	size += (vertexFlag & FbxProcesser::Vertex::eBinormal) ? 12 : 0;

	return size;
}

shared_ptr<VertexDeclaration> GetVertexDeclaration(uint32_t vertexFlag)
{
	std::vector<VertexElement> elements;

	size_t offset = 0;

	if (vertexFlag & FbxProcesser::Vertex::ePosition)
	{
		elements.push_back(VertexElement(offset, VEF_Float3, VEU_Position, 0));
		offset += 12;
	}

	if (vertexFlag & FbxProcesser::Vertex::eBlendWeight)
	{
		elements.push_back(VertexElement(offset, VEF_Float4, VEU_BlendWeight, 0));
		offset += 16;
	}

	if (vertexFlag & FbxProcesser::Vertex::eBlendIndices)
	{
		elements.push_back(VertexElement(offset, VEF_UInt4, VEU_BlendIndices, 0));
		offset += 16;
	}

	if (vertexFlag & FbxProcesser::Vertex::eNormal)
	{
		elements.push_back(VertexElement(offset, VEF_Float3, VEU_Normal, 0));
		offset += 12;
	}

	if (vertexFlag & FbxProcesser::Vertex::eTexcoord0)
	{
		elements.push_back(VertexElement(offset, VEF_Float2, VEU_TextureCoordinate, 0));
		offset += 8;
	}

	if (vertexFlag & FbxProcesser::Vertex::eTexcoord1)
	{
		elements.push_back(VertexElement(offset, VEF_Float2, VEU_TextureCoordinate, 1));
		offset += 8;
	}

	if (vertexFlag & FbxProcesser::Vertex::eTangent)
	{
		elements.push_back(VertexElement(offset, VEF_Float3, VEU_Tangent, 0));
		offset += 12;
	}
	
	if (vertexFlag & FbxProcesser::Vertex::eBinormal)
	{
		elements.push_back(VertexElement(offset, VEF_Float3, VEU_Binormal, 0));
		offset += 12;
	}

	return std::make_shared<VertexDeclaration>(elements);
}

void CorrectMaterialName(String& matName)
{
	std::replace(matName.begin(), matName.end(), ':', '_');
}

Vector4f FbxToVector4f(const FbxVector4& fbx)
{
	return Vector4f(float(fbx[0]), float(fbx[1]), float(fbx[2]), float(fbx[3]));
}

Vector3f FbxToVector3f(const FbxVector4& fbx)
{
	return Vector3f(float(fbx[0]), float(fbx[1]), float(fbx[2]));
}

Vector2f FbxToVector2f(const FbxVector2& fbx)
{
	return Vector2f(float(fbx[0]), float(fbx[1]));
}

Matrix4f MatrixFromFbxAMatrix(const FbxAMatrix& fbxMatrix)
{
	FbxVector4 r1 = fbxMatrix.GetRow(0);
	FbxVector4 r2 = fbxMatrix.GetRow(1);
	FbxVector4 r3 = fbxMatrix.GetRow(2);
	FbxVector4 r4 = fbxMatrix.GetRow(3);

	return Matrix4f((float)r1[0], (float)r1[1], (float)r1[2], (float)r1[3],
					(float)r2[0], (float)r2[1], (float)r2[2], (float)r2[3],
					(float)r3[0], (float)r3[1], (float)r3[2], (float)r3[3],
					(float)r4[0], (float)r4[1], (float)r4[2], (float)r4[3]);
}

Vector3f ReadMaterialColor(FbxPropertyT<FbxDouble3> FBXColorProperty, FbxPropertyT<FbxDouble> FBXFactorProperty)
{
	Vector3f Color;

	if( FBXColorProperty.IsValid() )
	{
		FbxDouble3 FBXColor = FBXColorProperty.Get();
		Color = Vector3f( 
			static_cast<float>(FBXColor[0]),
			static_cast<float>(FBXColor[1]),
			static_cast<float>(FBXColor[2]) );

		if( FBXFactorProperty.IsValid() )
		{
			double FBXFactor = FBXFactorProperty.Get();
			Color = Color * static_cast<float>(FBXFactor);
		}
	}
	return Color;
}

bool GetMaterialTexture(const FbxSurfaceMaterial *pMaterial, const char *pPropertyName, String* filename)
{
	FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);

	if (!lProperty.IsValid())
		return false;

	if (int lTextureCount = lProperty.GetSrcObjectCount(FbxFileTexture::ClassId))
	{
		FbxTexture* lTexture = FbxCast<FbxTexture>(lProperty.GetSrcObject(FbxTexture::ClassId, 0));  
		if (lTexture)
		{
			if( FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(lTexture))
			{
				*filename = lFileTexture->GetFileName();
			}		
		}
	}

	return true;
}

}

void FbxProcesser::BoneWeights::AddBoneWeight( int nBoneIndex, float fBoneWeight )
{
	if( fBoneWeight <= 0.0f )
		return;

	bool added = false;
	for (size_t i = 0; i < mBoneWeights.size(); ++i)
	{
		if (fBoneWeight > mBoneWeights[i].second)
		{
			mBoneWeights.insert(mBoneWeights.begin() + i, std::make_pair(nBoneIndex,fBoneWeight));
			added = true;
			break;
		}
	}

	if (!added)
	{
		mBoneWeights.push_back(std::make_pair(nBoneIndex,fBoneWeight));
	}
}

void FbxProcesser::BoneWeights::Validate()
{
	// make sure not exceed max bone per vertex
	int size = mBoneWeights.size();
	if (size > MAXBONES_PER_VERTEX)
	{
		mBoneWeights.resize(MAXBONES_PER_VERTEX);
	}
	else
	{
		mBoneWeights.resize(MAXBONES_PER_VERTEX);

		std::fill_n(mBoneWeights.begin() + size, MAXBONES_PER_VERTEX-size, std::make_pair(int(0), float(0)));	
	}
}

void FbxProcesser::BoneWeights::Normalize()
{
	Validate();

	float sum = 0.0f;
	std::for_each(mBoneWeights.begin(), mBoneWeights.end(), [&sum](const std::pair<int, float>& value) 
	{
		sum += value.second;
	} );

	float scale = 1.0f / sum;
	std::for_each(mBoneWeights.begin(), mBoneWeights.end(), [scale](std::pair<int, float>& value)
	{
		value.second *= scale;
	});
}


FbxProcesser::FbxProcesser(void)
	: mFBXScene(nullptr), mFBXSdkManager(nullptr)
{
}

FbxProcesser::~FbxProcesser(void)
{
}

bool FbxProcesser::Initialize()
{
	// Create the FBX SDK Manager, destroy the old manager at first  
	if (mFBXSdkManager)
	{
		mFBXSdkManager->Destroy();
		mFBXSdkManager = nullptr;
	}

	mFBXSdkManager = FbxManager::Create();
	if( !mFBXSdkManager )
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else
	{
		FBXSDK_printf("Autodesk FBX SDK version %s\n", mFBXSdkManager->GetVersion());
	}

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(mFBXSdkManager, IOSROOT);
	mFBXSdkManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	mFBXSdkManager->LoadPluginsDirectory(lPath.Buffer());

	// Create the entity that hold the whole Scene  
	mFBXScene = FbxScene::Create(mFBXSdkManager , "");  
	if( !mFBXScene )
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}

	return true;
}

bool FbxProcesser::LoadScene( const char* filename )
{
	if (!mFBXSdkManager)
	{
		return false;
	}

	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;

	int i, lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(mFBXSdkManager,"");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(filename, -1, mFBXSdkManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if( !lImportStatus )
	{
		FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", lImporter->GetLastErrorString());

		if (lImporter->GetLastErrorID() == FbxIOBase::eFileVersionNotSupportedYet ||
			lImporter->GetLastErrorID() == FbxIOBase::eFileVersionNotSupportedAnymore)
		{
			FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", filename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", filename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		FBXSDK_printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		FBXSDK_printf("\n");

		for(i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			FBXSDK_printf("    Animation Stack %d\n", i);
			FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			FBXSDK_printf("\n");
		}

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(mFBXScene);

	if(lStatus == false && lImporter->GetLastErrorID() == FbxIOBase::ePasswordError)
	{
		FBXSDK_printf("Please enter password: ");

		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

			FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(mFBXScene);

		if(lStatus == false && lImporter->GetLastErrorID() == FbxIOBase::ePasswordError)
		{
			FBXSDK_printf("\nPassword is wrong, import aborted.\n");
		}
	}

	//// Convert to DirectX axis system
	//FbxAxisSystem currentAxisSystem = mFBXScene->GetGlobalSettings().GetAxisSystem();

	//if (FbxAxisSystem::DirectX != currentAxisSystem)
	//{
	//	FbxAxisSystem::DirectX.ConvertScene(mFBXScene);
	//}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}

void FbxProcesser::ProcessScene( FbxScene* fbxScene )
{
	CollectMaterials(mFBXScene);
	CollectBones(mFBXScene);
	CollectMeshes(mFBXScene);
}

void FbxProcesser::RunCommand( const vector<String>& arguments )
{

}

void FbxProcesser::ProcessNode( FbxNode* pNode, FbxNodeAttribute::EType attributeType )
{
	if( !pNode )
		return;

	FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
	if (pNodeAttribute)
	{
		if( pNodeAttribute->GetAttributeType() == attributeType )
		{
			switch(pNodeAttribute->GetAttributeType())
			{
			case FbxNodeAttribute::eSkeleton:
				ProcessSkeleton(pNode);
				break;
			case FbxNodeAttribute::eMesh:
				ProcessMesh(pNode);
				break;

			default:
				break;
			};
		}
	}

	for( int i = 0; i < pNode->GetChildCount(); ++i )
	{
		ProcessNode(pNode->GetChild(i), attributeType);
	}
}

void FbxProcesser::ProcessMesh( FbxNode* pNode )
{
	FbxMesh* pMesh = pNode->GetMesh();

	if (!pMesh)
	{
		return;
	}

	if (!pMesh->IsTriangleMesh())
	{
		// triangulation first
		FbxGeometryConverter GeometryConverter(mFBXSdkManager);
		if( !GeometryConverter.TriangulateInPlace( pNode ) )
		{
			std::cerr << "Triangulate mesh error!" << std::endl;
			return;
		}

		pMesh = pNode->GetMesh();
	}

	// material count this mesh will use
	int numMaterial = pNode->GetMaterialCount();

	// just use layer 0.
	FbxLayer* layer = pMesh->GetLayer(0);

	int tangentRefMode = layer->GetTangents() ? layer->GetTangents()->GetReferenceMode() : -1;
	int binormalRefMode = layer->GetBinormals() ? layer->GetBinormals()->GetReferenceMode() : -1;

	// group the polygons by material
	vector< vector<int> > polysByMaterial;
	polysByMaterial.resize((std::max)(1,numMaterial));
    const FbxLayerElementMaterial *materials = layer->GetMaterials();

	bool useDefaultMat = false;
	if (materials) 
	{
		FbxLayerElement::EMappingMode mm = materials->GetMappingMode();

		static const char *mm_str[] = { "eNONE", "eBY_CONTROL_POINT", "eBY_POLYGON_VERTEX", "eBY_POLYGON", "eBY_EDGE", "eALL_SAME" };
		

		if( !(mm == FbxLayerElement::eByPolygon || mm == FbxLayerElement::eAllSame) )
		{
			// only support eByPolygon and eAllSame
			std::cerr << "Unsupported mapping mode: " << ((mm <= FbxLayerElement::eAllSame) ? mm_str[mm] : "unknown") << std::endl;
			return;
		}
			
		const FbxLayerElementArrayTemplate<int>& polyMatIndices = materials->GetIndexArray();

		if (mm == FbxLayerElement::eByPolygon) 
		{
			// each poly has its own material index
			for (int i = 0; i < polyMatIndices.GetCount(); ++i)
			{
				int materialIndex = polyMatIndices.GetAt(i);  
				polysByMaterial[materialIndex].push_back(i);
			}	
		} 
		else if (mm == FbxLayerElement::eAllSame) 
		{
			// all the polys share the same material
			for (int i = 0; i < pMesh->GetPolygonCount(); ++i)
			{
				polysByMaterial[0].push_back(i);
			}
		}
	} 
	else 
	{
		// no materials found, so use default material
		useDefaultMat = true;
		for (int i = 0; i < pMesh->GetPolygonCount(); ++i)
			polysByMaterial[0].push_back(i);
	}

	// get the uv sets
	vector<String> uvSets;
	for (int i = 0; i < layer->GetUVSetCount(); ++i)
		uvSets.push_back(layer->GetUVSets()[i]->GetName());

	
	// test if had skin
	bool lHasSkin = false;
	const int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int iSkinIndex = 0; iSkinIndex < lSkinCount; ++iSkinIndex)
	{
		if( ((FbxSkin*)pMesh->GetDeformer(iSkinIndex, FbxDeformer::eSkin))->GetClusterCount() > 0 )
		{
			lHasSkin = true;
			break;
		}
	}

	std::vector<BoneWeights> meshBoneWeights;
	
	if (lHasSkin)
	{
		meshBoneWeights.resize(pMesh->GetControlPointsCount());
		// Deform the vertex array with the skin deformer.
		ProcessBoneWeights(pMesh, meshBoneWeights);
	}


	// build mesh data
	shared_ptr<MeshData> mesh = std::make_shared<MeshData>();
	
	// read name
	mesh->Name = pNode->GetName();

	for (size_t mi = 0; mi < polysByMaterial.size(); ++mi)
	{
		shared_ptr<MeshPartData> meshPart = std::make_shared<MeshPartData>();
		BoundingSpheref sphere;
		
		// mesh part name
		meshPart->Name = pNode->GetName();
		if (polysByMaterial.size() > 1)
		{
			meshPart->Name += LexicalCast<String>(mi);
		}

		// material name
		meshPart->MaterialName = useDefaultMat ? "DefaultMaterial" : pNode->GetMaterial(mi)->GetName();
		CorrectMaterialName(meshPart->MaterialName);

		uint32_t vertexFlag = 0;
		
		if (tangentRefMode != -1)
		{
			FbxSurfaceMaterial *material = pNode->GetMaterial(mi);
			
			String filename;
			if (GetMaterialTexture(material, FbxSurfaceMaterial::sBump, &filename))
			{
				if (!filename.empty())
					vertexFlag |= Vertex::eTexcoord0| Vertex::eTangent;
			}
		}

		if (!useDefaultMat)
		{
			FbxSurfaceMaterial *material = pNode->GetMaterial(mi);

			String filename;
			if (GetMaterialTexture(material, FbxSurfaceMaterial::sDiffuse, &filename))
			{
				if (!filename.empty())
					vertexFlag |= Vertex::eTexcoord0;
			}
		}

		if (lHasSkin)
		{
			vertexFlag |= Vertex::eBlendIndices;
			vertexFlag |= Vertex::eBlendWeight;
		}

		std::set<Vertex> exitingVertices;

		for (size_t pi = 0; pi < polysByMaterial[mi].size(); ++pi)
		{
			int polyIndex = polysByMaterial[mi][pi];
			assert(pMesh->GetPolygonSize(polyIndex) == 3);

			for (size_t vi = 0; vi < 3; ++vi)
			{
				Vertex vertex;
				
				// get position
				int ctrlPointIndex = pMesh->GetPolygonVertex(polyIndex , vi); 
				vertex.Position = FbxToVector3f(pMesh->GetControlPointAt(ctrlPointIndex));
				vertexFlag |= Vertex::ePosition;

				sphere.Merge(vertex.Position);


				if (vertexFlag & Vertex::eBlendWeight)
				{
					auto& boneWeights = meshBoneWeights[ctrlPointIndex];
					boneWeights.Normalize();

					auto& indicesAndWeights = boneWeights.GetBoneWeights();
					
					for (auto iter = indicesAndWeights.begin(); iter != indicesAndWeights.end(); ++iter)
					{
						vertex.BlendIndices.push_back(iter->first);
						vertex.BlendWeights.push_back(iter->second);
					}
				}


				// get normal
				FbxVector4 fbxNormal;
				if( pMesh->GetPolygonVertexNormal(polyIndex, vi, fbxNormal) )
				{
					vertex.Normal = FbxToVector3f(fbxNormal);
					vertex.Normal.Normalize();
					vertexFlag |= Vertex::eNormal;
				}
				
					
				for (size_t uv_idx = 0; uv_idx  < uvSets.size(); ++uv_idx)
				{
					FbxVector2 uv;
					if( pMesh->GetPolygonVertexUV(polyIndex, vi, uvSets[uv_idx].c_str(), uv) )
					{
						// flip the y-coordinate to match DirectX
						uv[1] = 1 - uv[1];

						if (uv_idx == 0) 
						{
							vertex.Tex0 = FbxToVector2f(uv);
							vertexFlag |= Vertex::eTexcoord0;
						}
						else if (uv_idx == 1) 
						{
							vertex.Tex1 = FbxToVector2f(uv);
							vertexFlag |= Vertex::eTexcoord1;
						} 
						else 
						{
							std::cerr << "Too many uv-sets!\n" ;
						}
					}						
				}

				if (vertexFlag & Vertex::eTangent)
				{
					FbxVector4 fbxTangent, fbxBinormal;

					if (tangentRefMode == FbxLayerElement::eDirect) 
					{
						fbxTangent = layer->GetTangents()->GetDirectArray()[ctrlPointIndex + vi];
					} 
					else
					{
						int idx = layer->GetTangents()->GetIndexArray()[ctrlPointIndex + vi];
						fbxTangent = layer->GetTangents()->GetDirectArray()[idx];
					}
					vertexFlag |= Vertex::eTangent;

					if (binormalRefMode == FbxLayerElement::eDirect) 
					{
						fbxBinormal = layer->GetBinormals()->GetDirectArray()[ctrlPointIndex + vi];
					} 
					else 
					{
						int idx = layer->GetBinormals()->GetIndexArray()[ctrlPointIndex + vi];
						fbxBinormal = layer->GetBinormals()->GetDirectArray()[idx];
					}
					vertexFlag |= Vertex::eBinormal;

					/*	if (isStatic) {
					fbxTangent = mtxTransformNormal.MultT(fbxTangent);
					fbxBinormal = mtxTransformNormal.MultT(fbxBinormal);
					}*/

					vertex.Tangent = FbxToVector3f(fbxTangent);
					vertex.Binormal = FbxToVector3f(fbxBinormal);

					vertex.Tangent.Normalize();
					vertex.Binormal.Normalize();
				}

				size_t index;

				auto exitingIter = exitingVertices.find(vertex);
				if (exitingIter == exitingVertices.end())
				{
					vertex.Index = meshPart->Vertices.size();
					index = vertex.Index;
					vertex.Flag = vertexFlag;
					exitingVertices.insert(vertex);
					meshPart->Vertices.push_back(vertex);
				}
				else
				{
					index = exitingIter->Index;
				}

				meshPart->Indices.push_back(index);
			}
			meshPart->Bound = sphere;
		}
		mesh->Bound.Merge(meshPart->Bound);
		mesh->MeshParts.push_back(meshPart);
	}
	mSceneMeshes.push_back(mesh);
}

void FbxProcesser::ProcessSkeleton( FbxNode* pNode )
{
	FbxSkeleton* pFBXSkeleton = pNode->GetSkeleton();

	if (!pFBXSkeleton)
	{
		return;
	}

	if (!mSkeleton)
	{
		mSkeleton = std::make_shared<Skeleton>();
	}

	Bone* parentBone = nullptr;
	
	FbxNode* pParentNode = pNode->GetParent();
	if (pParentNode)
	{
		parentBone = mSkeleton->GetBone(pParentNode->GetName());
	}

	mSkeleton->AddBone(pNode->GetName(), parentBone);
}

void FbxProcesser::ProcessBoneWeights( FbxMesh* pMesh, std::vector<BoneWeights>& meshBoneWeights )
{
	if( !mSkeleton )
		return;

	for( int i = 0; i < pMesh->GetDeformerCount(); ++i )
	{
		FbxDeformer* pFBXDeformer = pMesh->GetDeformer(i);

		if( !pFBXDeformer )
			continue;

		if( pFBXDeformer->GetDeformerType() == FbxDeformer::eSkin )
		{
			FbxSkin* pFBXSkin = (FbxSkin *)(pFBXDeformer);

			if( !pFBXSkin )
				continue;

			FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize; //Default link mode

			int nClusterCount = pFBXSkin->GetClusterCount();
			for( int i = 0; i < nClusterCount; ++i )
			{
				FbxCluster* pFBXCluster = pFBXSkin->GetCluster(i);

				if( !pFBXCluster )
					continue;

				linkMode = pFBXCluster->GetLinkMode();
				FbxNode* pLinkNode = pFBXCluster->GetLink();

				if( !pLinkNode )
					continue;

				Bone* skeletonBone = mSkeleton->GetBone(pLinkNode->GetName());
				Bone* parentBone = (Bone*)skeletonBone->GetParent();

				if (!skeletonBone)
				{
					continue;
				}

				FbxAMatrix matClusterTransformMatrix;
				FbxAMatrix matClusterLinkTransformMatrix;
				pFBXCluster->GetTransformMatrix(matClusterTransformMatrix);
				pFBXCluster->GetTransformLinkMatrix(matClusterLinkTransformMatrix);

				FbxAMatrix matInvBindPose = matClusterTransformMatrix * matClusterLinkTransformMatrix.Inverse();


				Matrix4f matBindPose = MatrixFromFbxAMatrix(matInvBindPose.Inverse());

				if (parentBone)
				{
					Matrix4f paretTrans = parentBone->GetWorldTransform();

					matBindPose = matBindPose * paretTrans.Inverse();
				}
 
				Vector3f pos, scale;
				Quaternionf rot;
				MatrixDecompose(scale, rot, pos, matBindPose);

				skeletonBone->SetTransform(pos, rot, scale);
			
				/*skeletonBone->SetBindPoseTransform(KFbxXMatrixToD3DXMATRIX(matClusterLinkTransformMatrix));
				pSkeletonBone->SetBoneReferenceTransform(KFbxXMatrixToD3DXMATRIX(matClusterTransformMatrix));*/

				int* indices = pFBXCluster->GetControlPointIndices();
				double* weights = pFBXCluster->GetControlPointWeights();

				for( int j = 0; j < pFBXCluster->GetControlPointIndicesCount(); ++j )
				{
					int ctlPointIndex = indices[j];
					meshBoneWeights[ctlPointIndex].AddBoneWeight(skeletonBone->GetBoneIndex(), (float)weights[j]);
				}
			}
		}
	}
}

void FbxProcesser::CollectMaterials( FbxScene* pScene )
{ 
	for( int i = 0; i < pScene->GetMaterialCount(); ++i )
	{
		FbxSurfaceMaterial* pSurfaceMaterial = pScene->GetMaterial(i);

		String matName = pSurfaceMaterial->GetName();

		Vector3f AmbientColor;
		Vector3f EmissiveColor;
		Vector3f DiffuseColor;
		Vector3f SpecularColor;
		float fSpecularPower = 1.0f;
		float fTransparency = 1.0f;

		FbxSurfaceLambert* pLambert = FbxCast<FbxSurfaceLambert>(pSurfaceMaterial);
		FbxSurfacePhong* pPhong = FbxCast<FbxSurfacePhong>(pSurfaceMaterial);

		if( pLambert )
		{
			AmbientColor = ReadMaterialColor(pLambert->Ambient, pLambert->AmbientFactor);
			EmissiveColor = ReadMaterialColor(pLambert->Emissive, pLambert->EmissiveFactor);
			DiffuseColor = ReadMaterialColor(pLambert->Diffuse, pLambert->DiffuseFactor);

			FbxPropertyT<FbxDouble> FBXTransparencyProperty = pLambert->TransparencyFactor;
			if( FBXTransparencyProperty.IsValid() )
				fTransparency = static_cast<float>( FBXTransparencyProperty.Get() );
		}

		if( pPhong )
		{
			SpecularColor = ReadMaterialColor(pPhong->Specular, pPhong->SpecularFactor);

			FbxPropertyT<FbxDouble> FBXSpecularPowerProperty = pPhong->Shininess;
			if( FBXSpecularPowerProperty.IsValid() )
				fSpecularPower = static_cast<float>( FBXSpecularPowerProperty.Get() );
		}

		int textureLayerIndex;  
		FbxProperty pProperty;  
		int texID;  

		for(int textureLayerIndex = 0 ; textureLayerIndex < FbxLayerElement::sTypeTextureCount ; ++textureLayerIndex)
		{  
			pProperty = pSurfaceMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[textureLayerIndex]);  
			if(pProperty.IsValid())  
			{  
				int textureCount = pProperty.GetSrcObjectCount(FbxTexture::ClassId);  

				for(int tex = 0 ; tex < textureCount ; ++tex)  
				{  
					FbxTexture* pTexture = FbxCast<FbxTexture>(pProperty.GetSrcObject(FbxTexture::ClassId,tex));  
					if(pTexture)  
					{  
						FbxFileTexture *pFileTexture = FbxCast<FbxFileTexture>(pTexture);
						if (pFileTexture)
						{
							 String filepath =  pFileTexture->GetFileName();
							 std::cout << FbxLayerElement::sTextureChannelNames[textureLayerIndex] << ": " << filepath << std::endl;
						}
						
					}  
				}  
			}  
		}  

		MaterialData matData;

		matData.Ambient = ColorRGBA(AmbientColor.X(), AmbientColor.Y(), AmbientColor.Z(), 1.0f);
		matData.Diffuse = ColorRGBA(DiffuseColor.X(), DiffuseColor.Y(), DiffuseColor.Z(), 1.0f);
		matData.Specular = ColorRGBA(SpecularColor.X(), SpecularColor.Y(), SpecularColor.Z(), 1.0f);
		matData.Emissive = ColorRGBA(EmissiveColor.X(), EmissiveColor.Y(), EmissiveColor.Z(), 1.0f);
		matData.Power = fSpecularPower;

		mMaterials.push_back(matData);
	}
}

void FbxProcesser::CollectBones( FbxScene* fbxScene )
{
	ProcessNode(fbxScene->GetRootNode(), FbxNodeAttribute::eSkeleton);
}

void FbxProcesser::CollectMeshes( FbxScene* fbxScene )
{
	ProcessNode(fbxScene->GetRootNode(), FbxNodeAttribute::eMesh);
}

void FbxProcesser::BuildAndSaveXML( )
{
	for (size_t mi = 0; mi < mSceneMeshes.size(); ++mi)
	{
		MeshData& mesh  = *(mSceneMeshes[mi]);

		XMLDoc meshxml;

		XMLNodePtr meshNode = meshxml.AllocateNode(XML_Node_Element, "mesh");
		meshxml.RootNode(meshNode);

		// mesh name
		meshNode->AppendAttribute(meshxml.AllocateAttributeString("name", mesh.Name));
		
		// mesh bounding
		XMLNodePtr meshBoundNode = meshxml.AllocateNode(XML_Node_Element, "bounding");
		meshBoundNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", mesh.Bound.Center.X()));
		meshBoundNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", mesh.Bound.Center.Y()));
		meshBoundNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", mesh.Bound.Center.Z()));
		meshBoundNode->AppendAttribute(meshxml.AllocateAttributeFloat("radius", mesh.Bound.Radius));
		meshNode->AppendNode(meshBoundNode);

		// mesh skeleton
		XMLNodePtr meshSkeletonNode = meshxml.AllocateNode(XML_Node_Element, "skeleton");
		meshSkeletonNode->AppendAttribute(meshxml.AllocateAttributeUInt("boneCount", mSkeleton->GetBoneCount()));
		vector<Bone*>& bones = mSkeleton->GetBonesModified();
		for (size_t iBone = 0; iBone < bones.size(); ++iBone)
		{
			Bone* parentBone = static_cast<Bone*>(bones[iBone]->GetParent());
			
			XMLNodePtr boneNode = meshxml.AllocateNode(XML_Node_Element, "bone");
			boneNode->AppendAttribute(meshxml.AllocateAttributeString("name", bones[iBone]->GetName()));
			boneNode->AppendAttribute(meshxml.AllocateAttributeString("name", parentBone ? parentBone->GetName() : ""));
			meshSkeletonNode->AppendNode(boneNode);

			Vector3f pos = bones[iBone]->GetPosition();
			Vector3f scale = bones[iBone]->GetScale();
			Quaternionf rot = bones[iBone]->GetRotation();

			XMLNodePtr posNode = meshxml.AllocateNode(XML_Node_Element, "bindPosition");
			posNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", pos.X()));
			posNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", pos.Y()));
			posNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", pos.Z()));
			boneNode->AppendNode(posNode);

			XMLNodePtr rotNode = meshxml.AllocateNode(XML_Node_Element, "bindRotation");
			rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("w", rot.W()));
			rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", rot.X()));
			rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", rot.Y()));
			rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", rot.Z()));
			boneNode->AppendNode(rotNode);

			XMLNodePtr scaleNode = meshxml.AllocateNode(XML_Node_Element, "bindScale");
			scaleNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", scale.X()));
			scaleNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", scale.Y()));
			scaleNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", scale.Z()));
			boneNode->AppendNode(scaleNode);
		}
		meshNode->AppendNode(meshSkeletonNode);

		for (size_t mpi = 0; mpi < mesh.MeshParts.size(); ++mpi)
		{
			MeshPartData& meshPart = *(mesh.MeshParts[mpi]);

			XMLNodePtr meshPartNode = meshxml.AllocateNode(XML_Node_Element, "meshPart");
			XMLAttributePtr meshPartNameAttr = meshxml.AllocateAttributeString("name", meshPart.Name);
			XMLAttributePtr meshPartMatAttr = meshxml.AllocateAttributeString("material", meshPart.MaterialName);
			meshPartNode->AppendAttribute(meshPartNameAttr);
			meshPartNode->AppendAttribute(meshPartMatAttr);
			// add to mesh
			meshNode->AppendNode(meshPartNode);

			XMLNodePtr boundNode = meshxml.AllocateNode(XML_Node_Element, "bounding");
			boundNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", meshPart.Bound.Center.X()));
			boundNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", meshPart.Bound.Center.Y()));
			boundNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", meshPart.Bound.Center.Z()));
			boundNode->AppendAttribute(meshxml.AllocateAttributeFloat("radius", meshPart.Bound.Radius));
			meshPartNode->AppendNode(boundNode);

			XMLNodePtr verticesNode = meshxml.AllocateNode(XML_Node_Element, "vertices");
			XMLAttributePtr verticesCountAttr = meshxml.AllocateAttributeUInt("verticesCount", meshPart.Vertices.size());
			XMLAttributePtr vertexSizeAttr = meshxml.AllocateAttributeUInt("vertexSize", CalculateVertexSize(meshPart.Vertices[0].Flag));
			verticesNode->AppendAttribute(verticesCountAttr);
			verticesNode->AppendAttribute(vertexSizeAttr);		
			// add to mesh part
			meshPartNode->AppendNode(verticesNode);

			for (size_t vi = 0; vi < meshPart.Vertices.size(); ++vi)
			{
				Vertex& vertex = meshPart.Vertices[vi];
				XMLNodePtr vertexNode = meshxml.AllocateNode(XML_Node_Element, "vertex");
				verticesNode->AppendNode(vertexNode);

				uint32_t vertexFlag = vertex.Flag;
				if (vertexFlag & Vertex::ePosition)
				{
					XMLNodePtr positionNode = meshxml.AllocateNode(XML_Node_Element, "position");
					positionNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", vertex.Position.X()));
					positionNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", vertex.Position.Y()));
					positionNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", vertex.Position.Z()));
					vertexNode->AppendNode(positionNode);
				}

				if (vertexFlag & Vertex::eNormal)
				{
					XMLNodePtr normalNode = meshxml.AllocateNode(XML_Node_Element, "normal");
					normalNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", vertex.Normal.X()));
					normalNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", vertex.Normal.Y()));
					normalNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", vertex.Normal.Z()));
					vertexNode->AppendNode(normalNode);
				}

				if (vertexFlag & Vertex::eTexcoord0)
				{
					XMLNodePtr texcoordNode = meshxml.AllocateNode(XML_Node_Element, "texcoord");
					texcoordNode->AppendAttribute(meshxml.AllocateAttributeFloat("u", vertex.Tex0.X()));
					texcoordNode->AppendAttribute(meshxml.AllocateAttributeFloat("v", vertex.Tex0.Y()));
					vertexNode->AppendNode(texcoordNode);
				}

				if (vertexFlag & Vertex::eTexcoord1)
				{
					XMLNodePtr texcoordNode = meshxml.AllocateNode(XML_Node_Element, "texcoord");
					texcoordNode->AppendAttribute(meshxml.AllocateAttributeFloat("u", vertex.Tex1.X()));
					texcoordNode->AppendAttribute(meshxml.AllocateAttributeFloat("v", vertex.Tex1.Y()));
					vertexNode->AppendNode(texcoordNode);
				}

				if (vertexFlag & Vertex::eTangent)
				{
					XMLNodePtr tangentNode = meshxml.AllocateNode(XML_Node_Element, "tangent");
					tangentNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", vertex.Tangent.X()));
					tangentNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", vertex.Tangent.Y()));
					tangentNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", vertex.Tangent.Z()));
					vertexNode->AppendNode(tangentNode);
				}

				if (vertexFlag & Vertex::eBinormal)
				{
					XMLNodePtr binormalNode = meshxml.AllocateNode(XML_Node_Element, "binormal");
					binormalNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", vertex.Binormal.X()));
					binormalNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", vertex.Binormal.Y()));
					binormalNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", vertex.Binormal.Z()));
					vertexNode->AppendNode(binormalNode);
				}

				if (vertexFlag & Vertex::eBlendWeight)
				{
					for (size_t ib = 0; ib < 4; ++ib)
					{
						XMLNodePtr boneNode = meshxml.AllocateNode(XML_Node_Element, "bone");
						boneNode->AppendAttribute(meshxml.AllocateAttributeUInt("index", vertex.BlendIndices[ib]));
						boneNode->AppendAttribute(meshxml.AllocateAttributeFloat("weight", vertex.BlendWeights[ib]));
						vertexNode->AppendNode(boneNode);
					}
				}
			}
			

			size_t triangleCount = meshPart.Indices.size() / 3;
			XMLNodePtr trianglesNode = meshxml.AllocateNode(XML_Node_Element, "triangles");
			trianglesNode->AppendAttribute(meshxml.AllocateAttributeUInt("triangleCount", triangleCount));

			// add to mesh part
			meshPartNode->AppendNode(trianglesNode);

			for (size_t tri = 0; tri < triangleCount; ++tri)
			{
				XMLNodePtr triangleNode = meshxml.AllocateNode(XML_Node_Element, "triangle");
				triangleNode->AppendAttribute(meshxml.AllocateAttributeUInt("a", meshPart.Indices[3*tri]));
				triangleNode->AppendAttribute(meshxml.AllocateAttributeUInt("b", meshPart.Indices[3*tri+1]));
				triangleNode->AppendAttribute(meshxml.AllocateAttributeUInt("c", meshPart.Indices[3*tri+2]));
				trianglesNode->AppendNode(triangleNode);
			}			
		}

		std::ofstream xmlFile(mesh.Name + ".mesh.xml");
		meshxml.Print(xmlFile);
		xmlFile.close();
	}	
}

void FbxProcesser::BuildAndSaveBinary( )
{
	const uint32_t MeshId = ('M' << 24) | ('E' << 16) | ('S' << 8) | ('H');

	for (size_t mi = 0; mi < mSceneMeshes.size(); ++mi)
	{
		MeshData& mesh  = *(mSceneMeshes[mi]);

		FileStream stream;
		stream.Open(mesh.Name + ".mesh", FILE_WRITE);

		// Write mesh id
		stream.WriteUInt(MeshId);
		
		// write mesh name
		stream.WriteString(mesh.Name);

		// write mesh bounding sphere
		stream.Write(&mesh.Bound.Center, sizeof(Vector3f));
		stream.WriteFloat(mesh.Bound.Radius);

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
			stream.Write(&meshPart.Bound.Center, sizeof(Vector3f));
			stream.WriteFloat(meshPart.Bound.Radius);

			// write vertex count and vertex size
			stream.WriteUInt(meshPart.Vertices.size());
			stream.WriteUInt(vertexSize);

			// write vertex declaration, elements count
			stream.WriteUInt(vertexDecl->GetElementCount());
			
			const std::vector<VertexElement>& elements = vertexDecl->GetElements();
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
					stream.Write(&vertex.Position, sizeof(Vector3f));
					checkSize += sizeof(Vector3f);
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
					stream.Write(&vertex.Normal, sizeof(Vector3f));
					checkSize += sizeof(Vector3f);
				}

				if (vertexFlag & Vertex::eTexcoord0)
				{
					stream.Write(&vertex.Tex0, sizeof(Vector2f));
					checkSize += sizeof(Vector2f);
				}

				if (vertexFlag & Vertex::eTexcoord1)
				{
					stream.Write(&vertex.Tex1, sizeof(Vector2f));
					checkSize += sizeof(Vector2f);
				}

				if (vertexFlag & Vertex::eTangent)
				{
					stream.Write(&vertex.Tangent, sizeof(Vector3f));
					checkSize += sizeof(Vector3f);
				}

				if (vertexFlag & Vertex::eBinormal)
				{
					stream.Write(&vertex.Binormal, sizeof(Vector3f));
					checkSize += sizeof(Vector3f);
				}				
			}
			assert(checkSize == bufferSize);

			// write indices count
			stream.WriteUInt(meshPart.Indices.size());

			if (meshPart.Indices.size() < (std::numeric_limits<uint32_t>::max)())
			{
				stream.WriteUInt(IBT_Bit16);
				for (size_t i = 0; i < meshPart.Indices.size(); ++i)
				{
					stream.WriteUShort(meshPart.Indices[i]);
				}
			}
			else
			{
				stream.WriteUInt(IBT_Bit32);
				stream.Write(&(meshPart.Indices[0]), sizeof(char) * meshPart.Indices.size());
			}
		}

		if (mSkeleton && mSkeleton->GetBoneCount())
		{
			auto& bones = mSkeleton->GetBones();
			stream.WriteUInt(bones.size());
			for (size_t iBone = 0; iBone < bones.size(); ++iBone)
			{
				Bone* bone = bones[iBone];
				Bone* parentBone = static_cast<Bone*>(bones[iBone]->GetParent());

				String parentName = "";
				if (parentBone)
				{
					parentName = parentBone->GetName();
				}

				Vector3f pos = bone->GetPosition();
				Vector3f scale = bone->GetScale();
				Quaternionf rot = bone->GetRotation();

				stream.WriteString(bone->GetName());
				stream.WriteString(parentName);

				stream.Write(&pos, sizeof(Vector3f));
				stream.Write(&rot, sizeof(Quaternionf));
				stream.Write(&scale, sizeof(Vector3f));

			}
		}
		else
		{
			stream.WriteUInt(0);
		}

		//// no animation
		//stream.WriteUInt(0);

		stream.WriteUInt(1);
		stream.WriteString("dudeWalk.anim");
		
		stream.Close();
	}
}

void FbxProcesser::ProcessAnimations( FbxScene* fbxScene )
{

}

int main()
{
	//FbxProcesser::BoneWeights test;

	//test.AddBoneWeight(1, 0.353f);
	//test.AddBoneWeight(3, 0.453f);
	//test.AddBoneWeight(2, 0.153f);
	//test.AddBoneWeight(2, 0.113f);
	//test.AddBoneWeight(2, 0.013f);
	//test.Normalize();

	FbxProcesser fbxProcesser;
	fbxProcesser.Initialize();

	if (fbxProcesser.LoadScene("dude.FBX"))
	{
		fbxProcesser.ProcessScene(0);
		fbxProcesser.BuildAndSaveXML();
		fbxProcesser.BuildAndSaveBinary();
	}

	return 0;
}