#include <Graphics/Sky.h>
#include <Graphics/Material.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/SimpleGeometry.h>
#include <Graphics/Material.h>
#include <Resource/ResourceManager.h>
#include <Core/Context.h>
#include <IO/FileSystem.h>

namespace {

using namespace RcEngine;

typedef vector<Vector3f> VertexList;
typedef vector<uint32_t> IndexList;

static const char* SkyBoxPlaneName[Sky::MaxPlaneCount] = {
	"FrontPlane", "BackPlane", "LeftPlane", "RightPlane", "TopPlane", "BottomPlane"
};

void Subdivide(VertexList& vertices, IndexList& indices)
{
	VertexList vin = vertices;
	IndexList  iin = indices;

	vertices.resize(0);
	indices.resize(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	size_t numTris = iin.size()/3;
	for(size_t i = 0; i < numTris; ++i)
	{
		Vector3f v0 = vin[ iin[i*3+0] ];
		Vector3f v1 = vin[ iin[i*3+1] ];
		Vector3f v2 = vin[ iin[i*3+2] ];

		Vector3f m0 = 0.5f*(v0 + v1);
		Vector3f m1 = 0.5f*(v1 + v2);
		Vector3f m2 = 0.5f*(v0 + v2);

		vertices.push_back(v0); // 0
		vertices.push_back(v1); // 1
		vertices.push_back(v2); // 2
		vertices.push_back(m0); // 3
		vertices.push_back(m1); // 4
		vertices.push_back(m2); // 5

		indices.push_back(i*6+0);
		indices.push_back(i*6+3);
		indices.push_back(i*6+5);

		indices.push_back(i*6+3);
		indices.push_back(i*6+4);
		indices.push_back(i*6+5);

		indices.push_back(i*6+5);
		indices.push_back(i*6+4);
		indices.push_back(i*6+2);

		indices.push_back(i*6+3);
		indices.push_back(i*6+1);
		indices.push_back(i*6+4);
	}
}

void BuildGeometrySphere(uint32_t numSubdivisions, float radius, VertexList& vertices, IndexList& indices)
{
	// Put a cap on the number of subdivisions.
	numSubdivisions = (std::min)(numSubdivisions, uint32_t(5));

	// Approximate a sphere by tesselating an icosahedron.

	const float X = 0.525731f; 
	const float Z = 0.850651f;

	Vector3f pos[12] = 
	{
		Vector3f(-X, 0.0f, Z),  Vector3f(X, 0.0f, Z),  
		Vector3f(-X, 0.0f, -Z), Vector3f(X, 0.0f, -Z),    
		Vector3f(0.0f, Z, X),   Vector3f(0.0f, Z, -X), 
		Vector3f(0.0f, -Z, X),  Vector3f(0.0f, -Z, -X),    
		Vector3f(Z, X, 0.0f),   Vector3f(-Z, X, 0.0f), 
		Vector3f(Z, -X, 0.0f),  Vector3f(-Z, -X, 0.0f)
	};

	uint32_t k[60] = 
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,    
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,    
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0, 
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7 
	};

	vertices.resize(12);
	indices.resize(60);

	for(int i = 0; i < 12; ++i)
		vertices[i] = pos[i];

	for(int i = 0; i < 60; ++i)
		indices[i] = k[i];

	for(UINT i = 0; i < numSubdivisions; ++i)
		Subdivide(vertices, indices);

	// Project vertices onto sphere and scale.
	for(size_t i = 0; i < vertices.size(); ++i)
	{
		vertices[i] = Normalize(vertices[i]);
		vertices[i] *= radius;
	}
}

struct SkyPlaneVertex
{
	Vector3f Position;
	Vector3f Tex;		// use texture array, third dimension is sky texture index

	SkyPlaneVertex() {}
	SkyPlaneVertex( const Vector3f& pos, const Vector3f& tex ) : Position(pos), Tex(tex) {}
};

}


