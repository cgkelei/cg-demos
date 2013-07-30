#ifndef Font_h__
#define Font_h__

#include <Core/Prerequisites.h>
#include <Resource/Resource.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>
#include <Math/Vector.h>


static const int FONT_TEXTURE_MIN_SIZE = 128;
static const int FONT_TEXTURE_MAX_SIZE = 2048;
static const int FONT_DPI = 96;

namespace RcEngine {

class SpriteBatch;
class FontLoader;

class _ApiExport Font : public Resource
{
public:
	enum Alignment
	{
		Align_Hor_Left = 1UL << 0,
		Align_Hor_Center = 1UL << 1,
		Align_Hor_Right = 1UL << 2,

		Align_Ver_Top = 1UL << 3,
		Align_Ver_Center = 1UL << 4,
		Align_Ver_Bottom= 1UL << 5,
	};

	struct Glyph
	{
		int32_t Width, Height;
		// Glyph X, Y offset from origin.
		float OffsetX, OffsetY;
		// the horizontal advance value for the glyph. 
		float Advance;

		// Start Position in Texture
		int32_t SrcX, SrcY;
	};

public:
	Font(ResourceTypes resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~Font();

	void DrawString(SpriteBatch& spriteBatch, std::wstring& text, uint32_t fontSize, const float2& position, const ColorRGBA& color);
	
	//void DrawText(std::wstring& text, uint32_t fontSize, const float2& position, const ColorRGBA& color, float rotation, const float2& origin, float scale, float layerDepth);
	//void DrawText(std::wstring& text, uint32_t fontSize, const float2& position, const ColorRGBA& color, float rotation, const float2& origin, const float2& scale, float layerDepth);

	void MeasureString(const std::wstring& text,uint32_t fontSize, uint32_t* widthOut, uint32_t* heightOut);

	int32_t GetFontSize() const   { return mCharSize; }
	int32_t GetRowHeight() const  { return mRowHeight; }

	/**
	 * Get row height of specified font size
	 */
	int32_t GetRowHeight(int32_t fontSize) const;

protected:
	void LoadImpl();
	void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:

	friend class FontLoader;
	
	String mFontName;

	/// Point size.
	int32_t mCharSize;

	/// Row height.
	int32_t mRowHeight;

	std::unordered_map<uint32_t, Glyph> mGlyphs;

	shared_ptr<Texture> mFontTexture;
};


}


#endif // Font_h__
