#ifndef Font_h__
#define Font_h__

#include <Core/Prerequisites.h>
#include <Resource/Resource.h>
#include <Math/ColorRGBA.h>
#include <Math/Vector.h>


static const int FONT_TEXTURE_MIN_SIZE = 128;
static const int FONT_TEXTURE_MAX_SIZE = 2048;
static const int FONT_DPI = 96;

namespace RcEngine {

struct _ApiExport FontGlyph
{
	int32_t X, Y;
	int32_t Width, Height;
	// Glyph X offset from origin.
	int32_t OffsetX;
	// Glyph Y offset from origin.
	int32_t OffsetY;
	// the horizontal advance value for the glyph. 
	int32_t Advance;

	unordered_map<uint32_t, uint32_t> Kerning;
};

class SpriteBatch;

class _ApiExport FontFace 
{
public:
	FontFace();
	~FontFace();

	const FontGlyph& GetGlyph(uint32_t ch) const;
	uint32_t GetKerning(uint32_t c, uint32_t d) const;

	shared_ptr<Texture> Texture;
	vector<FontGlyph> FontGlyphs;

	uint32_t PointSize;
	uint32_t RowHeight;

	unordered_map<uint32_t, uint32_t> GlyphMapping;

	bool HasKerning;
};

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

public:
	Font(uint32_t resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~Font();

	void DrawText(std::wstring& text, uint32_t fontSize, const Vector2f& position, const ColorRGBA& color);
	void DrawText(std::wstring& text, uint32_t fontSize, const Vector2f& position, const ColorRGBA& color, float rotation, const Vector2f& origin, float scale, float layerDepth);
	void DrawText(std::wstring& text, uint32_t fontSize, const Vector2f& position, const ColorRGBA& color, float rotation, const Vector2f& origin, const Vector2f& scale, float layerDepth);

	void MeasureText(const std::wstring& text,uint32_t size, uint32_t* widthOut, uint32_t* heightOut);

	const FontFace* GetFace(uint32_t pointSize);

private:
	void Draw(SpriteBatch* spriteBatch, const std::wstring& text, const Vector2f& Position, const ColorRGBA& color, float rotation, const Vector2f&  origin, const Vector2f& scale, float depth);

protected:
	void LoadImpl();
	void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:
	vector<uint8_t> mFontData;
	uint32_t mFontDataSize;
	unordered_map<uint32_t, FontFace*> mFontFaces;
};


}


#endif // Font_h__
