#ifndef SpriteBatch_h__
#define SpriteBatch_h__

#include <Core/Prerequisites.h>
#include <Math/Matrix.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

class _ApiExport SpriteBatch
{
public:
	SpriteBatch();
	~SpriteBatch();

	void Begin();
	void End();

	void Draw(const Rectanglef& dest, const Rectanglef& src, const ColorRGBA& color = ColorRGBA::White);
	void Draw(const Rectanglef& dest, const Rectanglef& src, const ColorRGBA& color, const Vector2f& origin, float rotAngle, float layerDepth = 0.0f);

private:
	shared_ptr<Material> mMaterial;

	Matrix4f mProjectionMatrix;
};


}

#endif // SpriteBatch_h__
