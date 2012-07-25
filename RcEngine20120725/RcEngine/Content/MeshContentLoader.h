#ifndef MeshContentLoader_h__
#define MeshContentLoader_h__

#include "Core/Prerequisites.h"
#include "Math/BoundingSphere.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/VertexDeclaration.h"

namespace RcEngine
{
	namespace Content
	{
		class _ApiExport MeshPartContent
		{
		public:
			struct _ApiExport Face
			{
				uint32_t Indices[3];

				Face() { };
				Face(uint32_t A, uint32_t B, uint32_t C)
				{
					Indices[0] = A;
					Indices[1] = B;
					Indices[2] = C;
				}			
			};
				
		public:
			MeshPartContent();

			String Name;
			uint32_t MaterialID;

			Math::BoundingSpheref BoundingSphere;

			std::vector<Face> mFaces;
			
			Render::IndexBufferType IndexFormat;

			shared_ptr<Render::VertexDeclaration> VertexDeclaration;

			uint32_t StartIndex;
			uint32_t IndexCount;

			uint32_t VertexCount;
			uint32_t StartVertex;

			char* VertexData;
		};


		class MaterialContent;

		class _ApiExport MeshContent
		{
		public:
			MeshContent();

			String Name;
			Math::BoundingSpheref BoundingSphere;

			vector<MaterialContent*> MaterialContentLoaders;
			vector<MeshPartContent*> MeshPartContentLoaders;
		};

		MeshContent* ReadMeshContent(const String& meshFile);
	}
}

#endif // MeshContentLoader_h__
