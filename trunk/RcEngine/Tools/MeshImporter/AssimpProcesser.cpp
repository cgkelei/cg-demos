#include "AssimpProcesser.h"
#include "Content/MeshContentLoader.h"
#include "Content/MaterialContentLoader.h"
#include "Graphics/VertexDeclaration.h"
#include "Graphics/RenderFactory.h"
#include "Math/ColorRGBA.h"

#include <fstream>
using namespace std;


#pragma comment(lib, "assimp.lib")

using namespace RcEngine::Render;
using namespace RcEngine::Math;

AssimpProcesser::AssimpProcesser(void)
{
}


AssimpProcesser::~AssimpProcesser(void)
{
}

bool AssimpProcesser::Process( const char* filePath )
{
	Assimp::Importer importer;
	
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate |
		aiProcess_RemoveRedundantMaterials | aiProcess_OptimizeMeshes | aiProcess_FlipUVs);
	
	if(!scene)
	{
		// Error
		std::cerr <<  importer.GetErrorString() << std::endl;
		return false;
	}

	aiNode* rootNode = scene->mRootNode;
	string name(rootNode->mName.data);
	uint32_t c = rootNode->mNumChildren;
	ProcessScene(scene);

	return true;
}

void AssimpProcesser::ProcessScene( const aiScene* scene )
{
	MeshContent* meshContent = new MeshContent;

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];
		ProcessMesh(mesh, meshContent);
	}
	
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* material = scene->mMaterials[i];
		ProcessMaterial(material, meshContent);
	}

	Export("Test.xml", meshContent);
}

void AssimpProcesser::ProcessMesh( aiMesh* mesh, MeshContent* meshContent )
{
	MeshPartContent* meshPartContent = new MeshPartContent;
	meshPartContent->Name = std::string(mesh->mName.C_Str());
	meshPartContent->MaterialID = mesh->mMaterialIndex;
	
	// process faces
	meshPartContent->mFaces.resize(mesh->mNumFaces);
	meshPartContent->IndexCount = mesh->mNumFaces * 3;
	meshPartContent->IndexFormat = IBT_Bit32;
	for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
	{
		aiFace face = mesh->mFaces[f];
		assert(face.mNumIndices == 3);
		meshPartContent->mFaces[f] = MeshPartContent::Face(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
	}

	// Process vertices
	std::vector<VertexElement> vertexElements;
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

	meshPartContent->VertexDeclaration = std::make_shared<VertexDeclaration>(vertexElements);
	meshPartContent->VertexCount = mesh->mNumVertices;
	unsigned int vertexSize = offset;
	
	// bounding sphere
	meshPartContent->BoundingSphere.mCenter = Vector3f(0.0f, 0.0f, 0.0f);
	meshPartContent->BoundingSphere.mRadius = 0.0f;

	meshPartContent->VertexData = new char[vertexSize * mesh->mNumVertices];

	aiVector3D v = mesh->mVertices[mesh->mNumVertices-1];

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		int baseOffset = vertexSize * i;
		for (size_t ve = 0; ve < vertexElements.size(); ++ve)
		{
			const VertexElement& element = vertexElements[ve];
			float* vertexPtr = (float*)(meshPartContent->VertexData + baseOffset + element.Offset);
			switch(element.Usage)
			{
			case VEU_Position:
				*(vertexPtr) = mesh->mVertices[i].x;
				*(vertexPtr+1) = mesh->mVertices[i].y;
				*(vertexPtr+2) = mesh->mVertices[i].z;
				break;
			case VEU_Normal:
				*(vertexPtr) = mesh->mNormals[i].x;
				*(vertexPtr+1) = mesh->mNormals[i].y;
				*(vertexPtr+2) = mesh->mNormals[i].z;
				break;
			case VEU_Tangent:
				*(vertexPtr) = mesh->mTangents[i].x;
				*(vertexPtr+1) = mesh->mTangents[i].y;
				*(vertexPtr+2) = mesh->mTangents[i].z;
				break;
			case VEU_Binormal:
				*(vertexPtr) = mesh->mBitangents[i].x;
				*(vertexPtr+1) = mesh->mBitangents[i].y;
				*(vertexPtr+2) = mesh->mBitangents[i].z;
				break;
			case VEU_TextureCoordinate:
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
			case VEU_BlendIndices:
				break;
			case VEU_BlendWeight:
				break; 

			default:
				break;
			}
		}
	}
	
	// combine in mesh
	meshContent->MeshPartContentLoaders.push_back(meshPartContent);	
}

