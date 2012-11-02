#include <Graphics/Font.h>
#include <Graphics/Texture.h>
#include <Graphics/RenderFactory.h>
#include <Core/Context.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <Math/Rectangle.h>
#include <Resource/ResourceManager.h>


#include <ft2build.h>
#include FT_FREETYPE_H

#pragma comment(lib, "freetype.lib")

namespace {

using namespace RcEngine;

class FreeTypeLibrary
{
public:
	static FreeTypeLibrary& GetSingleton()
	{
		static FreeTypeLibrary library;
		return library;
	}

	FT_Library& GetLibrary()	{ return mLibrary; }

private:
	FreeTypeLibrary()
	{
		FT_Error error = FT_Init_FreeType(&mLibrary);
		if (error)
		{
			ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Could not initialize FreeType library", "FreeTypeLibrary");
		}
	}

	/// Destruct.
	~FreeTypeLibrary()
	{
		FT_Done_FreeType(mLibrary);
	}

private:
	/// FreeType library.
	FT_Library mLibrary;
};

class AreaAllocator
{
public:
	AreaAllocator(int32_t width, int32_t height)
	{
		Reset(width, height);
	}

	void Reset(int32_t width, int32_t height)
	{
		mFreeAreas.clear();
		mFreeAreas.push_back(IntRect(0, 0, width, height));
	}

	bool Allocate(int width, int height, int& x, int& y)
	{
		if (width < 0)	width = 0;
		if (height < 0)	height = 0;

		std::vector<IntRect>::iterator best = mFreeAreas.end();
		int32_t bestFreeArea = (std::numeric_limits<int32_t>::max)();

		for (auto iter = mFreeAreas.begin(); iter != mFreeAreas.end(); ++iter)
		{
			int freeWidth = iter->Right() - iter->Left();
			int freeHeight = iter->Bottom() - iter->Top();

			if (width <= freeWidth && height <= freeHeight)
			{
				int32_t freeArea = freeWidth * freeHeight;

				if (freeArea < bestFreeArea)
				{
					bestFreeArea = freeArea;
					best = iter;
				}
			}
		}

		if (best == mFreeAreas.end())
			return false;

		IntRect reserved;
		reserved.X = best->Left();
		reserved.Y = best->Top();
		reserved.Width = width;
		reserved.Height = height;

		x = best->Left();
		y = best->Top();


		std::vector<IntRect> splitRects;

		// Remove the reserved area from all free areas
		for (size_t i = 0; i < mFreeAreas.size(); i++)
		{
			IntRect& original = mFreeAreas[i];

			bool split = false; 

			if (reserved.Right() > original.Left() && reserved.Left() < original.Right() && 
				reserved.Bottom() > original.Top() && reserved.Top() < original.Bottom())
			{
				// Check for splitting from the right
				if (reserved.Right() < original.Right()) 
				{
					IntRect newRect = original;
					newRect.SetLeft(reserved.Right());
					newRect.SetRight(original.Right());
					splitRects.push_back(newRect);
					split = true;
				}
				// Check for splitting from the left
				if (reserved.Left() > original.Left())
				{
					IntRect newRect = original;
					newRect.SetRight(reserved.Left());
					splitRects.push_back(newRect);
					split = true;
				}
				// Check for splitting from the bottom
				if (reserved.Bottom() < original.Bottom())
				{
					IntRect newRect = original;
					newRect.SetTop(reserved.Bottom());
					newRect.SetBottom(original.Bottom());
					splitRects.push_back(newRect);
					split = true;
				}
				// Check for splitting from the top
				if (reserved.Top() > original.Top())
				{
					IntRect newRect = original;
					newRect.SetBottom(reserved.Top());
					splitRects.push_back(newRect);
					split = true;
				}
			}

			if (!split)
			{
				splitRects.push_back(original);
			}
		}

		// Remove rects which are contained within another rect
		for (size_t i = 0; i < splitRects.size(); )
		{
			bool erased = false;
			for (size_t j = i + 1; j < splitRects.size(); )
			{
				if ((splitRects[i].Left() >= splitRects[j].Left()) &&
					(splitRects[i].Top() >= splitRects[j].Top()) &&
					(splitRects[i].Right() <= splitRects[j].Right()) &&
					(splitRects[i].Bottom() <= splitRects[j].Bottom()))
				{
					splitRects.erase(splitRects.begin() + i);
					erased = true;
					break;
				}
				if ((splitRects[j].Left() >= splitRects[i].Left()) &&
					(splitRects[j].Top() >= splitRects[i].Top()) &&
					(splitRects[j].Right() <= splitRects[i].Right()) &&
					(splitRects[j].Bottom() <= splitRects[i].Bottom()))
					splitRects.erase(splitRects.begin() + j);
				else
					++j;
			}
			if (!erased)
				++i;
		}

		mFreeAreas.swap(splitRects);

		return true;
	}

private:
	std::vector<IntRect> mFreeAreas;
};


}