namespace RcEngine {

//---------------------------------------------------------------------------
Sky::Sky( float size  , bool cube /*= false*/ )
	: SceneObject("Sky Box"), mCubeMapSky(cube), mRenderOperation(new RenderOperation)
{
	mType = SOT_Sky;
	mRenderable = true;

	if (!cube)
	{
		InitializeSkyBox(size);	
	}
	else
	{
		InitializeSkyCubeMap(size);
	}

	// init render operation
	mRenderOperation->BindVertexStream(mVertexBuffer, mVertexDecl);
	mRenderOperation->StartVertexLocation = 0;
	mRenderOperation->BindIndexStream(mIndexBuffer, IBT_Bit32);
	mRenderOperation->StartIndexLocation = 0;
	mRenderOperation->PrimitiveType = PT_Triangle_List;
}

Sky::~Sky()
{

}

void Sky::SetMaterial( const shared_ptr<Material>& mat )
{
	mMaterial = mat;
}

uint32_t Sky::GetWorldTransformsCount() const
{
	return 1;
}

void Sky::GetWorldTransforms( Matrix4f* xform ) const
{
	xform[0] = GetWorldTransform();
}

void Sky::InitializeSkyBox(float size)
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	// Create vertex declaration
	VertexElement vertexElems[] = {
		VertexElement(0, VEF_Float3, VEU_Position),
		VertexElement(sizeof(Vector3f), VEF_Float3, VEU_TextureCoordinate)
	};

	mVertexDecl = factory.CreateVertexDeclaration(vertexElems, 2);

	SkyPlaneVertex vertices[Sky::MaxPlaneCount*4];

	size_t base;
	
	// Left
	base = Sky::Left * 4;
	vertices[base+0].Position = Vector3f( -size, -size, size );
	vertices[base+0].Tex = Vector3f(0.0f, 1.0f, float(Sky::Left));
	vertices[base+1].Position = Vector3f( -size, -size, -size );
	vertices[base+1].Tex = Vector3f(1.0f, 1.0f, float(Sky::Left));
	vertices[base+2].Position = Vector3f( -size, size, -size );
	vertices[base+2].Tex = Vector3f(1.0f, 0.0f, float(Sky::Left));
	vertices[base+3].Position = Vector3f( -size, size, size );
	vertices[base+3].Tex = Vector3f(0.0f, 0.0f, float(Sky::Left));

	// Right
	base = Sky::Right * 4;
	vertices[base+0].Position = Vector3f( size, -size, -size );
	vertices[base+0].Tex = Vector3f(0.0f, 1.0f, float(Sky::Right));
	vertices[base+1].Position = Vector3f( size, -size, size );
	vertices[base+1].Tex = Vector3f(1.0f, 1.0f, float(Sky::Right));
	vertices[base+2].Position = Vector3f( size, size, size );
	vertices[base+2].Tex = Vector3f(1.0f, 0.0f, float(Sky::Right));
	vertices[base+3].Position = Vector3f( size, size, -size );
	vertices[base+3].Tex = Vector3f(0.0f, 0.0f, float(Sky::Right));

	// Up
	base = Sky::Up * 4;
	vertices[base+0].Position = Vector3f( -size, size, -size );
	vertices[base+0].Tex = Vector3f(0.0f, 1.0f, float(Sky::Up));
	vertices[base+1].Position = Vector3f( size, size, -size );
	vertices[base+1].Tex = Vector3f(1.0f, 1.0f, float(Sky::Up));
	vertices[base+2].Position = Vector3f( size, size, size );
	vertices[base+2].Tex = Vector3f(1.0f, 0.0f, float(Sky::Up));
	vertices[base+3].Position = Vector3f( -size, size, size );
	vertices[base+3].Tex = Vector3f(0.0f, 0.0f, float(Sky::Up));

