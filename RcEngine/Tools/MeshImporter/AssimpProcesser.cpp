#include "AssimpProcesser.h"
#include "Graphics/VertexDeclaration.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/Skeleton.h"
#include "Math/ColorRGBA.h"
#include "Math/Matrix.h"
#include "Math/MathUtil.h"
#include "Graphics/Mesh.h"
#include "MainApp/Application.h"
#include "Core/Exception.h"
#include "IO/FileStream.h"
#include "Core/XMLDom.h"
#include "MaterialData.h"
#include "MeshData.h"

#pragma comment(lib, "assimp.lib")


struct OutModel
{
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
	shared_ptr<Skeleton> Skeleton;
};


Vector3f Transform(const Vector3f& vec, const Matrix4f& mat)
{
	Vector4f vec4(vec.X(), vec.Y(), vec.Z(), 1.0f);
	Vector4f transformed = vec4 * mat;
	return Vector3f(transformed.X(), transformed.Y(), transformed.Z());
}



// Convert aiMatrix to RcEngine matrix, note that assimp 
// assume the right handed coordinate system, so aiMatrix 
// is a right-handed matrix.You need to transpose it to get
// a left-handed matrix.
Matrix4f FromAIMatrix(const aiMatrix4x4& in)
{
	Matrix4f out;
	out.M11 = in.a1;
	out.M12 = in.b1;
	out.M13 = in.c1;
	out.M14 = in.d1;
	
	out.M21 = in.a2;
	out.M22 = in.b2;
	out.M23 = in.c2;
	out.M24 = in.d2;
	
	out.M31 = in.a3;
	out.M32 = in.b3;
	out.M33 = in.c3;
	out.M34 = in.d3;
	
	out.M41 = in.a4;
	out.M42 = in.b4;
	out.M43 = in.c4;
	out.M44 = in.d4;

	return out;
}

Vector3f FromAIVector(const aiVector3D& vec)
{
	 return Vector3f(vec.x, vec.y, vec.z);
}

void GetPosRotScale(const aiMatrix4x4& transform, Vector3f& pos, Quaternionf& rot, Vector3f& scale)
{
	aiVector3D aiPos;
	aiQuaternion aiRot;
	aiVector3D aiScale;
	transform.Decompose(aiScale, aiRot, aiPos);
	pos = FromAIVector(aiPos);
	rot.X() = aiRot.x; rot.Y() = aiRot.y;rot.Z() = aiRot.z;rot.W() = aiRot.w;
	scale = FromAIVector(aiScale);
}


aiNode* GetNode(const String& name, aiNode* node)
{
	if (!node)
		return nullptr;
	
	if (String(node->mName.C_Str()) == name)
	{
		return node;
	}

	for (uint32_t i = 0; i < node->mNumChildren; ++i)
	{
		aiNode* found = GetNode(name, node->mChildren[i]);
		if (found)
		{
			return found;
		}
	}

	return nullptr;
}

uint32_t GetBoneIndex(const OutModel& model, const aiString& boneName)
{
	for (size_t i = 0; i < model.Bones.size(); ++i)
	{
		if (boneName == model.Bones[i]->mName)
		{
			return i;
		}
	}
	return (numeric_limits<uint32_t>::max)();
}

aiMatrix4x4 GetDerivedTransform(aiNode* node, aiNode* rootNode, aiMatrix4x4 transform)
{
	while(node && node != rootNode)
	{
		node = node->mParent;
		if (node)
			transform = node->mTransformation * transform;
		
	}
	return transform;
}

aiMatrix4x4 GetMeshBakingTransform(aiNode* meshNode, aiNode* meshRootNode)
{
	if (meshNode == meshRootNode)
		return aiMatrix4x4();
	else
		return GetDerivedTransform(meshNode, meshRootNode, meshNode->mTransformation);
}

aiMatrix4x4 GetOffsetMatrix(const OutModel& model, const String& boneName)
{
	for (size_t i = 0; i < model.Meshes.size(); ++i)
	{
		aiMesh* mesh = model.Meshes[i];
		aiNode* meshNode = model.MeshNodes[i];

		for (size_t j = 0; j < mesh->mNumBones; ++j)
		{
			aiBone* bone = mesh->mBones[j];
			if (String(bone->mName.C_Str()) == boneName)
			{
				aiMatrix4x4 offset = bone->mOffsetMatrix;

				/* Note that the all mesh vertex has been baked into the same coordinate system which
				 * is defined by the root node, called model space. So offset matrix must first transform
				 * vertex from model space to mesh space, then transform mesh space to bone space in bind pose
				 * that why we multiply the nodeDerivedInverse matrix.If you use left-handed, change it correspondingli 
				 * V(bone) = offset * nodeDerivedInverse * V(model),
				 */
				aiMatrix4x4 nodeDerivedInverse = GetMeshBakingTransform(meshNode, model.RootNode);
				nodeDerivedInverse.Inverse();
				offset *= nodeDerivedInverse;
				return offset;
			}
		}
	}
	return aiMatrix4x4();
}