namespace RcEngine {


FontFace::FontFace()
{

}

FontFace::~FontFace()
{

}

const FontGlyph& FontFace::GetGlyph( uint32_t c ) const
{
	auto found = GlyphMapping.find(c);
	if (found != GlyphMapping.end())
		return FontGlyphs[found->second];
	else
		return FontGlyphs[0];
}

////////////////////////////////////////////////////////////////////////////
Font::Font( uint32_t resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(resType, creator, handle, name, group)
{

}

Font::~Font()
{

}

void Font::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();

	shared_ptr<Stream> stream = fileSystem.OpenStream(mName, mGroup);
	Stream& source = *stream;	

	mFontDataSize = source.GetSize();

	mFontData.resize(mFontDataSize);
	if ( mFontDataSize != source.Read(&mFontData[0], mFontDataSize) )
	{
		assert (false);
	}	
}

void Font::UnloadImpl()
{

}

const FontFace* Font::GetFace( uint32_t pointSize )
{
	auto found = mFontFaces.find(pointSize);
	if (found != mFontFaces.end())
	{
		return found->second;
	}

	FT_Face face;
	FT_Error error;

	FT_Library& library = FreeTypeLibrary::GetSingleton().GetLibrary();

	if (!mFontDataSize)
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Font not loaded", "Font::GetFace");
		return nullptr;
	}