	// Down
	base = Sky::Down * 4;
	vertices[base+0].Position = Vector3f( -size, -size, size );
	vertices[base+0].Tex = Vector3f(0.0f, 1.0f, float(Sky::Down));
	vertices[base+1].Position = Vector3f( size, -size, size );
	vertices[base+1].Tex = Vector3f(1.0f, 1.0f, float(Sky::Down));
	vertices[base+2].Position = Vector3f( size, -size, -size );
	vertices[base+2].Tex = Vector3f(1.0f, 0.0f, float(Sky::Down));
	vertices[base+3].Position = Vector3f( -size, -size, -size );
	vertices[base+3].Tex = Vector3f(0.0f, 0.0f, float(Sky::Down));

	// Front
	base = Sky::Front * 4;
	vertices[base+0].Position = Vector3f( -size, -size, -size );
	vertices[base+0].Tex = Vector3f(0.0f, 1.0f, float(Sky::Front));
	vertices[base+1].Position = Vector3f( size, -size, -size );
	vertices[base+1].Tex = Vector3f(1.0f, 1.0f, float(Sky::Front));
	vertices[base+2].Position = Vector3f( size, size, -size );
	vertices[base+2].Tex = Vector3f(1.0f, 0.0f, float(Sky::Front));
	vertices[base+3].Position = Vector3f( -size, size, -size );
	vertices[base+3].Tex = Vector3f(0.0f, 0.0f, float(Sky::Front));

	// Back
	base = Sky::Back * 4;
	vertices[base+0].Position = Vector3f( size, -size, size );
	vertices[base+0].Tex = Vector3f(0.0f, 1.0f, float(Sky::Back));
	vertices[base+1].Position = Vector3f( -size, -size, size );
	vertices[base+1].Tex = Vector3f(1.0f, 1.0f, float(Sky::Back));
	vertices[base+2].Position = Vector3f( -size, size, size );
	vertices[base+2].Tex = Vector3f(1.0f, 0.0f, float(Sky::Back));
	vertices[base+3].Position = Vector3f( size, size, size );
	vertices[base+3].Tex = Vector3f(0.0f, 0.0f, float(Sky::Back));

	ElementInitData vInitData;
	vInitData.pData = vertices;
	vInitData.rowPitch = sizeof(vertices);
	mVertexBuffer = factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

	uint32_t indices[36];
	for (size_t i = 0; i < 6; ++i)
	{
		indices[i*6+0] = 0 + i*4;
		indices[i*6+1] = 1+ i*4;
		indices[i*6+2] = 3+ i*4;
		indices[i*6+3] = 3+ i*4;
		indices[i*6+4] = 1+ i*4;
		indices[i*6+5] = 2+ i*4;
	}

	ElementInitData iInitData;
	iInitData.pData = indices;
	iInitData.rowPitch = sizeof(indices);
	mIndexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);

	// Load material
	//mMaterial = factory.CreateMaterialFromFile("SkyNormal", "../Media/Materials/SkyNormal.material.xml");
}

void Sky::InitializeSkyCubeMap(float size)
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	// Create vertex declaration
	VertexElement vertexElems[] = {
		VertexElement(0, VEF_Float3, VEU_Position),
	};
	mVertexDecl = factory.CreateVertexDeclaration(vertexElems, 1);

	std::vector<Vector3f> vertices;
	std::vector<uint32_t> indices;
	BuildGeometrySphere(2, size, vertices, indices);

	for(size_t i = 0; i < vertices.size(); ++i)
	{
		// Scale on y-axis to turn into an ellipsoid to make a flatter Sky surface
		vertices[i] = 0.5f*vertices[i];
	}

	ElementInitData vInitData;
	vInitData.pData = &vertices[0];
	vInitData.rowPitch = sizeof(Vector3f) * vertices.size();
	mVertexBuffer = factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

	ElementInitData iInitData;
	iInitData.pData = &indices[0];
	iInitData.rowPitch = sizeof(uint32_t) * indices.size();
	mIndexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);

	// Load material
	mMaterial = std::static_pointer_cast<Material>(
		ResourceManager::GetSingleton().GetResourceByName(ResourceTypes::Material, "SkyCubeMap.material.xml", "General"));

	mMaterial->Load();
}

}