shared_ptr<VertexDeclaration> GetVertexDeclaration(aiMesh* mesh)
{
	vector<VertexElement> vertexElements;
	unsigned int offset = 0;
	if (mesh->HasPositions())
	{
		vertexElements.push_back(VertexElement(offset, VEF_Vector3, VEU_Position, 0));
		offset += VertexElement::GetTypeSize(VEF_Vector3);
	}

	if (mesh->HasNormals())
	{
		vertexElements.push_back(VertexElement(offset, VEF_Vector3, VEU_Normal, 0));
		offset += VertexElement::GetTypeSize(VEF_Vector3);
	}

	if (mesh->HasTangentsAndBitangents())
	{
		vertexElements.push_back(VertexElement(offset, VEF_Vector3, VEU_Tangent, 0));
		offset += VertexElement::GetTypeSize(VEF_Vector3);

		vertexElements.push_back(VertexElement(offset, VEF_Vector3, VEU_Binormal, 0));
		offset += VertexElement::GetTypeSize(VEF_Vector3);
	}

	for (unsigned int i = 0; i < mesh->GetNumUVChannels(); ++i)
	{
		switch (mesh->mNumUVComponents[i])
		{
		case 1:
			vertexElements.push_back(VertexElement(offset, VEF_Single, VEU_TextureCoordinate, i));
			offset += VertexElement::GetTypeSize(VEF_Single);
			break;
		case 2:
			vertexElements.push_back(VertexElement(offset, VEF_Vector2, VEU_TextureCoordinate, i));
			offset += VertexElement::GetTypeSize(VEF_Vector2);
			break;
		case 3:
			vertexElements.push_back(VertexElement(offset, VEF_Vector3, VEU_TextureCoordinate, i));
			offset += VertexElement::GetTypeSize(VEF_Vector3);
			break;
		}
	}

	if (mesh->HasBones())
	{
		vertexElements.push_back(VertexElement(offset, VEF_Vector4, VEU_BlendWeight, 0));
		offset += VertexElement::GetTypeSize(VEF_Vector4);

		vertexElements.push_back(VertexElement(offset, VEF_UByte4, VEU_BlendIndices, 0));
		offset += VertexElement::GetTypeSize(VEF_UByte4);
	}

	shared_ptr<VertexDeclaration> vd ( new VertexDeclaration(vertexElements) );
	assert(vd->GetVertexSize() == offset );
	return vd;
}

void GetBlendData(OutModel& model, aiMesh* mesh, vector<uint32_t>& boneMappings, vector<vector<uint8_t> >&
	blendIndices, vector<vector<float> >& blendWeights)
{
	blendIndices.resize(mesh->mNumVertices);
	blendWeights.resize(mesh->mNumVertices);

	for (size_t i = 0; i < mesh->mNumBones; ++i)
	{
		aiBone* bone = mesh->mBones[i];
		uint32_t boneIndex = GetBoneIndex(model, bone->mName);
		if (boneIndex == (numeric_limits<uint32_t>::max)())
		{
			//ErrorExit("Bone " + boneName + " not found");
			continue;
		}
		
		for (size_t j = 0; j < bone->mNumWeights; ++j)
		{
			uint32_t vertexID = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;
			blendIndices[vertexID].push_back(boneIndex);
			blendWeights[vertexID].push_back(weight);
			if (blendWeights[vertexID].size() > 4)
			{
				//ErrorExit("More than 4 bone influences on vertex");
			}
		}
		
	}
}

void PrintLine(const String& str)
{
	cout << str << endl;
}

AssimpProcesser::AssimpProcesser(void)
{
}


AssimpProcesser::~AssimpProcesser(void)
{
}

bool AssimpProcesser::Process( const char* filePath )
{
	Assimp::Importer importer;
	
	mAIScene = const_cast<aiScene*>( importer.ReadFile(filePath, aiProcess_Triangulate |
		aiProcess_RemoveRedundantMaterials | aiProcess_OptimizeMeshes | aiProcess_FlipUVs) );
	
	if(!mAIScene)
	{
		// Error
		std::cerr <<  importer.GetErrorString() << std::endl;
		return false;
	}

	ProcessScene(mAIScene);

	return true;
}

void AssimpProcesser::ProcessScene( const aiScene* scene )
{
	OutModel model;
	model.RootNode = mAIScene->mRootNode;
	ExportModel(model, "Test.mdl");
	ExportBinary(model);
	ExportXML(model);
}