void AssimpProcesser::ProcessMaterial( aiMaterial* material, MeshContent* meshContent )
{
	MaterialContent* materialContent = new MaterialContent;

	for (unsigned int i = 0; i < material->mNumProperties; ++i)
	{
		aiMaterialProperty* prop = material->mProperties[i];
		
		if (prop->mKey == aiString("?mat.name"))
		{
			aiString name;
			material->Get(AI_MATKEY_NAME,name);
			materialContent->MaterialName = string(name.C_Str());
		}
		else if (prop->mKey == aiString("$clr.ambient"))
		{
			aiColor3D color (0.f,0.f,0.f);
			material->Get(AI_MATKEY_COLOR_AMBIENT,color);
			materialContent->Ambient = ColorRGBA(color.r, color.g, color.b, 1.0f);
		}
		else if (prop->mKey == aiString("$clr.diffuse"))
		{
			aiColor3D color (0.f,0.f,0.f);
			material->Get(AI_MATKEY_COLOR_DIFFUSE,color);
			materialContent->Diffuse = ColorRGBA(color.r, color.g, color.b, 1.0f);
		}
		else if (prop->mKey == aiString("$clr.specular"))
		{
			aiColor3D color (0.f,0.f,0.f);
			material->Get(AI_MATKEY_COLOR_SPECULAR,color);
			materialContent->Diffuse = ColorRGBA(color.r, color.g, color.b, 1.0f);
		}
		else if (prop->mKey == aiString("$mat.shininess"))
		{
			float shininess;
			material->Get(AI_MATKEY_SHININESS,shininess);
			materialContent->Power = shininess;
		}
		else if (prop->mKey == aiString("$tex.file"))
		{
			switch(prop->mSemantic)
			{
			case aiTextureType_AMBIENT:
				{
					aiString file;
					material->GetTexture(aiTextureType_AMBIENT, 0, &file);
					materialContent->mTextures.insert(make_pair(string("AmbientMap"), string(file.C_Str())));
				}
			case aiTextureType_DIFFUSE:
				{
					aiString file;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &file);
					materialContent->mTextures.insert(make_pair(String("DiffuseMap"), String(file.C_Str())));
				}
				break;
			case aiTextureType_SPECULAR:
				{
					aiString file;
					material->GetTexture(aiTextureType_SPECULAR, 0, &file);
					materialContent->mTextures.insert(make_pair(string("SpecularMap"), string(file.C_Str())));
				}
				break;
			case aiTextureType_NORMALS:
				{
					aiString file;
					material->GetTexture(aiTextureType_NORMALS, 0, &file);
					materialContent->mTextures.insert(make_pair(string("NormalMap"), string(file.C_Str())));
				}
				break;
			case aiTextureType_DISPLACEMENT:
				{
					aiString file;
					material->GetTexture(aiTextureType_DISPLACEMENT, 0, &file);
					materialContent->mTextures.insert(make_pair(string("DisplacementMap"), string(file.C_Str())));
				}
				break;
			default:
				{
					
				}
			}
		}
	}

	// combine in mesh
	meshContent->MaterialContentLoaders.push_back(materialContent);	
}

