#include <Graphics/Sky.h>
#include <Graphics/Material.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/SimpleGeometry.h>
#include <Core/Context.h>

namespace {

using namespace RcEngine;

typedef vector<Vector3f> VertexList;
typedef vector<uint32_t> IndexList;


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

}


namespace RcEngine {

static const char* SkyBoxPlaneName[SkyBox::MaxPlaneCount] = {
	"FrontPlane", "BackPlane", "LeftPlane", "RightPlane", "TopPlane", "BottomPlane"
};


struct SkyPlaneVertex
{
	Vector3f Position;
	Vector3f Tex;		// use texture array, third dimension is sky texture index

	SkyPlaneVertex() {}
	SkyPlaneVertex( const Vector3f& pos, const Vector3f& tex ) : Position(pos), Tex(tex) {}
};

class SkyPlane : public Renderable
{
public:
	SkyPlane(SkyBox& sky, SkyBox::SkyBoxPlane which)
		:mParent(sky), mPlaneType(which), mRenderOperation(new RenderOperation)
	{
		mRenderOperation->BindVertexStream(mParent.mVertexBuffer, mParent.mVertexDecl);
		mRenderOperation->StartVertexLocation = mPlaneType * 4;
		mRenderOperation->BindIndexStream(mParent.mIndexBuffer, IBT_Bit16);
		mRenderOperation->StartIndexLocation = 0;
		mRenderOperation->PrimitiveType = PT_Triangle_List;
	}

	const shared_ptr<Material>& GetMaterial() const
	{
		return mParent.mMaterials[mPlaneType];
	}

	const shared_ptr<RenderOperation>& GetRenderOperation() const
	{
		return mRenderOperation;
	}
	
	void GetWorldTransforms(Matrix4f* xform) const
	{
		xform[0] = mParent.GetWorldTransform();
	}

	uint32_t GetWorldTransformsCount() const
	{
		return 1;
	}

private:
	SkyBox& mParent;
	SkyBox::SkyBoxPlane mPlaneType;
	shared_ptr<RenderOperation> mRenderOperation;
};


class SkyBoxCube : public Renderable
{
public:
	SkyBoxCube( SkyBox& sky ) : mParent(sky), mRenderOperation(new RenderOperation)
	{
		mRenderOperation->BindVertexStream(mParent.mVertexBuffer, mParent.mVertexDecl);
		mRenderOperation->StartVertexLocation = 0;
		mRenderOperation->BindIndexStream(mParent.mIndexBuffer, IBT_Bit32);
		mRenderOperation->StartIndexLocation = 0;
		mRenderOperation->PrimitiveType = PT_Triangle_List;
	}

	const shared_ptr<Material>& GetMaterial() const
	{
		return mParent.mMaterial;
	}

	const shared_ptr<RenderOperation>& GetRenderOperation() const
	{
		return mRenderOperation;
	}
	