	error = FT_New_Memory_Face(library, &mFontData[0], mFontDataSize, 0, &face);
	if (error)
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Could not create font face", "Font::GetFace");
		return nullptr;
	}

	FontFace* fontFace = new FontFace;


	FT_F26Dot6 ftSize = (FT_F26Dot6)(pointSize * (1 << 6));
	error = FT_Set_Char_Size(face, 0, ftSize, FONT_DPI, FONT_DPI);
	if (error)
	{
		FT_Done_Face(face);
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Could not set font point size:" + ToString(pointSize), "Font::GetFace");
	}


	FT_GlyphSlot slot = face->glyph;
	uint32_t numGlyphs = 0;

	// Build glyph mapping
	FT_UInt glyphIndex;
	FT_ULong charCode = FT_Get_First_Char(face, &glyphIndex);
	while (glyphIndex != 0)
	{
		numGlyphs = (std::max)((int)glyphIndex + 1, (int)numGlyphs);
		fontFace->GlyphMapping[charCode] = glyphIndex;
		charCode = FT_Get_Next_Char(face, charCode, &glyphIndex);
	}

	// Load each of the glyphs to see the sizes & store other information
	int32_t maxOffsetY = 0;
	int32_t maxHeight = 0;

	auto h1 = face->size->metrics.height>>6;
	auto h2 = face->size->metrics.ascender >> 6;

	for (uint32_t i = 0; i < numGlyphs; ++i)
	{
		FontGlyph newGlyph;

		error = FT_Load_Glyph(face, i, FT_LOAD_DEFAULT);
		if (!error)
		{
			// Note: position within texture will be filled later
			newGlyph.Width = (int32_t)((slot->metrics.width) >> 6);
			newGlyph.Height = (int32_t)((slot->metrics.height) >> 6);
			newGlyph.OffsetX = (int32_t)((slot->metrics.horiBearingX) >> 6);
			newGlyph.OffsetY = (int32_t)((face->size->metrics.ascender - slot->metrics.horiBearingY) >> 6);
			newGlyph.Advance = (int32_t)((slot->metrics.horiAdvance) >> 6);

			maxOffsetY =  (std::max)(maxOffsetY, newGlyph.OffsetY);
			maxHeight = (std::max)(maxHeight, newGlyph.Height);
		}
		else
		{
			newGlyph.Width = 0;
			newGlyph.Height = 0;
			newGlyph.OffsetX = 0;
			newGlyph.OffsetY = 0;
			newGlyph.Advance = 0;
		}

		fontFace->FontGlyphs.push_back(newGlyph);
	}

	// Store kerning if face has kerning information
	if (FT_HAS_KERNING(face))
	{
		fontFace->HasKerning = true;
		size_t numGlyphs = fontFace->FontGlyphs.size();

		for (size_t i = 0; i < numGlyphs; ++i)
		{
			for (size_t j = 0; j < numGlyphs; ++j)
			{
				FT_Vector vector;
				FT_Get_Kerning(face, i, j, FT_KERNING_DEFAULT, &vector);
				fontFace->FontGlyphs[i].Kerning[j] = (uint32_t)(vector.x >> 6);		
			}
		}
	}

	// Store point size and the height of a row. Use the height of the tallest font if taller than the specified row height
	fontFace->PointSize = pointSize;
	fontFace->RowHeight = (std::max)((int32_t)(face->size->metrics.height + 63) >> 6, maxHeight);


	// Now try to pack into the smallest possible texture
	int texWidth = FONT_TEXTURE_MIN_SIZE;
	int texHeight = FONT_TEXTURE_MIN_SIZE;
	bool doubleHorizontal = true;

	for (;;)
	{
		bool success = true;

		// Check first for theoretical possible fit. If it fails, there is no need to try to fit
		int totalArea = 0;
		for (size_t i = 0; i < fontFace->FontGlyphs.size(); ++i)
			totalArea += (fontFace->FontGlyphs[i].Width + 1) * (fontFace->FontGlyphs[i].Height + 1);

		if (totalArea > texWidth * texHeight)
			success = false;
		else
		{
			AreaAllocator allocator(texWidth, texHeight);
			for (size_t i = 0; i < fontFace->FontGlyphs.size(); ++i)
			{
				if (fontFace->FontGlyphs[i].Width && fontFace->FontGlyphs[i].Height)
				{
					int32_t x, y;
					// Reserve an empty border between glyphs for filtering
					if (!allocator.Allocate(fontFace->FontGlyphs[i].Width + 1, fontFace->FontGlyphs[i].Height + 1, x, y))
					{
						success = false;
						break;
					}
					else
					{
						fontFace->FontGlyphs[i].X = x;
						fontFace->FontGlyphs[i].Y = y;
					}
				}
				else
				{
					fontFace->FontGlyphs[i].X = 0;
					fontFace->FontGlyphs[i].Y = 0;
				}
			}
		}

		if (!success)
		{
			// Alternate between doubling the horizontal and the vertical dimension
			if (doubleHorizontal)
				texWidth <<= 1;
			else
				texHeight <<= 1;

			if (texWidth > FONT_TEXTURE_MAX_SIZE || texHeight > FONT_TEXTURE_MAX_SIZE)
			{
				FT_Done_Face(face);
				ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Font face could not be fit into the largest possible texture", "Font::GetFace");
				return nullptr;
			}
			doubleHorizontal = !doubleHorizontal;
		}
		else
			break;
	}

	vector<unsigned char> fontTextureData(texHeight * texWidth);
	memset(&fontTextureData[0], 0, sizeof(unsigned char) * texHeight * texWidth);



	// Render glyphs into texture, and find out a scaling value in case font uses less than full opacity (thin outlines)
	unsigned char avgMaxOpacity = 255;
	unsigned sumMaxOpacity = 0;
	unsigned samples = 0;
	for (size_t i = 0; i < fontFace->FontGlyphs.size(); ++i)
	{
		FontGlyph& glyph = fontFace->FontGlyphs[i];

		if (!glyph.Width || !glyph.Height)
			continue;

		FT_Load_Glyph(face, i, FT_LOAD_DEFAULT);
		FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

		unsigned char glyphOpacity = 0;
		for (int32_t y = 0; y < glyph.Height; ++y)
		{
			unsigned char* src = slot->bitmap.buffer + slot->bitmap.pitch * y;
			unsigned char* dest = &fontTextureData[0] + texWidth * (y + glyph.Y) + glyph.X;

			for (int32_t x = 0; x < glyph.Width; ++x)
			{
				dest[x] = src[x];
				glyphOpacity = (std::max)(glyphOpacity, src[x]);
			}
		}
		if (glyphOpacity)
		{
			sumMaxOpacity += glyphOpacity;
			++samples;
		}
	}

	// Clamp the minimum possible value to avoid overbrightening
	if (samples)
		avgMaxOpacity = (std::max)(sumMaxOpacity / samples, (unsigned)128);

	if (avgMaxOpacity < 255)
	{
		// Apply the scaling value if necessary
		float scale = 255.0f / avgMaxOpacity;
		for (unsigned i = 0; i < fontFace->FontGlyphs.size(); ++i)
		{
			for (int y = 0; y < fontFace->FontGlyphs[i].Height; ++y)
			{
				unsigned char* dest =  &fontTextureData[0] + texWidth * (y + fontFace->FontGlyphs[i].Y) + fontFace->FontGlyphs[i].X;
				for (int x = 0; x < fontFace->FontGlyphs[i].Width; ++x)
				{
					int pixel = dest[x];
					dest[x] = (std::min)((int)(pixel * scale), 255);
				}
			}
		}
	}

	FT_Done_Face(face);

	// Create font texture
	ElementInitData initData;
	initData.pData = &fontTextureData[0];
	initData.rowPitch = sizeof(unsigned char) * fontTextureData.size();
	fontFace->Texture = Context::GetSingleton().GetRenderFactory().CreateTexture2D(texWidth, texHeight, PF_Alpha8,
		1, 0, 1, 0, 0, &initData);

	Context::GetSingleton().GetRenderFactory().SaveTexture2D("E:/Font.pfm", fontFace->Texture, 0, 0);

	mFontFaces[pointSize] = fontFace;

	return fontFace;
}

shared_ptr<Resource> Font::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<Font>(ResourceTypes::Font, creator, handle, name, group);
}





}