#include "FbxImporter.h"
#include <Graphics/GraphicsCommon.h>
#include <Graphics/VertexDeclaration.h>
#include <Core/XMLDom.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <IO/FileStream.h>
#include <IO/PathUtil.h>
#include <set>

#include "ExportLog.h"

using namespace std;

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(mFBXSdkManager->GetIOSettings()))
#endif

ConsoleOutListener g_ConsoleOutListener;
DebugSpewListener  g_DebugSpewListener;
ExportSettings     g_ExportSettings;


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

void CorrectName(String& matName)
{
	std::replace(matName.begin(), matName.end(), ':', '_');
}

inline float4 float4FromFBX(const FbxVector4& fbx)
{
	return float4(float(fbx[0]), float(fbx[1]), float(fbx[2]), float(fbx[3]));
}

inline float3 float3FromFBX(const FbxVector4& fbx)
{
	return float3(float(fbx[0]), float(fbx[1]), float(fbx[2]));
}

inline float2 float2FromFBX(const FbxVector2& fbx)
{
	return float2(float(fbx[0]), float(fbx[1]));
}

inline Quaternionf quatFromFBX(const FbxQuaternion& quat) 
{
	return Quaternionf((float)quat[3], (float)quat[0], (float)quat[1], (float)quat[2]);
}

float4x4 matrixFromFbxAMatrix(const FbxAMatrix& fbxMatrix)
{
	FbxVector4 translation = fbxMatrix.GetT();
	FbxVector4 scale = fbxMatrix.GetS();
	FbxQuaternion rotation = fbxMatrix.GetQ();

	return CreateTransformMatrix(float3((float)scale[0], (float)scale[1], (float)scale[2]), 
		Quaternionf((float)rotation[3], (float)rotation[0], (float)rotation[1], (float)rotation[2]),
		float3((float)translation[0], (float)translation[1], (float)translation[2]));
}

float3 ReadMaterialColor(FbxPropertyT<FbxDouble3> FBXColorProperty, FbxPropertyT<FbxDouble> FBXFactorProperty)
{
	float3 Color;

	if( FBXColorProperty.IsValid() )
	{
		FbxDouble3 FBXColor = FBXColorProperty.Get();
		Color = float3( 
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

FbxNode* GetBoneRoot(FbxNode* boneNode)
{
	assert(boneNode &&
		boneNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton);

	String name = boneNode->GetName();
	FbxNode* parentNode = boneNode->GetParent();
	FbxNodeAttribute* pParentNodeAttribute = parentNode->GetNodeAttribute();

	while(parentNode &&  pParentNodeAttribute && pParentNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		boneNode = parentNode;
		parentNode = parentNode->GetParent();
		pParentNodeAttribute = parentNode->GetNodeAttribute();
	}

	return boneNode;
}

// Get the matrix of the given pose
FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
	FbxAMatrix lPoseMatrix;
	FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

	memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

	return lPoseMatrix;
}

// Get the global position of the node for the current pose.
// If the specified node is not part of the pose or no pose is specified, get its
// global position at the current time.
FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose = NULL, FbxAMatrix* pParentGlobalPosition = NULL)
{
	FbxAMatrix lGlobalPosition;
	bool        lPositionFound = false;

	if (pPose)
	{
		int lNodeIndex = pPose->Find(pNode);

		if (lNodeIndex > -1)
		{
			// The bind pose is always a global matrix.
			// If we have a rest pose, we need to check if it is
			// stored in global or local space.
			if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
			{
				lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
			}
			else
			{
				// We have a local matrix, we need to convert it to
				// a global space matrix.
				FbxAMatrix lParentGlobalPosition;

				if (pParentGlobalPosition)
				{
					lParentGlobalPosition = *pParentGlobalPosition;
				}
				else
				{
					if (pNode->GetParent())
					{
						lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
					}
				}

				FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
				lGlobalPosition = lParentGlobalPosition * lLocalPosition;
			}

			lPositionFound = true;
		}
	}

	if (!lPositionFound)
	{
		// There is no pose entry for that node, get the current global position instead.

		// Ideally this would use parent global position and local position to compute the global position.
		// Unfortunately the equation 
		//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
		// does not hold when inheritance type is other than "Parent" (RSrs).
		// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
		lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
	}

	return lGlobalPosition;
}

// Get the geometry offset to a node. It is never inherited by the children.
FbxAMatrix GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

bool HasSkin(FbxMesh* pMesh)
{
	const int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int iSkinIndex = 0; iSkinIndex < lSkinCount; ++iSkinIndex)
	{
		if( ((FbxSkin*)pMesh->GetDeformer(iSkinIndex, FbxDeformer::eSkin))->GetClusterCount() > 0 )
		{
			return true;
		}
	}

	return false;
}

float3 BakePosition(const float3& pos, const float4x4& globalTransform)
{
	float4 baked = float4(pos[0], pos[1], pos[2], 1.0f) * globalTransform;
	return float3(baked[0], baked[1], baked[2]);
}

float3 BakeDirection(const float3& dir, const float4x4& globalTransform)
{
	float4 baked = float4(dir[0], dir[1], dir[2], 0.0f) * globalTransform;
	return float3(baked[0], baked[1], baked[2]);
}

}