	void GetWorldTransforms(Matrix4f* xform) const
	{
		xform[0] = mParent.GetWorldTransform();
	}
	uint32_t GetWorldTransformsCount() const
	{
		return 1;
	}

private:
	SkyBox& mParent;
	shared_ptr<RenderOperation> mRenderOperation;
};

//---------------------------------------------------------------------------
SkyBox::SkyBox( float size /*= 100.0f*/ , bool cube /*= false*/ )
	: SceneObject("Sky Box"), mCubeMapSky(cube), mSkyCubeBox(nullptr)
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	
	if (!cube)
	{
		// Create vertex declaration
		VertexElement vertexElems[] = {
			VertexElement(0, VEF_Vector3, VEU_Position),
			VertexElement(sizeof(Vector3f), VEF_Vector3, VEU_TextureCoordinate)
		};
		mVertexDecl = factory.CreateVertexDeclaration(vertexElems, 2);

		SkyPlaneVertex vertices[MaxPlaneCount*4];

		size_t base;
		// Front
		base = SkyBox::Front * 4;
		vertices[base+0].Position = Vector3f( -size, -size, -size );
		vertices[base+0].Tex = Vector3f(0.0f, 1.0f, 0.0f);
		vertices[base+1].Position = Vector3f( size, -size, -size );
		vertices[base+1].Tex = Vector3f(1.0f, 1.0f, 0.0f);
		vertices[base+2].Position = Vector3f( size, size, -size );
		vertices[base+2].Tex = Vector3f(1.0f, 0.0f, 0.0f);
		vertices[base+3].Position = Vector3f( -size, size, -size );
		vertices[base+3].Tex = Vector3f(0.0f, 0.0f, 0.0f);

		// Back
		base = SkyBox::Back * 4;
		vertices[base+0].Position = Vector3f( size, -size, size );
		vertices[base+0].Tex = Vector3f(0.0f, 1.0f, 1.0f);
		vertices[base+1].Position = Vector3f( -size, -size, size );
		vertices[base+1].Tex = Vector3f(1.0f, 1.0f, 1.0f);
		vertices[base+2].Position = Vector3f( -size, size, size );
		vertices[base+2].Tex = Vector3f(1.0f, 0.0f, 1.0f);
		vertices[base+3].Position = Vector3f( size, size, size );
		vertices[base+3].Tex = Vector3f(0.0f, 0.0f, 1.0f);

		// Left
		base = SkyBox::Left * 4;
		vertices[base+0].Position = Vector3f( -size, -size, size );
		vertices[base+0].Tex = Vector3f(0.0f, 1.0f, 2.0f);
		vertices[base+1].Position = Vector3f( -size, -size, -size );
		vertices[base+1].Tex = Vector3f(1.0f, 1.0f, 2.0f);
		vertices[base+2].Position = Vector3f( -size, size, -size );
		vertices[base+2].Tex = Vector3f(1.0f, 0.0f, 2.0f);
		vertices[base+3].Position = Vector3f( -size, size, size );
		vertices[base+3].Tex = Vector3f(0.0f, 0.0f, 2.0f);

		// Right
		base = SkyBox::Right * 4;
		vertices[base+0].Position = Vector3f( size, -size, -size );
		vertices[base+0].Tex = Vector3f(0.0f, 1.0f, 3.0f);
		vertices[base+1].Position = Vector3f( size, -size, size );
		vertices[base+1].Tex = Vector3f(1.0f, 1.0f, 3.0f);
		vertices[base+2].Position = Vector3f( size, size, size );
		vertices[base+2].Tex = Vector3f(1.0f, 0.0f, 3.0f);
		vertices[base+3].Position = Vector3f( size, size, -size );
		vertices[base+3].Tex = Vector3f(0.0f, 0.0f, 3.0f);

		// Up
		base = SkyBox::Up * 4;
		vertices[base+0].Position = Vector3f( -size, size, -size );
		vertices[base+0].Tex = Vector3f(0.0f, 1.0f, 4.0f);
		vertices[base+1].Position = Vector3f( size, size, -size );
		vertices[base+1].Tex = Vector3f(1.0f, 1.0f, 4.0f);
		vertices[base+2].Position = Vector3f( size, size, size );
		vertices[base+2].Tex = Vector3f(1.0f, 0.0f, 4.0f);
		vertices[base+3].Position = Vector3f( -size, size, size );
		vertices[base+3].Tex = Vector3f(0.0f, 0.0f, 4.0f);

		// Down
		base = SkyBox::Down * 4;
		vertices[base+0].Position = Vector3f( -size, -size, size );
		vertices[base+0].Tex = Vector3f(0.0f, 1.0f, 5.0f);
		vertices[base+1].Position = Vector3f( size, -size, size );
		vertices[base+1].Tex = Vector3f(1.0f, 1.0f, 5.0f);
		vertices[base+2].Position = Vector3f( size, -size, -size );
		vertices[base+2].Tex = Vector3f(1.0f, 0.0f, 5.0f);
		vertices[base+3].Position = Vector3f( -size, -size, -size );
		vertices[base+3].Tex = Vector3f(0.0f, 0.0f, 5.0f);

		ElementInitData vInitData;
		vInitData.pData = vertices;
		vInitData.rowPitch = sizeof(vertices);
		mVertexBuffer = factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

		uint16_t indices[6] = { 0, 1, 3, 3, 1, 2 };
		ElementInitData iInitData;
		iInitData.pData = indices;
		iInitData.rowPitch = sizeof(indices);
		mIndexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);

	}
	else
	{
		// Create vertex declaration
		VertexElement vertexElems[] = {
			VertexElement(0, VEF_Vector3, VEU_Position),
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

		mSkyCubeBox = new SkyBoxCube(*this);
	}
	
}

SkyBox::~SkyBox()
{
	for (size_t i = 0; i < MaxPlaneCount; ++i)
	{
		if (mSkyPlanes[i])
			Safe_Delete(mSkyPlanes[i]);
	}

	if (mSkyCubeBox)
	{
		Safe_Delete(mSkyCubeBox);
	}

}

//void SkyBox::SetMaterial( SkyBoxPlane plane, const shared_ptr<Material>& mat )
//{
//	mMaterials[plane] = mat;
//}

void SkyBox::SetMaterial( const shared_ptr<Material>& mat )
{
	mMaterial = mat;
}

void SkyBox::AddToRenderQueue( RenderQueue& renderQueue ) const
{
	if(!mCubeMapSky)
	{
		for (size_t i = 0; i < MaxPlaneCount; ++i)
		{
			renderQueue.push_back( RenderQueueItem(SOT_Sky, mSkyPlanes[i], 0) );
		}
	}
	else
	{
		renderQueue.push_back( RenderQueueItem(SOT_Sky, mSkyCubeBox, 0) );
	}
}
}