shared_ptr<MeshPartData> AssimpProcesser::ProcessMeshPart( aiMesh* mesh )
{
	if (!mesh)
	{
		return nullptr;
	}

	shared_ptr<MeshPartData> meshPart( new MeshPartData() );

	meshPart->Name = std::string(mesh->mName.C_Str());
	meshPart->MaterialID = mesh->mMaterialIndex;

	// process faces
	uint16_t faceCount = mesh->mNumFaces;
	meshPart->StartIndex = 0;
	meshPart->IndexCount = mesh->mNumFaces * 3;		// only support triangle
	
	if ( meshPart->IndexCount < (std::numeric_limits<uint16_t>::max)() )
	{
		meshPart->IndexFormat = IBT_Bit16;
	}
	else
	{
		meshPart->IndexFormat = IBT_Bit32;
	}
	
	meshPart->IndexData.resize(meshPart->IndexCount);
	
	for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
	{
		aiFace face = mesh->mFaces[f];
		assert(face.mNumIndices == 3);
		meshPart->IndexData[ f*3 + 0] = face.mIndices[0];
		meshPart->IndexData[ f*3 + 1] = face.mIndices[1];
		meshPart->IndexData[ f*3 + 2] = face.mIndices[2];
	}

	bool bones = mesh->HasBones();

	// Process vertices
	vector<VertexElement> vertexElements;
	unsigned int offset = 0;
	if (mesh->HasPositions())
	{
		vertexElements.push_back(VertexElement(offset, VEF_Vector3, VEU_Position, 0));
		offset += VertexElement::GetTypeSize(VEF_Vector3);
	}

	if (mesh->HasNormals())
	{
		vertexElements.push_back(VertexElement(offset, VEF_Vector3, VEU_Normal, 0));
		offset += VertexElement::GetTypeSize(VEF_Vector3);
	}

	if (mesh->HasTangentsAndBitangents())
	{
		vertexElements.push_back(VertexElement(offset, VEF_Vector3, VEU_Tangent, 0));
		offset += VertexElement::GetTypeSize(VEF_Vector3);

		vertexElements.push_back(VertexElement(offset, VEF_Vector3, VEU_Binormal, 0));
		offset += VertexElement::GetTypeSize(VEF_Vector3);
	}

	for (unsigned int i = 0; i < mesh->GetNumUVChannels(); ++i)
	{
		switch (mesh->mNumUVComponents[i])
		{
		case 1:
			vertexElements.push_back(VertexElement(offset, VEF_Single, VEU_TextureCoordinate, i));
			offset += VertexElement::GetTypeSize(VEF_Single);
			break;
		case 2:
			vertexElements.push_back(VertexElement(offset, VEF_Vector2, VEU_TextureCoordinate, i));
			offset += VertexElement::GetTypeSize(VEF_Vector2);
			break;
		case 3:
			vertexElements.push_back(VertexElement(offset, VEF_Vector3, VEU_TextureCoordinate, i));
			offset += VertexElement::GetTypeSize(VEF_Vector3);
			break;
		}
	}

	unsigned int vertexSize = offset;
	
	// bounding sphere
	meshPart->BoundingSphere.mCenter = Vector3f(0.0f, 0.0f, 0.0f);
	meshPart->BoundingSphere.mRadius = 0.0f;

	meshPart->VertexCount = mesh->mNumVertices;
	meshPart->VertexData.resize(vertexSize * mesh->mNumVertices);

	
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		int baseOffset = vertexSize * i;
		for (size_t ve = 0; ve < vertexElements.size(); ++ve)
		{
			const VertexElement& element = vertexElements[ve];
			float* vertexPtr = (float*)(&meshPart->VertexData[0] + baseOffset + element.Offset);
			switch(element.Usage)
			{
			case VEU_Position:
				{
					aiVector3D v = mesh->mVertices[i];
					*(vertexPtr) = mesh->mVertices[i].x;
					*(vertexPtr+1) = mesh->mVertices[i].y;
					*(vertexPtr+2) = mesh->mVertices[i].z;

					// bouding sphere
				}	
				break;
			case VEU_Normal:
				{
					*(vertexPtr) = mesh->mNormals[i].x;
					*(vertexPtr+1) = mesh->mNormals[i].y;
					*(vertexPtr+2) = mesh->mNormals[i].z;
				}
				break;
			case VEU_Tangent:
				{
					*(vertexPtr) = mesh->mTangents[i].x;
					*(vertexPtr+1) = mesh->mTangents[i].y;
					*(vertexPtr+2) = mesh->mTangents[i].z;
				}			
				break;
			case VEU_Binormal:
				{
					*(vertexPtr) = mesh->mBitangents[i].x;
					*(vertexPtr+1) = mesh->mBitangents[i].y;
					*(vertexPtr+2) = mesh->mBitangents[i].z;
				}			
				break;
			case VEU_TextureCoordinate:
				{
					switch(mesh->mNumUVComponents[element.UsageIndex])
					{
					case 3:
						*(vertexPtr+2) = mesh->mTextureCoords[element.UsageIndex][i].z;
					case 2:
						*(vertexPtr+1) = mesh->mTextureCoords[element.UsageIndex][i].y;	
					case 1:
						*(vertexPtr) = mesh->mTextureCoords[element.UsageIndex][i].x;
						break;
					}
				}
			case VEU_BlendIndices:
				break;
			case VEU_BlendWeight:
				break; 

			default:
				break;
			}
		}
	}

	return meshPart;
}

