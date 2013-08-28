#ifndef Font_h__
#define Font_h__

#include <Core/Prerequisites.h>
#include <Resource/Resource.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>
#include <Math/Vector.h>

namespace RcEngine {

class SpriteBatch;
class FontLoader;

class _ApiExport Font : public Resource
{
public:

	struct Glyph
	{
		// Start Position in Texture
		int32_t SrcX, SrcY;

		int32_t Width, Height;

		// Glyph X, Y offset from origin.
		float OffsetX, OffsetY;

		// the horizontal advance value for the glyph. 
		float Advance;	
	};

	typedef std::unordered_map<wchar_t, Glyph> FontMetrics;

public:
	Font(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~Font();

	void DrawString(SpriteBatch& spriteBatch, std::wstring& text, int32_t fontSize, const float2& position, const ColorRGBA& color);
	
	void DrawString(SpriteBatch& spriteBatch, std::wstring& text, int32_t fontSize, uint32_t alignment, const Rectanglef& region, const ColorRGBA& color);

	//void DrawStringWrap(SpriteBatch& spriteBatch, std::wstring& text, int32_t fontSize, int32_t maxWidth, const float2& position, const ColorRGBA& color);

	//void DrawText(std::wstring& text, uint32_t fontSize, const float2& position, const ColorRGBA& color, float rotation, const float2& origin, float scale, float layerDepth);
	//void DrawText(std::wstring& text, uint32_t fontSize, const float2& position, const ColorRGBA& color, float rotation, const float2& origin, const float2& scale, float layerDepth);

	void MeasureString(const std::wstring& text, int32_t fontSize, float* widthOut, float* heightOut);

	int32_t GetFontSize() const { return mCharSize; }
	inline float GetRowHeight(float yScale = 1.0) const  	{ return mRowHeight * yScale; }
	inline float GetBaseLine(float yScale = 1.0) const	    { return mAscent * yScale; }


	const shared_ptr<Texture>& GetFontTexture() const { return mFontTexture; }

	/**
	 * Get row height of specified font size
	 */
	float GetRowHeight(int32_t fontSize) const;
	
	const FontMetrics& GetFontMetrics() const { return mGlyphs; }

	const Glyph& GetGlyphInfo(wchar_t ch) const;

protected:
	void LoadImpl();
	void UnloadImpl();

	void LoadTXT(const String& fileName);
	void LoadBinary(const String& fileName);

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:

	String mFontName;

	FontMetrics mGlyphs;

	int32_t mCharSize;

	// maximal font ascender (pixels above the baseline)
	float mAscent;
	// maximal font descender (negative pixels below the baseline)
	float mDescent;
	// Row height. (ascender - descender) 
	float mRowHeight;

	shared_ptr<Texture> mFontTexture;
};


}


#endif // Font_h__
