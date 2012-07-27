#include "AssimpProcesser.h"
#include "Graphics/VertexDeclaration.h"
#include "Graphics/RenderFactory.h"
#include "Math/ColorRGBA.h"
#include "Math/Matrix.h"
#include "Math/MathUtil.h"
#include "Graphics/Mesh.h"
#include "MainApp/Application.h"
#include "Core/Exception.h"

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
};

// Convert aiMatrix to RcEngine matrix, note that assimp 
// assume the right handed coordinate system, so aiMatrix 
// is a right-handed matrix.You need to transpose it to get
// a left-handed matrix.
void TransformMatrix(Matrix4f& out, aiMatrix4x4& in)
{
	out.M11 = in.a1;
	out.M12 = in.a2;
	out.M13 = in.a3;
	out.M14 = in.a4;
	
	out.M21 = in.b1;
	out.M22 = in.b2;
	out.M23 = in.b3;
	out.M24 = in.b4;
	
	out.M31 = in.c1;
	out.M32 = in.c2;
	out.M33 = in.c3;
	out.M34 = in.c4;
	
	out.M41 = in.d1;
	out.M42 = in.d2;
	out.M43 = in.d3;
	out.M44 = in.d4;
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
	CollectMeshes(model, scene->mRootNode);
	CollectBones(model);

	mMeshParts.resize(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];
		mMeshParts.push_back(  ProcessMeshPart(mesh) );
	}

	mMaterials.reserve(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* material = scene->mMaterials[i];
		mMaterials.push_back( ProcessMaterial(material) );
	}
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