shared_ptr<MaterialData> AssimpProcesser::ProcessMaterial( aiMaterial* material )
{
	if (!material)
	{
		return nullptr;
	}

	shared_ptr<MaterialData> materialData( new MaterialData );

	for (unsigned int i = 0; i < material->mNumProperties; ++i)
	{
		aiMaterialProperty* prop = material->mProperties[i];

		if (prop->mKey == aiString("?mat.name"))
		{
			aiString name;
			material->Get(AI_MATKEY_NAME,name);
			materialData->Name = string(name.C_Str());
		}
		else if (prop->mKey == aiString("$clr.ambient"))
		{
			aiColor3D color (0.f,0.f,0.f);
			material->Get(AI_MATKEY_COLOR_AMBIENT,color);
			materialData->Ambient = ColorRGBA(color.r, color.g, color.b, 1.0f);
		}
		else if (prop->mKey == aiString("$clr.diffuse"))
		{
			aiColor3D color (0.f,0.f,0.f);
			material->Get(AI_MATKEY_COLOR_DIFFUSE,color);
			materialData->Diffuse = ColorRGBA(color.r, color.g, color.b, 1.0f);
		}
		else if (prop->mKey == aiString("$clr.specular"))
		{
			aiColor3D color (0.f,0.f,0.f);
			material->Get(AI_MATKEY_COLOR_SPECULAR,color);
			materialData->Diffuse = ColorRGBA(color.r, color.g, color.b, 1.0f);
		}
		else if (prop->mKey == aiString("$mat.shininess"))
		{
			float shininess;
			material->Get(AI_MATKEY_SHININESS,shininess);
			materialData->Power = shininess;
		}
		else if (prop->mKey == aiString("$tex.file"))
		{
			switch(prop->mSemantic)
			{
			case aiTextureType_AMBIENT:
				{
					aiString file;
					material->GetTexture(aiTextureType_AMBIENT, 0, &file);
					materialData->Textures.insert(make_pair(string("AmbientMap"), string(file.C_Str())));
				}
			case aiTextureType_DIFFUSE:
				{
					aiString file;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &file);
					materialData->Textures.insert(make_pair(String("DiffuseMap"), String(file.C_Str())));
				}
				break;
			case aiTextureType_SPECULAR:
				{
					aiString file;
					material->GetTexture(aiTextureType_SPECULAR, 0, &file);
					materialData->Textures.insert(make_pair(string("SpecularMap"), string(file.C_Str())));
				}
				break;
			case aiTextureType_NORMALS:
				{
					aiString file;
					material->GetTexture(aiTextureType_NORMALS, 0, &file);
					materialData->Textures.insert(make_pair(string("NormalMap"), string(file.C_Str())));
				}
				break;
			case aiTextureType_DISPLACEMENT:
				{
					aiString file;
					material->GetTexture(aiTextureType_DISPLACEMENT, 0, &file);
					materialData->Textures.insert(make_pair(string("DisplacementMap"), string(file.C_Str())));
				}
				break;
			default:
				{

				}
			}
		}
	}

	return materialData;
}

