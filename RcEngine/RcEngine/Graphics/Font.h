#ifndef Font_h__
#define Font_h__

#include <Core/Prerequisites.h>
#include <Resource/Resource.h>


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


class _ApiExport FontFace 
{
public:
	FontFace();
	~FontFace();

	const FontGlyph& GetGlyph(uint32_t c) const;

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
	Font(uint32_t resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~Font();

	const FontFace* GetFace(uint32_t pointSize);

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
