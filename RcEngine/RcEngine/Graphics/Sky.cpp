#include <Graphics/Sky.h>
#include <Graphics/Material.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/SimpleGeometry.h>
#include <Core/Context.h>

namespace RcEngine {

static const char* SkyBoxPlaneName[SkyBox::MaxPlaneCount] = {
	"FrontPlane", "BackPlane", "LeftPlane", "RightPlane", "TopPlane", "BottomPlane"
};


struct SkyPlaneVertex
{
	Vector3f Position;
	Vector2f Tex;

	SkyPlaneVertex() {}
	SkyPlaneVertex( const Vector3f& pos, const Vector2f& tex ) : Position(pos), Tex(tex) {}
};

class SkyPlane : public Renderable
{
public:
	SkyPlane(SkyBox& sky, SkyBox::SkyBoxPlane which)
		:mParent(sky), mPlaneType(which)
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
};


class SkyBoxCube : public Renderable
{
public:
	SkyBoxCube( SkyBox& sky ) : mParent(sky)
	{
		mRenderOperation->BindVertexStream(mParent.mVertexBuffer, mParent.mVertexDecl);
		mRenderOperation->StartVertexLocation = 0;
		mRenderOperation->BindIndexStream(mParent.mIndexBuffer, IBT_Bit16);
		mRenderOperation->StartIndexLocation = 0;
		mRenderOperation->PrimitiveType = PT_Triangle_List;
	}

	const shared_ptr<Material>& GetMaterial() const
	{
		return mParent.mMaterialCubeMap;
	}

	const shared_ptr<RenderOperation>& GetRenderOperation() const
	{
		return mRenderOperation;
	}
	
	void GetWorldTransforms(Matrix4f* xform) const
	{
		mParent.GetWorldTransform();
	}
	uint32_t GetWorldTransformsCount() const
	{
		return 1;
	}

private:
	SkyBox& mParent;
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
			VertexElement(sizeof(Vector3f), VEF_Vector2, VEU_TextureCoordinate)
		};
		mVertexDecl = factory.CreateVertexDeclaration(vertexElems, 2);

		SkyPlaneVertex vertices[MaxPlaneCount*4];

		size_t base;
		// Front
		base = SkyBox::Front * 4;
		vertices[base+0].Position = Vector3f( -size, -size, -size );
		vertices[base+0].Tex = Vector2f(0.0f, 1.0f);
		vertices[base+1].Position = Vector3f( size, -size, -size );
		vertices[base+1].Tex = Vector2f(1.0f, 1.0f);
		vertices[base+2].Position = Vector3f( size, size, -size );
		vertices[base+2].Tex = Vector2f(1.0f, 0.0f);
		vertices[base+3].Position = Vector3f( -size, size, -size );
		vertices[base+3].Tex = Vector2f(0.0f, 0.0f);

		// Back
		base = SkyBox::Back * 4;
		vertices[base+0].Position = Vector3f( size, -size, size );
		vertices[base+0].Tex = Vector2f(0.0f, 1.0f);
		vertices[base+1].Position = Vector3f( -size, -size, size );
		vertices[base+1].Tex = Vector2f(1.0f, 1.0f);
		vertices[base+2].Position = Vector3f( -size, size, size );
		vertices[base+2].Tex = Vector2f(1.0f, 0.0f);
		vertices[base+3].Position = Vector3f( size, size, size );
		vertices[base+3].Tex = Vector2f(0.0f, 0.0f);

		// Left
		base = SkyBox::Left * 4;
		vertices[base+0].Position = Vector3f( -size, -size, size );
		vertices[base+0].Tex = Vector2f(0.0f, 1.0f);
		vertices[base+1].Position = Vector3f( -size, -size, -size );
		vertices[base+1].Tex = Vector2f(1.0f, 1.0f);
		vertices[base+2].Position = Vector3f( -size, size, -size );
		vertices[base+2].Tex = Vector2f(1.0f, 0.0f);
		vertices[base+3].Position = Vector3f( -size, size, size );
		vertices[base+3].Tex = Vector2f(0.0f, 0.0f);

		// Right
		base = SkyBox::Right * 4;
		vertices[base+0].Position = Vector3f( size, -size, -size );
		vertices[base+0].Tex = Vector2f(0.0f, 1.0f);
		vertices[base+1].Position = Vector3f( size, -size, size );
		vertices[base+1].Tex = Vector2f(1.0f, 1.0f);
		vertices[base+2].Position = Vector3f( size, size, size );
		vertices[base+2].Tex = Vector2f(1.0f, 0.0f);
		vertices[base+3].Position = Vector3f( size, size, -size );
		vertices[base+3].Tex = Vector2f(0.0f, 0.0f);

		// Up
		base = SkyBox::Up * 4;
		vertices[base+0].Position = Vector3f( -size, size, -size );
		vertices[base+0].Tex = Vector2f(0.0f, 1.0f);
		vertices[base+1].Position = Vector3f( size, size, -size );
		vertices[base+1].Tex = Vector2f(1.0f, 1.0f);
		vertices[base+2].Position = Vector3f( size, size, size );
		vertices[base+2].Tex = Vector2f(1.0f, 0.0f);
		vertices[base+3].Position = Vector3f( -size, size, size );
		vertices[base+3].Tex = Vector2f(0.0f, 0.0f);

		// Down
		base = SkyBox::Down * 4;
		vertices[base+0].Position = Vector3f( -size, -size, size );
		vertices[base+0].Tex = Vector2f(0.0f, 1.0f);
		vertices[base+1].Position = Vector3f( size, -size, size );
		vertices[base+1].Tex = Vector2f(1.0f, 1.0f);
		vertices[base+2].Position = Vector3f( size, -size, -size );
		vertices[base+2].Tex = Vector2f(1.0f, 0.0f);
		vertices[base+3].Position = Vector3f( -size, -size, -size );
		vertices[base+3].Tex = Vector2f(0.0f, 0.0f);

		ElementInitData vInitData;
		vInitData.pData = vertices;
		vInitData.rowPitch = sizeof(vertices);
		mVertexBuffer = factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

		uint16_t indices[6] = { 0, 1, 3, 3, 1, 2 };
		ElementInitData iInitData;
		iInitData.pData = indices;
		iInitData.rowPitch = sizeof(indices);
		mIndexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);

		for (size_t i = 0; i < MaxPlaneCount; ++i)
		{
			mSkyPlanes[i] = new SkyPlane(*this, (SkyBoxPlane)i);
		}
	}
	else
	{
		
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

void SkyBox::SetMaterial( SkyBoxPlane plane, const shared_ptr<Material>& mat )
{
	mMaterials[plane] = mat;
}

void SkyBox::SetMaterial( const shared_ptr<Material>& mat )
{
	
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
	{}
}
}