//void FBXTransformer::Initialize( FbxScene* pScene )
//{
//	ExportLog::LogMsg( 4, "Identifying scene's coordinate system." );
//
//	FbxAxisSystem SceneAxisSystem = pScene->GetGlobalSettings().GetAxisSystem();
//	FbxAxisSystem TargetAxisSystem = FbxAxisSystem::MayaYUp;
//	FbxAxisSystem::MayaYUp.ConvertScene(pScene);
//
//	int iUpAxisSign;
//	FbxAxisSystem::EUpVector UpVector = SceneAxisSystem.GetUpVector( iUpAxisSign );
//
//	if( UpVector == FbxAxisSystem::eZAxis )
//	{
//		ExportLog::LogMsg( 4, "Converting from Z-up axis system to Y-up axis system." );
//		m3dMaxConversion = true;
//	}
//	else
//		m3dMaxConversion = false;
//}
//
//void FBXTransformer::TransformMatrix( float4x4* pDestMatrix, const float4x4* pSrcMatrix ) const
//{
//	float4x4 SrcMatrix;
//	if( pSrcMatrix == pDestMatrix )
//	{
//		memcpy( &SrcMatrix, pSrcMatrix, sizeof( float4x4 ) );
//		pSrcMatrix = &SrcMatrix;
//	}
//	memcpy( pDestMatrix, pSrcMatrix, sizeof( float4x4 ) );
//
//	// What we're doing here is premultiplying by a left hand -> right hand matrix,
//	// and then postmultiplying by a right hand -> left hand matrix.
//	// The end result of those multiplications is that the third row and the third
//	// column are negated (so element _33 is left alone).  So instead of actually
//	// carrying out the multiplication, we just negate the 6 matrix elements.
//
//	pDestMatrix->M13 = -pSrcMatrix->M13;
//	pDestMatrix->M23 = -pSrcMatrix->M23;
//	pDestMatrix->M43 = -pSrcMatrix->M43;
//
//	pDestMatrix->M31 = -pSrcMatrix->M31;
//	pDestMatrix->M32 = -pSrcMatrix->M32;
//	pDestMatrix->M34 = -pSrcMatrix->M34;
//
//	// Apply the global unit scale to the translation components of the matrix.
//	pDestMatrix->M41 *= mUnitScale;
//	pDestMatrix->M42 *= mUnitScale;
//	pDestMatrix->M43 *= mUnitScale;
//}
//
//void FBXTransformer::TransformPosition( float3* pDestPosition, const float3* pSrcPosition ) const
//{
//	float3 SrcVector;
//	if( pSrcPosition == pDestPosition )
//	{
//		SrcVector = *pSrcPosition;
//		pSrcPosition = &SrcVector;
//	}
//
//	if( m3dMaxConversion )
//	{
//		pDestPosition->X() = pSrcPosition->X() * mUnitScale;
//		pDestPosition->Y() = pSrcPosition->Z() * mUnitScale;
//		pDestPosition->Z() = pSrcPosition->Y() * mUnitScale;
//	}
//	else
//	{
//		pDestPosition->X()= pSrcPosition->X() * mUnitScale;
//		pDestPosition->Y() = pSrcPosition->Y() * mUnitScale;
//		pDestPosition->Z() = -pSrcPosition->Z() * mUnitScale;
//	}
//}
//
//void FBXTransformer::TransformDirection( float3* pDestDirection, const float3* pSrcDirection ) const
//{
//	float3 SrcVector;
//	if( pSrcDirection == pDestDirection )
//	{
//		SrcVector = *pSrcDirection;
//		pSrcDirection = &SrcVector;
//	}
//
//	if( m3dMaxConversion )
//	{
//		pDestDirection->X() = pSrcDirection->X();
//		pDestDirection->Y() = pSrcDirection->Z();
//		pDestDirection->Z() = pSrcDirection->Y();
//	}
//	else
//	{
//		pDestDirection->X() = pSrcDirection->X();
//		pDestDirection->Y() = pSrcDirection->Y();
//		pDestDirection->Z() = -pSrcDirection->Z();
//	}
//}

//---------------------------------------------------------------------------------------------
FbxProcesser::FbxProcesser(void)
	: mFBXScene(nullptr), mFBXSdkManager(nullptr), mQuietMode(false), mMergeScene(false)
{
}

FbxProcesser::~FbxProcesser(void)
{
}

