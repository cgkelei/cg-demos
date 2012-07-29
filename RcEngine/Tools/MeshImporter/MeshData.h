#ifndef MeshData_h__
#define MeshData_h__

#include "Core/Prerequisites.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/VertexElement.h"
#include "Graphics/VertexDeclaration.h"
#include "Math/BoundingSphere.h"

#include "MaterialData.h"

using namespace RcEngine::Math;
using namespace RcEngine::Render;


struct MeshPartData
{
	String Name;
	uint32_t MaterialID;

	IndexBufferType IndexFormat;

	uint32_t StartIndex;
	uint32_t IndexCount;

	uint32_t VertexCount;
	uint32_t StartVertex;


	shared_ptr<VertexDeclaration> VertexDeclaration;

	BoundingSpheref BoundingSphere;

	vector<char> IndexData;
	vector<char> VertexData;
};

struct MeshData
{
	String Name;
	BoundingSpheref BoundingSphere;

	vector<shared_ptr<MeshPartData> > Materials;
	vector<shared_ptr<MeshPartData> > MeshParts;
};


#endif // MeshData_h__