void AssimpProcesser::ExportXML(  OutModel& outModel )
{
	ofstream file("newTest.xml");
	vector<shared_ptr<MeshPartData> >& subMeshes = outModel.MeshParts;
	for (size_t i = 0; i < subMeshes.size(); ++i)
	{
		shared_ptr<MeshPartData> submesh = subMeshes[i];
		
		file << "<subMesh>" << endl;

		// write each vertex element
		const std::vector<VertexElement>& elements = submesh->VertexDeclaration->GetElements();
		file << "\t<vertice vertexCount=\"" << submesh->VertexCount << "\" vertexSize=\"" << submesh->VertexDeclaration->GetVertexSize()<<"\"/>\n";

		for (size_t i = 0; i < submesh->VertexCount; ++i)
		{
			file << "\t\t<vertex>\n";
			int baseOffset = submesh->VertexDeclaration->GetVertexSize() * i;
			for (auto iter = elements.begin(); iter != elements.end(); ++iter)
			{
				const VertexElement& element = *iter;
				float* vertexPtr = (float*)(&submesh->VertexData[0] + baseOffset + element.Offset);
				switch(element.Usage)
				{
				case VEU_Position:
					{
						file << "\t\t\t<position x=\"" << vertexPtr[0] << "\" y=\"" << vertexPtr[1] << "\" z=\"" << vertexPtr[2] << "\"/>\n";
					}	
					break;
				case VEU_Normal:
					{
						file << "\t\t\t<normal x=\"" << vertexPtr[0] << "\" y=\"" << vertexPtr[1] << "\" z=\"" << vertexPtr[2] << "\"/>\n";
					}
					break;
				case VEU_Tangent:
					{
						file << "\t\t\t<tangent x=\"" << vertexPtr[0] << "\" y=\"" << vertexPtr[1] << "\" z=\"" << vertexPtr[2] << "\"/>\n";
					}			
					break;
				case VEU_Binormal:
					{
						file << "\t\t\t<binormal x=\"" << vertexPtr[0] << "\" y=\"" << vertexPtr[1] << "\" z=\"" << vertexPtr[2] << "\"/>\n";
					}			
					break;
				case VEU_TextureCoordinate:
					{
						switch(VertexElement::GetTypeCount(element.Type))
						{
						case 3:
							file << "\t\t\t<texcoord u=\"" << vertexPtr[0] << "\" v=\"" << vertexPtr[1] << "\" r=\"" << vertexPtr[2] << "\"/>\n";
							break;
						case 2:
							file << "\t\t\t<texcoord u=\"" << vertexPtr[0] << "\" v=\"" << vertexPtr[1] << "\"/>\n";
							break;
						case 1:
							file << "\t\t\t<texcoord u=\"" << vertexPtr[0] << "\"/>\n";
							break;
						}
					}
					break;
				case VEU_BlendWeight:
					{
						float* weightPtr = vertexPtr;
						uint8_t* indexPtr = (uint8_t*)(vertexPtr + 4);
						for (size_t k = 0; k < 4; ++k)
						{
							file << "\t\t\t<bone weight=\"" << weightPtr[k] << "\" index=\"" << (uint16_t)indexPtr[k] << "\"/>\n";
						}	
					}
					break;	
				default:
					break;
				}
			}
			file << "\t\t</vertex>\n";
		}
		file << "\t<vertices/>\n";

		file << "\t<triangles triangleCount=\""<<submesh->IndexCount/3 << "\">\n";
		
		uint32_t offset = 0;
		for (size_t i = 0; i < submesh->IndexCount / 3; i+=3)
		{
			
			if (submesh->IndexFormat == IBT_Bit32)
			{
				uint32_t* idx = (uint32_t*)( &submesh->IndexData[0] + offset);
				offset += sizeof(uint32_t) * 3;
				file << "\t\t<triangle a=\"" << idx[0] << "\" b=\"" << idx[1] << "\" c=\"" << idx[2] << "\"/>\n";
			}else
			{
				uint16_t* idx = (uint16_t*)( &submesh->IndexData[0] + offset);
				offset += sizeof(uint16_t) * 3;
				file << "\t\t<triangle a=\"" << idx[0] << "\" b=\"" << idx[1] << "\" c=\"" << idx[2] << "\"/>\n";
			}
		}
		file << "\t</triangles>\n";
		file << "</submesh>\n";
	}
	
}

void AssimpProcesser::ExportBinary( OutModel& outModel )
{
	FileStream stream(outModel.OutName, FILE_WRITE);

	// write mesh name, for test
	stream.WriteString("Test");
	stream.WriteUInt(outModel.MeshParts.size());

	vector<shared_ptr<MeshPartData> >& subMeshes = outModel.MeshParts;
	for (size_t i = 0; i < subMeshes.size(); ++i)
	{
		shared_ptr<MeshPartData> submesh = subMeshes[i];
		stream.WriteString(submesh->Name);
		stream.WriteUInt(submesh->VertexCount);
		stream.WriteUInt(submesh->VertexDeclaration->GetVertexSize());

		// write vertex declaration, element size
		stream.WriteUInt(submesh->VertexDeclaration->GetElementCount());

		// write each vertex element
		const std::vector<VertexElement>& elements = submesh->VertexDeclaration->GetElements();
		for (auto iter = elements.begin(); iter != elements.end(); ++iter)
		{
			const VertexElement& ve = *iter;
			stream.WriteUInt(ve.Offset);
			stream.WriteUInt(ve.Type);
			stream.WriteUInt(ve.Usage);
			stream.WriteUShort(ve.UsageIndex);
		}

		// write vertex buffer data
		uint32_t bufferSize = submesh->VertexCount * submesh->VertexDeclaration->GetVertexSize();
		stream.Write(&(submesh->VertexData[0]), sizeof(char) * submesh->VertexData.size());

		// write triangles count
		stream.WriteUInt(submesh->IndexCount);
		stream.WriteUInt(submesh->IndexFormat);
		stream.Write(&submesh->IndexData[0], sizeof(char) * submesh->IndexData.size());
	}

	stream.Close();
}

