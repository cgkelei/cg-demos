#ifndef MeshContentLoader_h__
#define MeshContentLoader_h__

#include "Core/Prerequisites.h"
#include "Math/BoundingSphere.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/VertexElement.h"

namespace RcEngine
{
	namespace Content
	{
		class _ApiExport MeshPartContentLoader
		{
			struct _ApiExport Face
			{
				uint32_t Indices[3];
			};
				
		public:
			String Name;
			uint32_t MaterialID;

			unsigned char* IndexData;
			Render::IndexBufferType IndexFormat;

			uint32_t StartIndex;
			uint32_t IndexCount;
			uint32_t VertexOffset;
		};


		class MaterialContentLoader;

		class _ApiExport MeshContentLoader
		{
		public:
			String Name;
			Math::BoundingSpheref BoundingSphere;

			vector<MaterialContentLoader*> MaterialContentLoaders;
			vector<MeshPartContentLoader*> MeshPartContentLoaders;
		};
	}
}




#endif // MeshContentLoader_h__