void AssimpProcesser::ExportXML( const String& fileName )
{
	//fstream stream(output, fstream::out);

	//stream << "<mesh name=\"" << meshContent->Name << "\">" << endl;


	//// output materials
	//stream << "<materials materialCount=\"" << meshContent->MaterialContentLoaders.size() << "\">" << endl;
	//vector<MaterialContent*>& materials = meshContent->MaterialContentLoaders;
	//for (size_t i = 0; i < materials.size(); ++i)
	//{
	//	MaterialContent* material = materials[i];
	//	stream << "\t<matrial name=\"" << material->MaterialName << "\">" << endl;
	//	stream << "\t\t<ambient r=\"" << material->Ambient.R() << "\" g=\"" << material->Ambient.G()
	//		<< "\" b=\"" << material->Ambient.B() << "\"/>" << endl;
	//	stream << "\t\t<diffuse r=\"" << material->Diffuse.R() << "\" g=\"" << material->Diffuse.G()
	//		<< "\" b=\"" << material->Diffuse.B() << "\"/>" << endl;
	//	stream << "\t\t<specular r=\"" << material->Specular.R() << "\" g=\"" << material->Specular.G()
	//		<< "\" b=\"" << material->Specular.B() << "\"/>" << endl;
	//	stream << "\t\t<emissive r=\"" << material->Emissive.R() << "\" g=\"" << material->Emissive.G()
	//		<< "\" b=\"" << material->Emissive.B() << "\"/>" << endl;
	//	stream << "\t\t<shininess v=\"" << material->Power << "\"/>" << endl;

	//	stream << "\t\t<texturesChunk>" << endl;
	//	for (auto iter = material->mTextures.begin(); iter != material->mTextures.end(); ++iter)
	//	{
	//		stream << "\t\t\t<texture type=\"" << iter->first << "\" name=\"" << iter->second << "\"/>" << endl;
	//	}
	//	stream << "\t\t</texturesChunk>" << endl;

	//	stream << "\t</matrial>" << endl;

	//} 
	//stream << "</materials>" << endl;


	//stream << "<subMeshes count=\"" << meshContent->MeshPartContentLoaders.size() << "\">" << endl;

	//vector<MeshPartContent*>& meshParts = meshContent->MeshPartContentLoaders;
	//for (size_t i = 0; i < meshParts.size(); ++i)
	//{
	//	MeshPartContent* meshPart = meshParts[i];
	//	stream << "\t<subMesh ";
	//	if (!meshPart->Name.empty())
	//	{
	//		stream <<"name=\"" << meshPart->Name << "\" ";
	//	}
	//	stream << "materialID=\"" << meshPart->MaterialID << "\">" << endl; 
	//	//stream << ">" << endl;

	//	// Output vertices
	//	stream << "\t\t<vertices vertexCount=\"" << meshPart->VertexCount
	//		<< "\" vertexSize=\"" << meshPart->VertexDeclaration->GetVertexSize() << "\">" << endl;

	//	const shared_ptr<VertexDeclaration>& vd = meshPart->VertexDeclaration;
	//	for (unsigned int j = 0; j < meshPart->VertexCount; ++j)
	//	{	
	//		uint32_t vs = vd->GetVertexSize();
	//		char* vertexPtr = meshPart->VertexData + j * vd->GetVertexSize();
	//		float* temp = (float*)vertexPtr;
	//		stream << "\t\t\t<vertex>" << endl;
	//		for (size_t k = 0; k < vd->GetElementCount(); ++k)
	//		{	
	//			const VertexElement& e = vd->GetElement(k);	
	//			switch(e.Usage)
	//			{
	//			case VEU_Position:
	//				{	
	//					float x = *(temp); temp++;
	//					float y = *(temp); temp++;
	//					float z = *(temp); temp++;
	//					stream << "\t\t\t\t<position x=\"" << x <<"\" y=\"" << y << "\" z=\"" << z << "\"/>" << endl;
	//				}	
	//				break;
	//			case VEU_Normal:
	//				{
	//					float x = *(temp); temp++;
	//					float y = *(temp); temp++;
	//					float z = *(temp); temp++;
	//					stream << "\t\t\t\t<normal x=\"" << x <<"\" y=\"" << y << "\" z=\"" << z << "\"/>" << endl;
	//				}	
	//				break;
	//			case VEU_Tangent:
	//				{
	//					float x = *(temp); temp++;
	//					float y = *(temp); temp++;
	//					float z = *(temp); temp++;
	//					stream << "\t\t\t\t<tangent x=\"" << x <<"\" y=\"" << y << "\" z=\"" << z << "\"/>" << endl;
	//				}	
	//				break;
	//			case VEU_Binormal:
	//				{
	//					float x = *(temp); temp++;
	//					float y = *(temp); temp++;
	//					float z = *(temp); temp++;
	//					stream << "\t\t\t\t<binormal x=\"" << x <<"\" y=\"" << y << "\" z=\"" << z << "\"/>" << endl;
	//				}	
	//				break;
	//			case VEU_TextureCoordinate:
	//				{
	//					switch(VertexElement::GetTypeCount(e.Type))
	//					{
	//					case 1:
	//						{
	//							float x = *(temp); temp++;
	//							stream << "\t\t\t\t<texcoord u=\"" << x << "\"/>" << endl;
	//						}
	//						break;
	//					case 2:
	//						{
	//							float x = *(temp); temp++;
	//							float y = *(temp); temp++;
	//							stream << "\t\t\t\t<texcoord u=\"" << x <<"\" v=\"" << y << "\"/>" << endl;
	//						}
	//						break;
	//					case 3:
	//						{
	//							float x = *(temp); temp++;
	//							float y = *(temp); temp++;
	//							float z = *(temp); temp++;
	//							stream << "\t\t\t\t<texcoord u=\"" << x <<"\" v=\"" << y << "\" w=\"" << z << "\"/>" << endl;
	//						}
	//						break;
	//					}
	//				}
	//			case VEU_BlendIndices:
	//				break;
	//			case VEU_BlendWeight:
	//				break; 

	//			default:
	//				break;
	//			}
	//		}
	//		stream << "\t\t\t</vertex>" << endl;
	//	}
	//	stream << "\t\t\t</vertex>" << endl;
	//	stream << "\t\t</vertices>" << endl;	

	//	// Output triangles
	//	stream << "\t\t<triangles triangleCount=\"" << meshPart->mFaces.size() << "\">" << endl;	
	//	for (unsigned int i = 0; i < meshPart->mFaces.size(); ++i)
	//	{
	//		MeshPartContent::Face face = meshPart->mFaces[i];
	//		stream << "\t\t\t<triangle a=\""<<face.Indices[0] 
	//		<< "\" b=\"" << face.Indices[1] << "\" c=\"" << face.Indices[2] <<"\"/>" << endl;
	//	}
	//	stream << "\t\t</triangles>" << endl;	

	//	stream << "\t</subMesh>" << endl;
	//}

	//stream << "</subMeshes>" << endl;

	//stream << "</mesh>" << endl;
	//stream.close();
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

			// find all precursor node
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
	outModel.OutName = outName;
}