void AssimpProcesser::CollectMeshes( OutModel& outModel, aiNode* node )
{
	for (uint32_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = mAIScene->mMeshes[node->mMeshes[i]];
		for (size_t j = 0; j < outModel.Meshes.size(); ++j)
		{
			if (outModel.Meshes[j] == mesh)
			{
				PrintLine("Same mesh found in multiple node");
				break;;
			}
		}

		outModel.Meshes.push_back(mesh);
		outModel.MeshNodes.push_back(node);
		outModel.TotalVertices += mesh->mNumVertices;
		outModel.TotalVertices += mesh->mNumFaces * 3;
	}

	for (uint32_t i = 0; i < node->mNumChildren; ++i)
	{
		CollectMeshes(outModel, node->mChildren[i]);
	}
}

void AssimpProcesser::CollectBones( OutModel& outModel )
{
	std::set<aiNode*> necessary;
	std::set<aiNode*> rootNodes;

	for (uint32_t i =0; i < outModel.Meshes.size(); ++i)
	{
		aiMesh* mesh = outModel.Meshes[i];
		aiNode* meshNode = outModel.MeshNodes[i];
		aiNode* meshParentNode = meshNode->mParent;
		aiNode* rootNode;

		for (size_t j = 0; j < mesh->mNumBones; ++j)
		{
			aiBone* bone = mesh->mBones[j];
			String boneName(bone->mName.C_Str());
			aiNode* boneNode = GetNode(boneName, mAIScene->mRootNode);
			if (!boneNode)
			{
				ENGINE_EXCEPT(Core::Exception::ERR_INTERNAL_ERROR, "Couldn't find the scene node for Bone: " + boneName
					, "AssimpProcesser::CollectBones");
			}

			necessary.insert(boneNode);
			rootNode = boneNode;

			while(true)
			{
				boneNode = boneNode->mParent;
				if (!boneNode || boneNode == meshNode || boneNode == meshParentNode)
					break;
				rootNode = boneNode;
				necessary.insert(boneNode);
			}
			rootNodes.insert(rootNode);
		}
	}

	 // If we find multiple root nodes
	if (rootNodes.size() > 1)
	{
		aiNode* commonParent =  (*rootNodes.begin())->mParent;

		for(auto iter = rootNodes.begin(); iter != rootNodes.begin(); ++iter)
		{
			if ( (*iter) != commonParent )
			{
				if (!commonParent || (*iter)->mParent != commonParent)
				{
					 ENGINE_EXCEPT(Core::Exception::ERR_INTERNAL_ERROR,
						 "Skeleton with multiple root nodes found, not supported", "AssimpProcesser::CollectBones");
				}
			}
		}

		rootNodes.clear();
		rootNodes.insert(commonParent);
		necessary.insert(commonParent);
	}

	if (rootNodes.empty())
		return;

	outModel.RootBone = *rootNodes.begin();
	CollectBonesFinal(outModel.Bones, necessary, outModel.RootBone);
}

void AssimpProcesser::CollectBonesFinal( vector<aiNode*>& bones, const set<aiNode*>& necessary, aiNode* node )
{
	if (necessary.find(node) != necessary.end())
	{
		bones.push_back(node);

		for (size_t i = 0; i < node->mNumChildren; ++i)
		{
			CollectBonesFinal(bones, necessary, node->mChildren[i]);
		}
	}
}

void AssimpProcesser::ExportModel( OutModel& outModel, const String& outName )
{
	using namespace RcEngine::Core;

	outModel.OutName = outName;
	CollectMeshes(outModel, outModel.RootNode);
	CollectBones(outModel);
	BuildAndSaveModel(outModel);

	/*XMLDoc file;
	XMLNodePtr root = file.AllocateNode(XML_Node_Element, "Joints");*/
	//ofstream stream("Joint.xml", std::ios::out);

	//auto help1 = [](const Vector3f& vec) -> string{
	//	stringstream sss;
	//	sss << "x=\"" << vec.X() << "\" y=\"" << vec.Y() << "\" z=\"" << vec.Z() << "\"" ;
	//	return sss.str();
	//};

	//auto help2 = [](const Quaternionf& quat) -> string{
	//	stringstream sss;
	//	sss << "w=\"" << quat.W() << "\" x=\"" << quat.X() << "\" y=\"" << quat.Y() << "\" z=\"" << quat.Z() << "\"" ;
	//	return sss.str();
	//};

	//vector<Joint> joints = outModel.Skeleton->GetJoints();
	//stream << "<Joints>" << endl;
	//for (size_t i = 0; i < joints.size(); ++i)
	//{
	//	/*XMLNodePtr jointNode = file.AllocateNode(XML_Node_Element, "Joint");
	//	jointNode->AppendAttribute( file.AllocateAttributeString("name", joints[i].Name) );
	//	jointNode->AppendAttribute( file.AllocateAttributeUInt("parent", joints[i].ParentIndex) );
	//	XMLNodePtr bindPosNode = file.AllocateNode(XML_Node_Element, "bindPostion");
	//	bindPosNode->AppendAttribute(file.AllocateAttributeFloat("x", joints[i].InitialPosition.X()));
	//	bindPosNode->AppendAttribute(file.AllocateAttributeFloat("y", joints[i].InitialPosition.Y()));
	//	bindPosNode->AppendAttribute(file.AllocateAttributeFloat("z", joints[i].InitialPosition.Z()));
	//	XMLNodePtr bindRotationNode = file.AllocateNode(XML_Node_Element, "bindRotation");
	//	bindRotationNode->AppendAttribute(file.AllocateAttributeFloat("w", joints[i].InitialRotation.W()));
	//	bindRotationNode->AppendAttribute(file.AllocateAttributeFloat("x", joints[i].InitialRotation.X()));
	//	bindRotationNode->AppendAttribute(file.AllocateAttributeFloat("y", joints[i].InitialRotation.Y()));
	//	bindRotationNode->AppendAttribute(file.AllocateAttributeFloat("z", joints[i].InitialRotation.Z()));*/
	//
	//	/*	jointNode->AppendNode(bindPosNode);
	//	jointNode->AppendNode(bindRotationNode);
	//	root->AppendNode(jointNode);*/

	//	stream << "<joint name=\"" << joints[i].Name << "\" parent=\"" << joints[i].ParentIndex << "\">" << endl;
	//	stream << "\t<bindPosition " + help1(joints[i].InitialPosition) + ">" << endl; 
	//	stream << "\t<bindRotation " + help2(joints[i].InitialRotation) + ">" << endl; 
	//	stream << "</joint>" << endl;

	//}
	//stream << "</Joints>" << endl;
	//stream.close();
	//file.RootNode(root);

	
}

