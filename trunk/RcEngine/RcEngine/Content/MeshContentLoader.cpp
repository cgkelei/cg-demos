#include "MeshContentLoader.h"
#include "Graphics/VertexDeclaration.h"
#include "Core/XMLDom.h"

namespace RcEngine
{
	namespace Content
	{
		using namespace Core;
		using namespace Render;

		MeshPartContent::MeshPartContent()
		{

		}

		MeshPartContent* ReadMeshPartContent(XMLNodePtr meshPartNode)
		{
			MeshPartContent* result = new MeshPartContent;

			uint32_t materialId = meshPartNode->AttributeUInt("materialID", (std::numeric_limits<uint32_t>::max)());

			// read vertices
			XMLNodePtr verticesChunk = meshPartNode->FirstNode("verticesChunk");
			if (verticesChunk)
			{
				uint32_t vertexCount = verticesChunk->AttributeUInt("vertexCount", (std::numeric_limits<uint32_t>::max)());
				uint32_t vertexSize = verticesChunk->AttributeUInt("vertexSize", (std::numeric_limits<uint32_t>::max)());
			
				result->VertexData = new char[vertexCount * vertexSize];

				float* dataPtr = (float*)result->VertexData;

				uint16_t testVertexCount = 0;
				for (XMLNodePtr vertex = verticesChunk->FirstNode("vertex"); vertex; vertex = vertex->NextSibling("vertex"), ++testVertexCount)
				{
					XMLAttributePtr attr = vertex->FirstAttribute();
					float x = attr->ValueFloat(); *(dataPtr) = attr->ValueFloat();	dataPtr++; attr=attr->NextAttribute();	//x
					float y = attr->ValueFloat(); *(dataPtr) = attr->ValueFloat();	dataPtr++; attr=attr->NextAttribute();	//y
					float z = attr->ValueFloat(); *(dataPtr) = attr->ValueFloat();	dataPtr++;								//z

					for (XMLNodePtr other = vertex->FirstNode(); other; other = other->NextSibling())
					{
						String name = other->NodeName();
						if (name == String("normal"))
						{
							VertexElement ve;
						}
					}
				}

			}

			return result;
		}

		//////////////////////////////////////////////////////////////////////////
		MeshContent::MeshContent()
		{

		}

		MeshContent* ReadMeshContent( const String& meshFile )
		{
			MeshContent* meshContent = new MeshContent; 

			/*XMLDoc xmlFile;
			XMLNodePtr root = xmlFile.Parse(meshFile);

			XMLNodePtr meshPartsNode = root->FirstNode("meshPartsChunk");
			if (meshPartsNode)
			{
				for (XMLNodePtr node = meshPartsNode->FirstNode("meshPart"); node; node = node->NextSibling("meshPart"))
				{
					MeshPartContent* meshPart = ReadMeshPartContent(node);
					meshContent->MeshPartContentLoaders.push_back(meshPart);
				}
			}*/

			return meshContent;
		}
	}
}
