#ifndef SpriteBatch_h__
#define SpriteBatch_h__

#include <Core/Prerequisites.h>
#include <Math/Matrix.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

// Forward declaration
class SpriteEntity;

class _ApiExport SpriteBatch
{
public:
	SpriteBatch();
	~SpriteBatch();

	void Begin();
	void End();
	void Flush();
	
	/**
	 * Adds a sprite to a batch of sprites for rendering using the specified texture, 
	 * destination rectangle, source rectangle, color, rotation, origin, effects and layer.
	 *
	 * @param destinationRectangle: A rectangle that specifies (in screen coordinates) the destination for drawing
	 * the sprite. If this rectangle is not the same size as the source rectangle, the sprite will be 
	 * scaled to fit.
	 *
	 * @param sourceRectangle: A rectangle that specifies (in texels) the source texels from a texture. Use null
	 * to draw the entire texture.
	 *
	 * @param color: The color to tint a sprite. Use Color.White for full color with no tinting.
	 *
	 * @param rotation: Specifies the angle (in radians) to rotate the sprite about its center.
	 *
	 * @param origin: The sprite origin; the default is (0,0) which represents the upper-left corner.
	 *
	 * @param layerDepth: The depth of a layer. By default, 0 represents the front layer and 1 represents
	 * a back layer. Use SpriteSortMode if you want sprites to be sorted during drawing.
	 */
	void Draw(const shared_ptr<Texture>& texture, const IntRect& destinationRectangle, IntRect* sourceRectangle, const ColorRGBA& color, float rotation, const Vector2f& origin, float layerDepth);

	void Draw(const shared_ptr<Texture>& texture, const Vector2f& position, IntRect* sourceRectangle, const ColorRGBA& color, float rotation, const Vector2f& origin, float scale, float layerDepth);
	void Draw(const shared_ptr<Texture>& texture, const Vector2f& position, IntRect* sourceRectangle, const ColorRGBA& color, float rotation, const Vector2f& origin, const Vector2f& scale, float layerDepth);

	void Draw(const shared_ptr<Texture>& texture, const IntRect& destinationRectangle, const ColorRGBA& color);
	void Draw(const shared_ptr<Texture>& texture, const IntRect& destinationRectangle, IntRect* sourceRectangle, const ColorRGBA& color);
	
	void Draw(const shared_ptr<Texture>& texture, const Vector2f& position, const ColorRGBA& color);
	void Draw(const shared_ptr<Texture>& texture, const Vector2f& position, IntRect* sourceRectangle, const ColorRGBA& color);

private:
	shared_ptr<Material> mMaterial;
	shared_ptr<Texture> mTexture;

	Matrix4f mProjectionMatrix;
	uint32_t mSortMode;
	shared_ptr<Material> mSpriteMaterial;

	std::map<shared_ptr<Texture>, SpriteEntity*> mBatches;
};


}

#endif // SpriteBatch_h__