void AssimpProcesser::BuildAndSaveModel( OutModel& outModel )
{
	if (!outModel.RootNode)
	{
		// Error no model root node
		return;
	}

	String rootNodeName = String(outModel.RootNode->mName.C_Str());
	if (outModel.Meshes.empty())
	{
		// Error, no geometry start from this node
		return;
	}

	for (size_t i = 0; i < outModel.Meshes.size(); ++i)
	{
		// Get the world transform of the mesh for baking into the vertices
		Matrix4f vertexTransform = FromAIMatrix(
			GetMeshBakingTransform(outModel.MeshNodes[i], outModel.RootNode) );
		
		aiMatrix4x4 verteAI = GetMeshBakingTransform(outModel.MeshNodes[i], outModel.RootNode );
		
		Vector3f scaleAI, positionAI;
		Quaternionf rotAI;
		GetPosRotScale(verteAI, positionAI, rotAI, scaleAI);

		Matrix4f normalTransform = MatrixInverse(vertexTransform).GetTransposed();

		Vector3f scale, translation;
		Quaternionf rot;
		MatrixDecompose(scale, rot, translation, vertexTransform);
		
		aiMesh* mesh = outModel.Meshes[i];
		shared_ptr<VertexDeclaration> vertexDecl = GetVertexDeclaration(mesh);

		shared_ptr<MeshPartData> meshPart(new MeshPartData);

		// Store index data
		bool largeIndices = mesh->mNumVertices > 65535;
		if (!largeIndices)
		{
			meshPart->IndexData.resize( sizeof(uint16_t) * mesh->mNumFaces * 3 );
			uint16_t* dest = (uint16_t*)(&meshPart->IndexData[0]);
			for (unsigned f = 0; f < mesh->mNumFaces; ++f)
			{
				aiFace face = mesh->mFaces[f];
				assert(face.mNumIndices == 3);
				*dest++ =  face.mIndices[0];
				*dest++ =  face.mIndices[1];
				*dest++ =  face.mIndices[2];
			}
			meshPart->IndexFormat = IBT_Bit16;
			meshPart->IndexCount =  mesh->mNumFaces * 3;
		}
		else
		{
			meshPart->IndexData.resize( sizeof(uint32_t) * mesh->mNumFaces * 3 );
			uint32_t* dest = (uint32_t*)(&meshPart->IndexData[0]);
			for (unsigned f = 0; f < mesh->mNumFaces; ++f)
			{
				aiFace face = mesh->mFaces[f];
				assert(face.mNumIndices == 3);
				*dest++ =  face.mIndices[0];
				*dest++ =  face.mIndices[1];
				*dest++ =  face.mIndices[2];
			}
			meshPart->IndexFormat = IBT_Bit32;
			meshPart->IndexCount =  mesh->mNumFaces * 3;
		}

		// Store vertex data
		meshPart->VertexData.resize( vertexDecl->GetVertexSize() * mesh->mNumVertices );

		vector<vector<uint8_t> > blendIndices;
		vector<vector<float> > blendWeights;
		vector<uint32_t> boneMappings;
		if (outModel.Bones.size())
		{
			GetBlendData(outModel, mesh, boneMappings, blendIndices, blendWeights);
		}

		const vector<VertexElement>& vertexElements = vertexDecl->GetElements();
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			int baseOffset = vertexDecl->GetVertexSize() * i;
			for (size_t ve = 0; ve < vertexElements.size(); ++ve)
			{
				const VertexElement& element = vertexElements[ve];
				float* vertexPtr = (float*)(&meshPart->VertexData[0] + baseOffset + element.Offset);
				switch(element.Usage)
				{
				case VEU_Position:
					{
						// Bake the mesh vertex in model space defined by the root node
						// So even without the skeleton, the mesh can render with unskin mesh.
						Vector3f vertex = Transform( FromAIVector(mesh->mVertices[i]), vertexTransform );
						
						aiVector3D v = verteAI *  mesh->mVertices[i];
						
						*(vertexPtr) = vertex.X();
						*(vertexPtr+1) = vertex.Y();
						*(vertexPtr+2) = vertex.Z();
						// bouding sphere
					}	
					break;
				case VEU_Normal:
					{
						Vector3f normal = Transform( FromAIVector(mesh->mNormals[i]), normalTransform);
						*(vertexPtr) = normal.X();
						*(vertexPtr+1) = normal.Y();
						*(vertexPtr+2) = normal.Z();
					}
					break;
				case VEU_Tangent:
					{
						Vector3f tangent = Transform( FromAIVector(mesh->mTangents[i]), normalTransform);
						*(vertexPtr) = tangent.X();
						*(vertexPtr+1) = tangent.Y();
						*(vertexPtr+2) = tangent.Z();
					}			
					break;
				case VEU_Binormal:
					{
						Vector3f bitangent = Transform( FromAIVector(mesh->mBitangents[i]), normalTransform);
						*(vertexPtr) = bitangent.X();
						*(vertexPtr+1) = bitangent.Y();
						*(vertexPtr+2) = bitangent.Z();
					}			
					break;
				case VEU_TextureCoordinate:
					{
						switch(mesh->mNumUVComponents[element.UsageIndex])
						{
						case 3:
							*(vertexPtr+2) = mesh->mTextureCoords[element.UsageIndex][i].z;
						case 2:
							*(vertexPtr+1) = mesh->mTextureCoords[element.UsageIndex][i].y;	
						case 1:
							*(vertexPtr) = mesh->mTextureCoords[element.UsageIndex][i].x;
							break;
						}
					}
					break;
				case VEU_BlendWeight:
					{
						for (uint32_t j = 0; j < 4; ++j)
						{
							if (j < blendWeights[i].size())
							{
								float weight = blendWeights[i][j];
								*vertexPtr++ = weight;
							}
							else
								*vertexPtr++ = 0.0f;
						}
					}
					break;
				case VEU_BlendIndices:
					{
						uint8_t* destBytes = (uint8_t*)vertexPtr;
						for (uint32_t j = 0; j < 4; ++j)
						{
							if (j < blendIndices[i].size())
							{
								uint16_t index =  blendIndices[i][j];
								*destBytes++ = blendIndices[i][j];
							}
							else
								*destBytes++ = 0;
						}
					}
					break;

				default:
					break;
				}
			}
		}

		meshPart->StartVertex = 0;
		meshPart->VertexCount = mesh->mNumVertices;
		meshPart->VertexDeclaration = vertexDecl;
		outModel.MeshParts.push_back(meshPart);
	}

	 // Build skeleton if necessary
	if (outModel.Bones.size() && outModel.RootBone)
	{
		shared_ptr<Skeleton> skeleton(new Skeleton);
		vector<Joint>& joints = skeleton->GetJoints();

		for (size_t i = 0; i < outModel.Bones.size(); ++i)
		{
			aiNode* boneNode = outModel.Bones[i];
			String boneName(boneNode->mName.C_Str());

			Joint newJoint;
			newJoint.Name = boneName;

			aiMatrix4x4 transform = boneNode->mTransformation;

			// Make the root bone transform relative to the model's root node, if it is not already
		    // This will put the mesh in model coordinate system.
			if (boneNode == outModel.RootBone)
				transform = GetDerivedTransform(boneNode, outModel.RootNode, boneNode->mTransformation);

			Matrix4f trans = FromAIMatrix(transform);

			MatrixDecompose(newJoint.InitialScale, newJoint.InitialRotation, newJoint.InitialPosition, trans);


			Vector3f scale, position;
			Quaternionf rot;
			GetPosRotScale(transform, position, rot, scale);

			// Get offset information if exists
			newJoint.OffsetMatrix = FromAIMatrix(GetOffsetMatrix(outModel, boneName));
			newJoint.ParentIndex = i;
			joints.push_back(newJoint);
		}
		// Set the bone hierarchy
		for (size_t i = 1; i < outModel.Bones.size(); ++i)
		{
			String parentName(outModel.Bones[i]->mParent->mName.C_Str());
			for (size_t j = 0; j < joints.size(); ++j)
			{
				if (joints[j].Name == parentName)
				{
					joints[i].ParentIndex = j;
					break;
				}
			}
		}

		outModel.Skeleton = skeleton;
	}
}