void FbxProcesser::Initialize()
{
	if (mFBXSdkManager)
		mFBXSdkManager->Destroy();

	mFBXSdkManager = FbxManager::Create();
	if( !mFBXSdkManager )
	{
		ExportLog::LogError("Error: Unable to create FBX Manager!");
		exit(1);
	}
	else
		ExportLog::LogMsg(0, "Autodesk FBX SDK version %s", mFBXSdkManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(mFBXSdkManager, IOSROOT);
	mFBXSdkManager->SetIOSettings(ios);

	mFBXScene = FbxScene::Create(mFBXSdkManager , "");  
	if( !mFBXScene )
	{
		ExportLog::LogError("Error: Unable to create FBX scene!");
		exit(1);
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

bool FbxProcesser::LoadScene( const String& filename )
{
	if (!mFBXSdkManager)
	{
		ExportLog::LogError("Create FBX Manager first!");
		return false;
	}

	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;
	bool bResult;

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);
	ExportLog::LogMsg(1, "FBX file format version for this FBX SDK is %d.%d.%d", lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(mFBXSdkManager,"");

	// Initialize the importer by providing a filename.
	bResult = lImporter->Initialize(filename.c_str(), -1, mFBXSdkManager->GetIOSettings());
	if( !bResult )
	{
		ExportLog::LogError("Call to FbxExporter::Initialize() failed.");
		//ExportLog::LogError("Error returned: %s\n\n", lImporter->GetLastErrorString());
		return false;
	}

	// Import the scene.
	bResult = lImporter->Import(mFBXScene);
	if( !bResult )
	{
		ExportLog::LogError( "Could not load FBX file \"%s\".", filename.c_str() );
		return false;
	}

	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);
	ExportLog::LogMsg(1, "FBX file format version for file '%s' is %d.%d.%d", filename.c_str(), lFileMajor, lFileMinor, lFileRevision);

	// Axis Conversion
	//mFBXTransformer.Initialize(mFBXScene);

	FbxAxisSystem engineAxisSystem = FbxAxisSystem::DirectX;
	if (g_ExportSettings.AxisSystem == Axis_OpenGL)
		engineAxisSystem = FbxAxisSystem::OpenGL;

	FbxAxisSystem sceneAxisSystem = mFBXScene->GetGlobalSettings().GetAxisSystem();
	if (sceneAxisSystem != engineAxisSystem)
	{
		engineAxisSystem.ConvertScene(mFBXScene);
	}

	// Destroy the importer.
	lImporter->Destroy();

	mOutputPath = PathUtil::GetPath(filename);
	mSceneName = PathUtil::GetFileName(filename);

	return bResult;
}

void FbxProcesser::ProcessScene( )
{
	CollectMaterials();
	CollectSkeletons();
	CollectMeshes();
	CollectAnimations();

	if (g_ExportSettings.MergeScene)
		MergeScene();

	if (g_ExportSettings.MergeWithSameMaterial)
		MergeSubMeshWithSameMaterial();
}

void FbxProcesser::RunCommand( const vector<String>& arguments )
{

}

void FbxProcesser::ProcessNode( FbxNode* pNode, FbxNodeAttribute::EType attriType )
{
	if( !pNode ) return;

	FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
	if (pNodeAttribute && pNodeAttribute->GetAttributeType() == attriType)
	{
		switch(pNodeAttribute->GetAttributeType())
		{
		case FbxNodeAttribute::eSkeleton:
			ProcessSkeleton(pNode);
			break;
		case FbxNodeAttribute::eMesh:
			ProcessMesh(pNode);
			break;
		case FbxNodeAttribute::eSubDiv:
			ProcessSubDiv(pNode);
			break;
		case FbxNodeAttribute::eCamera:
		case FbxNodeAttribute::eLight:
		default:
			break;
		};
	}

	for( int i = 0; i < pNode->GetChildCount(); ++i )
		ProcessNode(pNode->GetChild(i), attriType);
}

void FbxProcesser::ProcessSubDiv( FbxNode* pNode )
{
	FbxSubDiv* pSubDiv = pNode->GetSubdiv();

	if (!pSubDiv) return;
}

void FbxProcesser::ProcessMesh( FbxNode* pNode )
{
	FbxMesh* pMesh = pNode->GetMesh();

	if (!pMesh) return;

	ExportLog::LogMsg(0, "Process mesh: %s\n", pNode->GetName());

	if (!pMesh->IsTriangleMesh())
	{
		FbxGeometryConverter GeometryConverter(mFBXSdkManager);
		if( !GeometryConverter.TriangulateInPlace( pNode ) )
		{
			ExportLog::LogError("FbxMesh: %s triangulation failed!", pNode->GetName());
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
    const FbxLayerElementMaterial* materials = layer->GetMaterials();

	bool useDefaultMat = false;
	if (materials) 
	{
		FbxLayerElement::EMappingMode mm = materials->GetMappingMode();

		static const char *mm_str[] = { "eNONE", "eBY_CONTROL_POINT", "eBY_POLYGON_VERTEX", "eBY_POLYGON", "eBY_EDGE", "eALL_SAME" };
		
		if( !(mm == FbxLayerElement::eByPolygon || mm == FbxLayerElement::eAllSame) )
		{
			// only support eByPolygon and eAllSame
			ExportLog::LogError("Unsupported mapping mode: %s", ((mm <= FbxLayerElement::eAllSame) ? mm_str[mm] : "unknown"));
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
	if (g_ExportSettings.ExportSkeleton)  // Only process skin when ExportSkeleton is set
		lHasSkin = HasSkin(pMesh);
	
	// build mesh data
	shared_ptr<MeshData> mesh = std::make_shared<MeshData>();

	// read name
	mesh->Name = pNode->GetName();
	CorrectName(mesh->Name);

	std::vector<BoneWeights> meshBoneWeights;
	if (lHasSkin)
	{
		meshBoneWeights.resize(pMesh->GetControlPointsCount());
		// Deform the vertex array with the skin deformer.
		mesh->MeshSkeleton = ProcessBoneWeights(pMesh, meshBoneWeights);
	}

	float4x4 nodeGlobalTransform = matrixFromFbxAMatrix( pNode->EvaluateGlobalTransform() );

	for (size_t mi = 0; mi < polysByMaterial.size(); ++mi)
	{
		shared_ptr<MeshPartData> meshPart = std::make_shared<MeshPartData>();
		BoundingBoxf boundingBox;
		
		// mesh part name
		meshPart->Name = pNode->GetName();
		if (polysByMaterial.size() > 1)
			meshPart->Name += std::to_string(mi);

		// material name
		meshPart->MaterialName = useDefaultMat ? "DefaultMaterial" : pNode->GetMaterial(mi)->GetName();
		CorrectName(meshPart->MaterialName);

		uint32_t vertexFlag = 0;
		
		if (tangentRefMode != -1)
		{
			FbxSurfaceMaterial *material = pNode->GetMaterial(mi);
			
			if (material)
			{
				String filename;
				if (GetMaterialTexture(material, FbxSurfaceMaterial::sBump, &filename))
				{
					if (!filename.empty())
						vertexFlag |= Vertex::eTexcoord0| Vertex::eTangent;
				}
				
				if (GetMaterialTexture(material, FbxSurfaceMaterial::sNormalMap, &filename))
				{
					if (!filename.empty())
						vertexFlag |= Vertex::eTexcoord0| Vertex::eTangent;
				}
			}		
		}

		if (!useDefaultMat)
		{
			FbxSurfaceMaterial *material = pNode->GetMaterial(mi);

			if (material)
			{
				String filename;
				if (GetMaterialTexture(material, FbxSurfaceMaterial::sDiffuse, &filename))
				{
					if (!filename.empty())
						vertexFlag |= Vertex::eTexcoord0;
				}
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
				vertexFlag |= Vertex::ePosition;
				int ctrlPointIndex = pMesh->GetPolygonVertex(polyIndex , vi); 
				vertex.Position = float3FromFBX(pMesh->GetControlPointAt(ctrlPointIndex));
				vertex.Position = BakePosition(vertex.Position, nodeGlobalTransform);
				
				boundingBox.Merge(vertex.Position);

				if (vertexFlag & Vertex::eBlendWeight)
				{
					auto& boneWeights = meshBoneWeights[ctrlPointIndex];
					boneWeights.Normalize();

					for (auto& kv : boneWeights.GetBoneWeights())
					{
						vertex.BlendIndices.push_back(kv.first);
						vertex.BlendWeights.push_back(kv.second);
					}
				}

				// get normal
				FbxVector4 fbxNormal;
				if( pMesh->GetPolygonVertexNormal(polyIndex, vi, fbxNormal) )
				{
					vertex.Normal = float3FromFBX(fbxNormal);	
					vertex.Normal = BakeDirection(vertex.Normal, nodeGlobalTransform);
					vertex.Normal.Normalize();
					vertexFlag |= Vertex::eNormal;
				}
						
				for (size_t uv_idx = 0; uv_idx  < uvSets.size(); ++uv_idx)
				{
					FbxVector2 uv;
					bool hadUV;
					if( pMesh->GetPolygonVertexUV(polyIndex, vi, uvSets[uv_idx].c_str(), uv, hadUV) )
					{
						// flip the y-coordinate to match DirectX
						uv[1] = 1 - uv[1];

						if (uv_idx == 0) 
						{
							vertex.Tex0 = float2FromFBX(uv);
							vertexFlag |= Vertex::eTexcoord0;
						}
						else if (uv_idx == 1) 
						{
							vertex.Tex1 = float2FromFBX(uv);
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
						fbxTangent = layer->GetTangents()->GetDirectArray()[ctrlPointIndex + vi];
					else
					{
						int idx = layer->GetTangents()->GetIndexArray()[ctrlPointIndex + vi];
						fbxTangent = layer->GetTangents()->GetDirectArray()[idx];
					}
					vertexFlag |= Vertex::eTangent;

					if (binormalRefMode == FbxLayerElement::eDirect) 
						fbxBinormal = layer->GetBinormals()->GetDirectArray()[ctrlPointIndex + vi];
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

					vertex.Tangent = float3FromFBX(fbxTangent);
					vertex.Binormal = float3FromFBX(fbxBinormal);

					vertex.Tangent = BakeDirection(vertex.Tangent, nodeGlobalTransform);
					vertex.Tangent.Normalize();

					vertex.Binormal = BakeDirection(vertex.Binormal, nodeGlobalTransform);
					vertex.Binormal.Normalize();	
				}

				// test tangent
				/*if (vertexFlag & Vertex::eTangent)
				{
					float dot1 = Dot(vertex.Tangent, vertex.Binormal);
					float dot2 = Dot(vertex.Tangent, vertex.Normal);
					float dot3 = Dot(vertex.Binormal, vertex.Normal);
					printf("dot1=%f, dot2=%f, dot3=%f\n", dot1, dot2, dot3);
				}*/


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
					index = exitingIter->Index;

				meshPart->Indices.push_back(index);
			}
			meshPart->Bound = boundingBox;
		}
		mesh->Bound.Merge(meshPart->Bound);
		mesh->MeshParts.push_back(meshPart);
	}
	mSceneMeshes.push_back(mesh);
}

void FbxProcesser::ProcessSkeleton( FbxNode* pNode )
{
	FbxSkeleton* pFBXSkeleton = pNode->GetSkeleton();

	if (!pFBXSkeleton) return;

	FbxNode* skeletonRoot = GetBoneRoot(pNode);
	if (skeletonRoot)
	{
		if( mSkeletons.find(skeletonRoot->GetName()) == mSkeletons.end())
			mSkeletons[skeletonRoot->GetName()] = std::make_shared<Skeleton>();
	}

	shared_ptr<Skeleton> skeleton = mSkeletons[skeletonRoot->GetName()];

	Bone* parentBone = nullptr;
	
	FbxNode* pParentNode = pNode->GetParent();
	if (pParentNode)
		parentBone = skeleton->GetBone(pParentNode->GetName());

	printf("Bone: %s\n", pNode->GetName());
	skeleton->AddBone(pNode->GetName(), parentBone);
}

shared_ptr<Skeleton>  FbxProcesser::ProcessBoneWeights( FbxMesh* pMesh, std::vector<BoneWeights>& meshBoneWeights )
{
	shared_ptr<Skeleton> meshSkeleton = nullptr;

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

				// find which skeleton this mesh is skin to
				if (!meshSkeleton)
				{
					FbxNode* skeletonRoot = GetBoneRoot(pLinkNode);
					meshSkeleton = mSkeletons[skeletonRoot->GetName()];
					
					if (!meshSkeleton)
					{
						FBXSDK_printf("Mesh %s supposed to have a skeleton with root name %s, but not found!", pMesh->GetNode()->GetName(), skeletonRoot->GetName());
						assert(false);
					}
				}

				Bone* skeletonBone = meshSkeleton->GetBone(pLinkNode->GetName());
				Bone* parentBone = (Bone*)skeletonBone->GetParent();

				if (!skeletonBone)
					continue;

				FbxAMatrix matClusterTransformMatrix;
				FbxAMatrix matClusterLinkTransformMatrix;
				pFBXCluster->GetTransformMatrix(matClusterTransformMatrix);
				pFBXCluster->GetTransformLinkMatrix(matClusterLinkTransformMatrix);

				/*FbxAMatrix matInvBindPose = matClusterTransformMatrix * matClusterLinkTransformMatrix.Inverse();
				float4x4 matBindPose = MatrixFromFbxAMatrix(matInvBindPose.Inverse());*/

				float4x4 rcClusterTransformMatrix = matrixFromFbxAMatrix(matClusterTransformMatrix);
				float4x4 rcClusterLinkTransformMatrix = matrixFromFbxAMatrix(matClusterLinkTransformMatrix);
				float4x4 matBindPose = rcClusterLinkTransformMatrix * rcClusterTransformMatrix.Inverse();
	
				if (parentBone)
				{
					float4x4 paretTrans = parentBone->GetWorldTransform();
					matBindPose = matBindPose * paretTrans.Inverse();
				}

				float3 pos, scale;
				Quaternionf rot;
				MatrixDecompose(scale, rot, pos, matBindPose);

				skeletonBone->SetTransform(pos, rot, scale);
			
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

	return meshSkeleton;
}

void FbxProcesser::CollectMaterials( )
{ 
	ExportLog::LogMsg(2, "Collect Materials.");

	for( int i = 0; i < mFBXScene->GetMaterialCount(); ++i )
	{
		FbxSurfaceMaterial* pSurfaceMaterial = mFBXScene->GetMaterial(i);

		MaterialData matData;
		matData.Name = pSurfaceMaterial->GetName();
		std::replace(matData.Name.begin(), matData.Name.end(), ':', '_');

		float fSpecularPower = 1.0f;
		float fTransparency = 1.0f;

		FbxSurfaceLambert* pLambert = FbxCast<FbxSurfaceLambert>(pSurfaceMaterial);
		FbxSurfacePhong* pPhong = FbxCast<FbxSurfacePhong>(pSurfaceMaterial);

		if( pLambert )
		{
			matData.Ambient = ReadMaterialColor(pLambert->Ambient, pLambert->AmbientFactor);
			matData.Emissive = ReadMaterialColor(pLambert->Emissive, pLambert->EmissiveFactor);
			matData.Diffuse = ReadMaterialColor(pLambert->Diffuse, pLambert->DiffuseFactor);

			FbxPropertyT<FbxDouble> FBXTransparencyProperty = pLambert->TransparencyFactor;
			if( FBXTransparencyProperty.IsValid() )
				fTransparency = static_cast<float>( FBXTransparencyProperty.Get() );
		}

		if( pPhong )
		{
			matData.Specular = ReadMaterialColor(pPhong->Specular, pPhong->SpecularFactor);

			FbxPropertyT<FbxDouble> FBXSpecularPowerProperty = pPhong->Shininess;
			if( FBXSpecularPowerProperty.IsValid() )
				matData.Power = static_cast<float>( FBXSpecularPowerProperty.Get() );
		}

		FbxProperty pProperty;  
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
							 matData.Textures[FbxLayerElement::sTextureChannelNames[textureLayerIndex]] = filepath;
							 /*std::cout << FbxLayerElement::sTextureChannelNames[textureLayerIndex] << ": " << filepath << std::endl;*/
						}
						
					}  
				}  
			}  
		}  
		
		mMaterials.push_back(matData);
	}
}

void FbxProcesser::CollectSkeletons( )
{
	ExportLog::LogMsg(0, "Collect Skeletons.");
	ProcessNode(mFBXScene->GetRootNode(), FbxNodeAttribute::eSkeleton);
}

void FbxProcesser::CollectMeshes( )
{
	ExportLog::LogMsg(0, "Collect Meshes.");
	ProcessNode(mFBXScene->GetRootNode(), FbxNodeAttribute::eMesh);
}

void FbxProcesser::ExportMaterial()
{
	XMLDoc materialxml;

	XMLNodePtr rootNode = materialxml.AllocateNode(XML_Node_Element, "materials");
	materialxml.RootNode(rootNode);

	rootNode->AppendAttribute(materialxml.AllocateAttributeUInt("count", mMaterials.size()));

	XMLNodePtr matNode, tmpNode;
	XMLAttributePtr attrib;
	for (size_t i = 0; i < mMaterials.size(); ++i)
	{
		matNode = materialxml.AllocateNode(XML_Node_Element, "material");
		matNode->AppendAttribute(materialxml.AllocateAttributeString("material", mMaterials[i].Name));

		tmpNode = materialxml.AllocateNode(XML_Node_Element, "ambient");
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("r", mMaterials[i].Ambient[0]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("g", mMaterials[i].Ambient[1]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("b", mMaterials[i].Ambient[2]));
		matNode->AppendNode(tmpNode);

		tmpNode = materialxml.AllocateNode(XML_Node_Element, "diffuse");
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("r", mMaterials[i].Diffuse[0]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("g", mMaterials[i].Diffuse[1]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("b", mMaterials[i].Diffuse[2]));
		matNode->AppendNode(tmpNode);

		tmpNode = materialxml.AllocateNode(XML_Node_Element, "specular");
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("r", mMaterials[i].Specular[0]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("g", mMaterials[i].Specular[1]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("b", mMaterials[i].Specular[2]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("power", mMaterials[i].Power));
		matNode->AppendNode(tmpNode);

		tmpNode = materialxml.AllocateNode(XML_Node_Element, "emissive");
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("r", mMaterials[i].Emissive[0]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("g", mMaterials[i].Emissive[1]));
		tmpNode->AppendAttribute(materialxml.AllocateAttributeFloat("b", mMaterials[i].Emissive[2]));
		matNode->AppendNode(tmpNode);

		for (auto iter = mMaterials[i].Textures.begin(); iter != mMaterials[i].Textures.end(); ++iter)
		{
			tmpNode = materialxml.AllocateNode(XML_Node_Element, iter->first);
			tmpNode->AppendAttribute(materialxml.AllocateAttributeString("name", iter->second));
			matNode->AppendNode(tmpNode);
		}	

		rootNode->AppendNode(matNode);
	}

	std::ofstream xmlFile(mOutputPath + mSceneName + ".materials.xml");
	materialxml.Print(xmlFile);
	xmlFile.close();
}

void FbxProcesser::BuildAndSaveXML( )
{
	for (size_t mi = 0; mi < mSceneMeshes.size(); ++mi)
	{
		MeshData& mesh  = *(mSceneMeshes[mi]);

		if(!mQuietMode)
		{
			FBXSDK_printf("output xml mesh: %s.\n", mesh.Name.c_str());
		}

		XMLDoc meshxml;

		XMLNodePtr meshNode = meshxml.AllocateNode(XML_Node_Element, "mesh");
		meshxml.RootNode(meshNode);

		// mesh name
		meshNode->AppendAttribute(meshxml.AllocateAttributeString("name", mesh.Name));
		
		// mesh bounding
		XMLNodePtr meshBoundNode = meshxml.AllocateNode(XML_Node_Element, "bounding");

		XMLNodePtr meshBoundMinNode = meshxml.AllocateNode(XML_Node_Element, "min");
		meshBoundMinNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", mesh.Bound.Min.X()));
		meshBoundMinNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", mesh.Bound.Min.Y()));
		meshBoundMinNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", mesh.Bound.Min.Z()));
		meshBoundNode->AppendNode(meshBoundMinNode);

		XMLNodePtr meshBoundMaxNode = meshxml.AllocateNode(XML_Node_Element, "max");
		meshBoundMaxNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", mesh.Bound.Max.X()));
		meshBoundMaxNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", mesh.Bound.Max.Y()));
		meshBoundMaxNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", mesh.Bound.Max.Z()));
		meshBoundNode->AppendNode(meshBoundMaxNode);

		meshNode->AppendNode(meshBoundNode);

		// mesh skeleton
		if (mesh.MeshSkeleton)
		{
			XMLNodePtr meshSkeletonNode = meshxml.AllocateNode(XML_Node_Element, "skeleton");
			meshSkeletonNode->AppendAttribute(meshxml.AllocateAttributeUInt("boneCount", mesh.MeshSkeleton->GetBoneCount()));
			vector<Bone*>& bones = mesh.MeshSkeleton->GetBonesModified();
			for (size_t iBone = 0; iBone < bones.size(); ++iBone)
			{
				Bone* parentBone = static_cast<Bone*>(bones[iBone]->GetParent());

				XMLNodePtr boneNode = meshxml.AllocateNode(XML_Node_Element, "bone");
				boneNode->AppendAttribute(meshxml.AllocateAttributeString("name", bones[iBone]->GetName()));
				boneNode->AppendAttribute(meshxml.AllocateAttributeString("name", parentBone ? parentBone->GetName() : ""));
				meshSkeletonNode->AppendNode(boneNode);

				float3 pos = bones[iBone]->GetPosition();
				float3 scale = bones[iBone]->GetScale();
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
		}
		
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

			//  bounding
			XMLNodePtr boundNode = meshxml.AllocateNode(XML_Node_Element, "bounding");

			XMLNodePtr boundMinNode = meshxml.AllocateNode(XML_Node_Element, "min");
			boundMinNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", meshPart.Bound.Min.X()));
			boundMinNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", meshPart.Bound.Min.Y()));
			boundMinNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", meshPart.Bound.Min.Z()));
			boundNode->AppendNode(boundMinNode);

			XMLNodePtr boundMaxNode = meshxml.AllocateNode(XML_Node_Element, "max");
			boundMaxNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", meshPart.Bound.Max.X()));
			boundMaxNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", meshPart.Bound.Max.Y()));
			boundMaxNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", meshPart.Bound.Max.Z()));
			boundNode->AppendNode(boundMaxNode);

			meshPartNode->AppendNode(boundNode);

			XMLNodePtr verticesNode = meshxml.AllocateNode(XML_Node_Element, "vertices");
			XMLAttributePtr verticesCountAttr = meshxml.AllocateAttributeUInt("verticesCount", meshPart.Vertices.size());
			XMLAttributePtr vertexSizeAttr = meshxml.AllocateAttributeUInt("vertexSize", CalculateVertexSize(meshPart.Vertices[0].Flag));
			verticesNode->AppendAttribute(verticesCountAttr);
			verticesNode->AppendAttribute(vertexSizeAttr);		
			// add to mesh part
			meshPartNode->AppendNode(verticesNode);

			for(const Vertex& vertex : meshPart.Vertices)
			{
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

		if (mesh.MeshSkeleton)
		{
			auto animationIter = mAnimations.find(mesh.MeshSkeleton->GetRootBone()->GetName());

			if (animationIter != mAnimations.end())
			{
				AnimationData& animationData = animationIter->second;

				XMLNodePtr animationNode = meshxml.AllocateNode(XML_Node_Element, "animation");
				animationNode->AppendAttribute(meshxml.AllocateAttributeUInt("clipCount", animationData.AnimationClips.size()));
				meshNode->AppendNode(animationNode);

				for (auto iter = animationData.AnimationClips.begin(); iter != animationData.AnimationClips.end(); ++iter)
				{
					AnimationClipData& clipData = iter->second;

					XMLNodePtr clipNode = meshxml.AllocateNode(XML_Node_Element, "clip");
					clipNode->AppendAttribute(meshxml.AllocateAttributeString("name", iter->first));
					clipNode->AppendAttribute(meshxml.AllocateAttributeUInt("track", clipData.mAnimationTracks.size()));
					animationNode->AppendNode(clipNode);

					for (size_t iTrack = 0; iTrack < clipData.mAnimationTracks.size(); ++iTrack)
					{
						AnimationClipData::AnimationTrack& track =  clipData.mAnimationTracks[iTrack];

						XMLNodePtr trackNode = meshxml.AllocateNode(XML_Node_Element, "track");
						trackNode->AppendAttribute(meshxml.AllocateAttributeString("bone", track.Name));
						trackNode->AppendAttribute(meshxml.AllocateAttributeUInt("keys", track.KeyFrames.size()));
						clipNode->AppendNode(trackNode);

						for (size_t iKey = 0; iKey < track.KeyFrames.size(); ++iKey)
						{
							XMLNodePtr keyNode = meshxml.AllocateNode(XML_Node_Element, "keyframe");
							keyNode->AppendAttribute(meshxml.AllocateAttributeFloat("time", track.KeyFrames[iKey].Time));
							trackNode->AppendNode(keyNode);

							float3 pos = track.KeyFrames[iKey].Translation;
							float3 scale = track.KeyFrames[iKey].Scale;
							Quaternionf rot = track.KeyFrames[iKey].Rotation;

							XMLNodePtr posNode = meshxml.AllocateNode(XML_Node_Element, "position");
							posNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", pos.X()));
							posNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", pos.Y()));
							posNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", pos.Z()));
							keyNode->AppendNode(posNode);

							XMLNodePtr rotNode = meshxml.AllocateNode(XML_Node_Element, "rotation");
							rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("w", rot.W()));
							rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", rot.X()));
							rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", rot.Y()));
							rotNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", rot.Z()));
							keyNode->AppendNode(rotNode);

							XMLNodePtr scaleNode = meshxml.AllocateNode(XML_Node_Element, "scale");
							scaleNode->AppendAttribute(meshxml.AllocateAttributeFloat("x", scale.X()));
							scaleNode->AppendAttribute(meshxml.AllocateAttributeFloat("y", scale.Y()));
							scaleNode->AppendAttribute(meshxml.AllocateAttributeFloat("z", scale.Z()));
							keyNode->AppendNode(scaleNode);
						}
					}
				}
			}
		}

		std::ofstream xmlFile(mesh.Name + ".mesh.xml");
		meshxml.Print(xmlFile);
		xmlFile.close();
	}	

	ExportMaterial();
}

void FbxProcesser::BuildAndSaveBinary( )
{
	const uint32_t MeshId = ('M' << 24) | ('E' << 16) | ('S' << 8) | ('H');

	for (size_t mi = 0; mi < mSceneMeshes.size(); ++mi)
	{
		MeshData& mesh  = *(mSceneMeshes[mi]);

		FileStream stream;
		stream.Open(mOutputPath + mSceneName + ".mesh", FILE_WRITE);
		ExportLog::LogMsg(0, "Build mesh: %s\n", mesh.Name.c_str());

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
				for (size_t i = 0; i < meshPart.Indices.size(); ++i)
					stream.WriteUShort(meshPart.Indices[i]);
			}
			else
			{
				stream.WriteUInt(IBT_Bit32);
				stream.Write(&(meshPart.Indices[0]), sizeof(char) * meshPart.Indices.size());
			}
		}

		if (g_ExportSettings.ExportSkeleton && mesh.MeshSkeleton && mesh.MeshSkeleton->GetBoneCount())
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
			const String& boneRootName = mesh.MeshSkeleton->GetRootBone()->GetName();
			if (g_ExportSettings.ExportAnimation && mAnimations.count(boneRootName))
			{
				AnimationData& animationData = mAnimations[boneRootName];

				stream.WriteUInt(animationData.AnimationClips.size());

				// animtion clip are write to each animation file
				for (const auto& kv : animationData.AnimationClips)
				{
					String clipName = kv.first + ".anim";
					const AnimationClipData& clip = kv.second;

					// write clip file in mesh 
					stream.WriteString(clipName);

					FileStream clipStream;
					clipStream.Open(mOutputPath + clipName, FILE_WRITE);

					clipStream.WriteString(kv.first);
					clipStream.WriteFloat(clip.Duration);
					clipStream.WriteUInt(clip.mAnimationTracks.size());

					for (const AnimationClipData::AnimationTrack& track : clip.mAnimationTracks)
					{
						// write track name
						clipStream.WriteString(track.Name);
						// write track key frame count
						clipStream.WriteUInt(track.KeyFrames.size());

						for (const AnimationClipData::KeyFrame& key : track.KeyFrames)
						{
							// write key time
							clipStream.WriteFloat(key.Time);
							clipStream.Write(&key.Translation, sizeof(float3));
							clipStream.Write(&key.Rotation, sizeof(Quaternionf));
							clipStream.Write(&key.Scale, sizeof(float3));
						}
					}

					clipStream.Close();
				}
			}
			else
			{
				stream.WriteUInt(0);  // No Animations
			}
		}
		else
		{ 
			stream.WriteUInt(0);	// No Skeleton
		}
	
		stream.Close();
	}
}

void FbxProcesser::CollectAnimations( )
{
	FbxNode* pRootNode = mFBXScene->GetRootNode();
	
	if (!pRootNode) return;

	double frameRate = FbxTime::GetFrameRate(mFBXScene->GetGlobalSettings().GetTimeMode());

	for (int i = 0; i < mFBXScene->GetSrcObjectCount<FbxAnimStack>(); i++)
	{
		FbxAnimStack* lAnimStack = mFBXScene->GetSrcObject<FbxAnimStack>(i);
		
		FbxString takeName = lAnimStack->GetName();

		if (takeName != FbxString("Default"))
		{
			FbxTakeInfo* lCurrentTakeInfo = mFBXScene->GetTakeInfo(takeName);

			mFBXScene->ActiveAnimStackName.Set(takeName);

			FbxTime KStart, KStop;

			if (lCurrentTakeInfo)
			{
				KStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
				KStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
			}
			else
			{
				// Take the time line value
				FbxTimeSpan lTimeLineTimeSpan;
				mFBXScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);

				KStart = lTimeLineTimeSpan.GetStart();
				KStop  = lTimeLineTimeSpan.GetStop();
			}

			double fStart = KStart.GetSecondDouble();
			double fStop = KStop.GetSecondDouble();

			//int num_frames = (int)(lTimeLineTimeSpan.GetDuration().GetMilliSeconds() * frameRate / 1000 + 0.5f);

			if( fStart < fStop )
				ProcessAnimation(lAnimStack, pRootNode, frameRate, fStart, fStop);
		}
	}
}

void FbxProcesser::ProcessAnimation( FbxAnimStack* pStack, FbxNode* pNode, double fFrameRate, double fStart, double fStop )
{
	FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
	if (pNodeAttribute && pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		FbxNode* skeletonRoot = GetBoneRoot(pNode);
			
		AnimationClipData& clip = mAnimations[skeletonRoot->GetName()].AnimationClips[pStack->GetName()];
		shared_ptr<Skeleton> skeleton = mSkeletons[skeletonRoot->GetName()];
				
		if( skeleton )
		{
			Bone* pBone = skeleton->GetBone(pNode->GetName());	
				
			if( pBone )
			{	
				AnimationClipData::AnimationTrack track;
				track.Name = pNode->GetName();

				double fTime = 0;
				while( fTime <= fStop )
				{
					FbxTime takeTime;
					takeTime.SetSecondDouble(fTime);

					FbxAMatrix offset = GetGeometry(pNode);

					FbxAMatrix matAbsoluteTransform = GetGlobalPosition(pNode, takeTime);
					FbxAMatrix matParentAbsoluteTransform = GetGlobalPosition(pNode->GetParent(), takeTime);
					FbxAMatrix matLocalTrasform =  matParentAbsoluteTransform.Inverse() * matAbsoluteTransform;

					AnimationClipData::KeyFrame keyframe;

					FbxQuaternion quat = matLocalTrasform.GetQ();
					FbxVector4 trans = matLocalTrasform.GetT();
					FbxVector4 scale = matLocalTrasform.GetS();
					keyframe.Rotation = quatFromFBX(quat);
					keyframe.Scale = float3FromFBX(scale);
					keyframe.Translation = float3FromFBX(trans);

					// Handness
					/*float4x4 transform = matrixFromFbxAMatrix(matLocalTrasform);
					mFBXTransformer.TransformMatrix(&transform, &transform);
					MatrixDecompose(keyframe.Scale, keyframe.Rotation, keyframe.Translation, transform);*/
			
					keyframe.Time = (float)fTime;
					track.KeyFrames.push_back(keyframe);

					fTime += 1.0f/fFrameRate;
				}

				clip.Duration = (float)(fTime-1.0f/fFrameRate);
				clip.mAnimationTracks.push_back(track);
			}	
			else 
				ExportLog::LogWarning("Bone: %s not found", pNode->GetName());
		}
	}

	for( int i = 0; i < pNode->GetChildCount(); ++i )
		ProcessAnimation(pStack, pNode->GetChild(i), fFrameRate, fStart, fStop);
}

void FbxProcesser::MergeScene()
{
	if (mSceneMeshes.size() > 1)
	{
		if (mSkeletons.size() && g_ExportSettings.ExportSkeleton)
		{
			ExportLog::LogWarning("Found mesh with skeleton, can't merge!");
			return;
		}

		// Make other mesh as a mesh part of first mesh
		for (size_t mi = 1; mi < mSceneMeshes.size(); ++mi)
		{
			MeshData& mesh  = *(mSceneMeshes[mi]);

			mSceneMeshes[0]->Bound.Merge(mesh.Bound);

			for (auto& part : mesh.MeshParts)
				mSceneMeshes[0]->MeshParts.push_back(part);
		}

		mSceneMeshes.resize(1);	
	}	
}

void FbxProcesser::MergeSubMeshWithSameMaterial()
{
	for (size_t mi = 0; mi < mSceneMeshes.size(); ++mi)
	{
		MeshData& mesh  = *(mSceneMeshes[mi]);

		std::vector<shared_ptr<MeshPartData>> mergedList;

		shared_ptr<MeshPartData> subMesh;

		bool merging = false;
		if (mesh.MeshParts.size())
		{
			subMesh = mesh.MeshParts.back();
			mesh.MeshParts.pop_back();
			merging = true;
		}

		while (merging)
		{
			auto it = mesh.MeshParts.begin();
			while (it != mesh.MeshParts.end())
			{
				shared_ptr<MeshPartData> testMesh = *it;

				bool canMerge = false;
				
				// Must have same material
				canMerge = (subMesh->MaterialName == testMesh->MaterialName); 
				canMerge &= (subMesh->Indices.size() + testMesh->Indices.size() < UINT_MAX);
				canMerge &= (subMesh->VertexDecl == testMesh->VertexDecl);

				if (canMerge)
				{
					size_t baseIndex = subMesh->Vertices.size();

					subMesh->Vertices.reserve(subMesh->Vertices.size() + testMesh->Vertices.size());
					subMesh->Indices.reserve(testMesh->Indices.size() + testMesh->Indices.size());
					for (Vertex& vertex : testMesh->Vertices)
					{
						vertex.Index += baseIndex;
						subMesh->Vertices.push_back(vertex);
					}

					for (const uint32_t& index : testMesh->Indices)
					{
						subMesh->Indices.push_back(baseIndex + index);
					}

					subMesh->Bound.Merge(testMesh->Bound);

					it = mesh.MeshParts.erase(it);
				}
				else 
					++it;
			}

			mergedList.push_back(subMesh);

			if (mesh.MeshParts.size())
			{
				subMesh = mesh.MeshParts.back();
				mesh.MeshParts.pop_back();
			}
			else
			{
				// finished
				merging = false;
			}
		}

		mesh.MeshParts.swap(mergedList);
	}

}

int main()
{
	ExportLog::AddListener( &g_ConsoleOutListener );
#if _MSC_VER >= 1500
	if( IsDebuggerPresent() )
		ExportLog::AddListener( &g_DebugSpewListener );
#endif

	ExportLog::SetLogLevel( 1 );
	ExportLog::EnableLogging( TRUE );

	FbxProcesser fbxProcesser;
	fbxProcesser.Initialize();

	g_ExportSettings.ExportSkeleton = true;
    g_ExportSettings.MergeScene = true;
	g_ExportSettings.AxisSystem = Axis_OpenGL;
	g_ExportSettings.MergeWithSameMaterial = false;

	if (fbxProcesser.LoadScene("E:/Engines/RcEngine/Media/Mesh/Dude/dude.fbx"))
	{
		fbxProcesser.ProcessScene();
		//fbxProcesser.BuildAndSaveXML();
		fbxProcesser.BuildAndSaveBinary();
		fbxProcesser.ExportMaterial();
	}

	return 0;
}