void AssimpProcesser::Export( const char* output, MeshContent* meshContent )
{
	fstream stream(output, fstream::out);

	stream << "<mesh>" << endl;

	// output materials
	stream << "<materials materialCount=\"" << meshContent->MaterialContentLoaders.size() << "\">" << endl;
	vector<MaterialContent*>& materials = meshContent->MaterialContentLoaders;
	for (size_t i = 0; i < materials.size(); ++i)
	{
		MaterialContent* material = materials[i];
		stream << "\t<matrial>" << endl;
		stream << "\t\t<ambient r=\"" << material->Ambient.R() << "\" g=\"" << material->Ambient.G()
			<< "\" b=\"" << material->Ambient.B() << "\"/>" << endl;
		stream << "\t\t<diffuse r=\"" << material->Diffuse.R() << "\" g=\"" << material->Diffuse.G()
			<< "\" b=\"" << material->Diffuse.B() << "\"/>" << endl;
		stream << "\t\t<specular r=\"" << material->Specular.R() << "\" g=\"" << material->Specular.G()
			<< "\" b=\"" << material->Specular.B() << "\"/>" << endl;
		stream << "\t\t<emissive r=\"" << material->Emissive.R() << "\" g=\"" << material->Emissive.G()
			<< "\" b=\"" << material->Emissive.B() << "\"/>" << endl;
		stream << "\t\t<shininess v=\"" << material->Power << "\"/>" << endl;
		
		stream << "\t\t<texturesChunk>" << endl;
		for (auto iter = material->mTextures.begin(); iter != material->mTextures.end(); ++iter)
		{
			stream << "\t\t\t<texture type=\"" << iter->first << "\" name=\"" << iter->second << "\"/>" << endl;
		}
		stream << "\t\t</texturesChunk>" << endl;

		stream << "\t</matrial>" << endl;

	} 
	stream << "</materials>" << endl;

	stream << "<subMeshes>" << endl;

	vector<MeshPartContent*>& meshParts = meshContent->MeshPartContentLoaders;
	for (size_t i = 0; i < meshParts.size(); ++i)
	{
		MeshPartContent* meshPart = meshParts[i];
		stream << "\t<subMesh ";
		if (!meshPart->Name.empty())
		{
			stream <<"name=\"" << meshPart->Name << "\" ";
		}
		stream << "materialID=\"" << meshPart->MaterialID << "\">" << endl; 

		// Output vertices
		stream << "\t\t<vertices vertexCount=\"" << meshPart->VertexCount
			   << "\" vertexSize=\"" << meshPart->VertexDeclaration->GetVertexSize() << "\">" << endl;

		const shared_ptr<VertexDeclaration>& vd = meshPart->VertexDeclaration;
		for (unsigned int j = 0; j < meshPart->VertexCount; ++j)
		{	
			uint32_t vs = vd->GetVertexSize();
			char* vertexPtr = meshPart->VertexData + j * vd->GetVertexSize();
			float* temp = (float*)vertexPtr;
			stream << "\t\t\t<vertex>" << endl;
			for (size_t k = 0; k < vd->GetElementCount(); ++k)
			{	
				const VertexElement& e = vd->GetElement(k);	
				switch(e.Usage)
				{
				case VEU_Position:
					{	
						float x = *(temp); temp++;
						float y = *(temp); temp++;
						float z = *(temp); temp++;
						stream << "\t\t\t\t<position x=\"" << x <<"\" y=\"" << y << "\" z=\"" << z << "\"/>" << endl;
					}	
					break;
				case VEU_Normal:
					{
						float x = *(temp); temp++;
						float y = *(temp); temp++;
						float z = *(temp); temp++;
						stream << "\t\t\t\t<normal x=\"" << x <<"\" y=\"" << y << "\" z=\"" << z << "\"/>" << endl;
					}	
					break;
				case VEU_Tangent:
					{
						float x = *(temp); temp++;
						float y = *(temp); temp++;
						float z = *(temp); temp++;
						stream << "\t\t\t\t<tangent x=\"" << x <<"\" y=\"" << y << "\" z=\"" << z << "\"/>" << endl;
					}	
					break;
				case VEU_Binormal:
					{
						float x = *(temp); temp++;
						float y = *(temp); temp++;
						float z = *(temp); temp++;
						stream << "\t\t\t\t<binormal x=\"" << x <<"\" y=\"" << y << "\" z=\"" << z << "\"/>" << endl;
					}	
					break;
				case VEU_TextureCoordinate:
					{
						switch(VertexElement::GetTypeCount(e.Type))
						{
						case 1:
							{
								float x = *(temp); temp++;
								stream << "\t\t\t\t<texcoord u=\"" << x << "\"/>" << endl;
							}
							break;
						case 2:
							{
								float x = *(temp); temp++;
								float y = *(temp); temp++;
								stream << "\t\t\t\t<texcoord u=\"" << x <<"\" v=\"" << y << "\"/>" << endl;
							}
							break;
						case 3:
							{
								float x = *(temp); temp++;
								float y = *(temp); temp++;
								float z = *(temp); temp++;
								stream << "\t\t\t\t<texcoord u=\"" << x <<"\" v=\"" << y << "\" w=\"" << z << "\"/>" << endl;
							}
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
			stream << "\t\t\t</vertex>" << endl;
		}
		stream << "\t\t\t</vertex>" << endl;
		stream << "\t\t</vertices>" << endl;	

		// Output triangles
		stream << "\t\t<triangles>" << endl;	
		for (unsigned int i = 0; i < meshPart->mFaces.size(); ++i)
		{
			MeshPartContent::Face face = meshPart->mFaces[i];
			stream << "\t\t\t<triangle a=\""<<face.Indices[0] 
				   << "\" b=\"" << face.Indices[1] << "\" c=\"" << face.Indices[2] <<"\"/>" << endl;
		}
		stream << "\t\t</triangles>" << endl;	
		
		stream << "\t</subMesh>" << endl;
	}
	
	stream << "</subMeshes>" << endl;

	stream << "</mesh>" << endl;
	stream.close();
}
