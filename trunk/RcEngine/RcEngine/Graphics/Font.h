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
		int32_t OffsetX, OffsetY;
		// the horizontal advance value for the glyph. 
		int32_t Advance;
	};

	struct CharInfo
	{
		Rectanglef TexRect;
		uint64_t Tick;
	};

public:
	Font(uint32_t resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~Font();

	void DrawText(std::wstring& text, uint32_t fontSize, const Vector2f& position, const ColorRGBA& color);
	void DrawText(std::wstring& text, uint32_t fontSize, const Vector2f& position, const ColorRGBA& color, float rotation, const Vector2f& origin, float scale, float layerDepth);
	void DrawText(std::wstring& text, uint32_t fontSize, const Vector2f& position, const ColorRGBA& color, float rotation, const Vector2f& origin, const Vector2f& scale, float layerDepth);

	void MeasureText(const std::wstring& text,uint32_t fontHeight, uint32_t* widthOut, uint32_t* heightOut);

private:
	void Draw(SpriteBatch* spriteBatch, const std::wstring& text, const Vector2f& Position, const ColorRGBA& color, float rotation, const Vector2f&  origin, const Vector2f& scale, float depth);

	// map wchat_t to glyph index
	int32_t GetGlyphIndex(wchar_t ch);

	// kerning between chars
	int32_t GetKerning(wchar_t c, wchar_t d) const;

	void UpdateTexture(const std::wstring& text);

protected:
	void LoadImpl();
	void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:
	vector<uint8_t> mFontData;
	uint32_t mFontDataSize;
	//unordered_map<uint32_t, FontFace*> mFontFaces;

	bool mHasKerning;
	bool mOutline;

	// y of base line
	uint32_t mBase;

	// total height of the font
	uint32_t mCharSize;


	float mScale;

	// all character glyphs
	vector<Glyph> mGlyphs;

	std::map<std::pair<uint32_t, uint32_t>, int32_t> mKerning;

	// wchar_t to glyph index map
	unordered_map<uint32_t, int32_t> mGlyphMapping;
	
	// cached glyph in current texture
	unordered_map<wchar_t, CharInfo> mCharacterCached;

	// free slots in font texture,  [first, second]
	std::list<std::pair<uint32_t, uint32_t> > mFreeCharacterSlots;

	vector<uint8_t> mDistancesLzma;

	// distance data offset in mDistancesLzma
	vector<uint32_t> mGlyphDistanceOffset;


	shared_ptr<Texture> mDistanceTexture;
};


}


#endif // Font_h__
