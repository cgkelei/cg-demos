#include <Graphics/Font.h>
#include <Graphics/Texture.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/SpriteBatch.h>
#include <Core/Context.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <GUI/GuiSkin.h>
#include <Math/Rectangle.h>
#include <Resource/ResourceManager.h>

namespace {

using RcEngine::Font;

int SkipWhiteSpace( std::string &str, int start )
{
	size_t n = start;
	while( n < str.size() )
	{
		char ch = str[n];
		if( ch != ' ' && 
			ch != '\t' && 
			ch != '\r' && 
			ch != '\n' )
			break;

		++n;
	}

	return n;
}

int FindEndOfToken( std::string &str, int start )
{
	size_t n = start;
	if( str[n] == '"' )
	{
		n++;
		while( n < str.size() )
		{
			char ch = str[n];
			if( ch == '"' )
			{
				// Include the last quote char in the token
				++n;
				break;
			}
			++n;
		}
	}
	else
	{
		while( n < str.size() )
		{
			char ch = str[n];
			if( ch == ' ' ||
				ch == '\t' ||
				ch == '\r' ||
				ch == '\n' ||
				ch == '=' )
				break;

			++n;
		}
	}

	return n;
}

int32_t InterpretChar( std::string &str, int start, Font::Glyph& glyph)
{
	// Read all attributes
	int id = 0;
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	float xoffset = 0;
	float yoffset = 0;
	float xadvance = 0;
	int page = 0;
	int chnl = 0;

	int pos, pos2 = start;
	while( true )
	{
		pos = SkipWhiteSpace(str, pos2);
		pos2 = FindEndOfToken(str, pos);

		std::string token = str.substr(pos, pos2-pos);

		pos = SkipWhiteSpace(str, pos2);
		if( pos == str.size() || str[pos] != '=' ) break;

		pos = SkipWhiteSpace(str, pos+1);
		pos2 = FindEndOfToken(str, pos);

		std::string value = str.substr(pos, pos2-pos);

		if( token == "id" )
			id = strtol(value.c_str(), 0, 10);
		else if( token == "x" )
			x = strtol(value.c_str(), 0, 10);
		else if( token == "y" )
			y = strtol(value.c_str(), 0, 10);
		else if( token == "width" )
			width = strtol(value.c_str(), 0, 10);
		else if( token == "height" )
			height = strtol(value.c_str(), 0, 10);
		else if( token == "xoffset" )
			xoffset = (float)atof(value.c_str());
		else if( token == "yoffset" )
			yoffset = (float)atof(value.c_str());
		else if( token == "xadvance" )
			xadvance = (float)atof(value.c_str());
		else if( token == "page" )
			page = strtol(value.c_str(), 0, 10);
		else if( token == "chnl" )
			chnl = strtol(value.c_str(), 0, 10);

		if( pos2 == str.size() ) 
			break;
	}

	glyph.SrcX = x;
	glyph.SrcY = y;
	glyph.Width = width;
	glyph.Height = height;
	glyph.Advance = xadvance;
	glyph.OffsetX = xoffset;
	glyph.OffsetY = yoffset;

	return id;
}

int32_t InterpretChars( std::string &str, int start )
{
	int pos, pos2 = start;
	while( true )
	{
		pos = SkipWhiteSpace(str, pos2);
		pos2 = FindEndOfToken(str, pos);

		std::string token = str.substr(pos, pos2-pos);

		pos = SkipWhiteSpace(str, pos2);
		if( pos == str.size() || str[pos] != '=' ) break;

		pos = SkipWhiteSpace(str, pos+1);
		pos2 = FindEndOfToken(str, pos);

		std::string value = str.substr(pos, pos2-pos);

		if (token == "count")
			return (size_t)strtol(value.c_str(), 0, 10);

		if( pos2 == str.size() ) break;
	}

	return 0;
}

std::string InterpretInfo( std::string &str, int start )
{
	std::string fontName;

	int pos, pos2 = start;
	while( true )
	{
		pos = SkipWhiteSpace(str, pos2);
		pos2 = FindEndOfToken(str, pos);

		std::string token = str.substr(pos, pos2-pos);

		pos = SkipWhiteSpace(str, pos2);
		if( pos == str.size() || str[pos] != '=' ) break;

		pos = SkipWhiteSpace(str, pos+1);
		pos2 = FindEndOfToken(str, pos);

		std::string value = str.substr(pos, pos2-pos);

		if (token == "face")
			return value;

			
		if( pos2 == str.size() ) break;
	}

	return std::string("");
}

}

namespace RcEngine {

Font::Font(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Font, creator, handle, name, group), mRowHeight(0.0f), mDescent(0.0f), mAscent(0.0f)
{

}


////////////////////////////////////////////////////////////////////////////

Font::~Font()
{

}

void Font::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	// font texture
	shared_ptr<TextureResource> fontTexture = std::static_pointer_cast<TextureResource>(resMan.GetResourceByName(RT_Texture, mName + ".sdff.png", mGroup));
	fontTexture->Load();
	mFontTexture = fontTexture->GetTexture();

	// description file
	String filePath = fileSystem.Locate(mName + ".sdff.txt", mGroup)  ;

	if (filePath.find(".txt") != std::string::npos)
		LoadTXT(filePath);
	else
		LoadBinary(filePath);
}

void Font::UnloadImpl()
{

}

void Font::LoadTXT(const String& fileName)
{
	std::string line;

	int32_t numChars;
	
	Glyph charGlyph;

	std::ifstream file(fileName);
	while (std::getline(file, line))
	{
		// Skip white spaces
		int pos = SkipWhiteSpace(line, 0);
		
		// Read token
		int pos2 = FindEndOfToken(line, pos);
		std::string token = line.substr(pos, pos2-pos);
		
		// Interpret line
		if( token == "info" )
			mFontName = InterpretInfo(line, pos2);
		else if( token == "chars" )
			numChars = InterpretChars(line, pos2);
		else if( token == "char" )
		{
			int32_t unicode = InterpretChar(line, pos2, charGlyph);
			mGlyphs[unicode] = charGlyph;

			mAscent = (std::max)(mAscent, charGlyph.OffsetY);
			mDescent = (std::min)(mDescent, charGlyph.OffsetY - charGlyph.Height);
		}
	}

	mCharSize = 68;

	assert(numChars == mGlyphs.size());

	mRowHeight = mAscent - mDescent;
}

void Font::DrawString(SpriteBatch& spriteBatch, std::wstring& text, int32_t fontSize, const float2& position, const ColorRGBA& color)
{
	float x = position.X();
	float y = position.Y();

	const float scale = float(fontSize) / float(mCharSize);

	for (auto iter = text.begin(); iter != text.end(); ++iter)
	{
		wchar_t ch = *iter;

		if (ch == L'\n')
		{
			y += mRowHeight * scale;
			x = position.X();
		}
		else
		{
			const Glyph& glyph = mGlyphs[ch];

			float ch_x = x + glyph.OffsetX * scale;
			float ch_y = y - glyph.OffsetY * scale;

			IntRect sourceRect(glyph.SrcX, glyph.SrcY, glyph.Width, glyph.Height);
			Rectanglef destRect(ch_x, ch_y, glyph.Width * scale, glyph.Height* scale);
			
			spriteBatch.Draw(mFontTexture, destRect, &sourceRect, color);
		
			x += glyph.Advance * scale;
		}
	}
}

void Font::MeasureString( const std::wstring& text, int32_t fontSize, float* widthOut, float* heightOut )
{
	const float scale = float(fontSize) / mCharSize;

	std::vector<float> rowWidths(1, 0);

	for (size_t i = 0; i < text.length(); ++i)
	{
		wchar_t ch = text[i];

		if (ch != L'\n')
			rowWidths.back() += mGlyphs[ch].Advance * scale;
		else
			rowWidths.push_back(0);
	}

	if(widthOut)  *widthOut = *std::max_element(rowWidths.begin(), rowWidths.end());
	if(heightOut) *heightOut = rowWidths.size() * mRowHeight * scale;

}

float Font::GetRowHeight( int32_t fontSize ) const
{
	return mRowHeight * float(fontSize) / mCharSize;
}

shared_ptr<Resource> Font::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<Font>(creator, handle, name, group);
}

const Font::Glyph& Font::GetGlyphInfo( wchar_t ch ) const
{
	FontMetrics::const_iterator it = mGlyphs.find(ch);

	if (it == mGlyphs.end())
		return (mGlyphs.begin())->second;
	else
		return it->second;
}

static float GetRowStartPos(float rowWidth, float maxWidth, uint32_t alignment)
{
	if (alignment & AlignLeft)
		return 0;
	else if (alignment & AlignCenter)
		return (maxWidth - rowWidth) / 2;
	else if (alignment & AlignRight)
		return (maxWidth - rowWidth);
	else
		return 0;
}

void Font::DrawString( SpriteBatch& spriteBatch, std::wstring& text, int32_t fontSize, uint32_t alignment, const Rectanglef& region, const ColorRGBA& color )
{
	const float scale = float(fontSize) / mCharSize;
	const float rowHeight = mRowHeight * scale;

	std::vector<float> rowWidths(1, 0);

	// Compute width and height to draw this text
	for (size_t i = 0; i < text.length(); ++i)
	{
		wchar_t ch = text[i];

		if (ch != L'\n')
			rowWidths.back() += mGlyphs[ch].Advance * scale;
		else
			rowWidths.push_back(0);
	}

	float width = *std::max_element(rowWidths.begin(), rowWidths.end());
	float height = rowWidths.size() * rowHeight;

	size_t rowIdx = 0;

	float x = region.Left() + GetRowStartPos(rowWidths[rowIdx], region.Width, alignment);
	float y;

	if (alignment & AlignTop)
	{
		y = region.Top() + GetBaseLine(scale);
	}
	else if (alignment & AlignVCenter)
	{
		y = region.Top() + (region.Height - height) * 0.5f + GetBaseLine(scale);
	}
	else if (alignment & AlignBottom)
	{
		y = (region.Bottom() - height) + GetBaseLine(scale);
	}
	else
		y = region.Top() + GetBaseLine(scale);

	for (size_t i = 0; i < text.length(); ++i)
	{
		wchar_t ch = text[i];

		if (ch == L'\n')
		{
			y += rowHeight;

			rowIdx++;
			x = region.Left() + GetRowStartPos(rowWidths[rowIdx], region.Width, alignment);
		}
		else
		{
			const Glyph& glyph = mGlyphs[ch];

			float ch_x = x + glyph.OffsetX * scale;
			float ch_y = y - glyph.OffsetY * scale;

			float ch_width = glyph.Width * scale;
			float ch_height = glyph.Height* scale;

			x += glyph.Advance * scale;

			// Out of region
			if ( (ch_x + ch_width <= region.Left()) ||
				(ch_x >= region.Right())            ||
				(ch_y >= region.Bottom())           ||
				(ch_y + ch_height <= region.Top()) )
				continue;

			IntRect sourceRect;
			Rectanglef destRect;

			if (ch_x < region.Left())
			{
				float ratio = (region.Left() - ch_x) / ch_width;
				sourceRect.X = glyph.SrcX + int(glyph.Width * ratio);
				sourceRect.Width = int(glyph.Width * (1 - ratio));

				destRect.X = region.Left();
				destRect.Width = ch_height * (1 - ratio);
			}
			else if (ch_x + ch_width > region.Right())
			{
				float ratio = (ch_x + ch_width  - region.Right()) / ch_width;
				sourceRect.X = glyph.SrcX;
				sourceRect.Width = int(glyph.Width * (1 - ratio));

				destRect.X = ch_x;
				destRect.SetRight(region.Right());
			}
			else
			{
				sourceRect.X = glyph.SrcX;
				sourceRect.Width = glyph.Width;

				destRect.X = ch_x;
				destRect.Width = ch_width;
			}

			if (ch_y < region.Top())
			{
				float ratio = (region.Top() - ch_y) / ch_height;

				sourceRect.Y = glyph.SrcY + int(glyph.Height * ratio);
				sourceRect.Height = int(glyph.Height * (1 - ratio));

				destRect.Y = region.Top();
				destRect.Height = ch_height * (1 - ratio);
			}
			else if (ch_y + ch_height > region.Bottom())
			{
				float ratio = (ch_y + ch_height - region.Bottom()) / ch_height;

				sourceRect.Y = glyph.SrcY;
				sourceRect.Height = int(glyph.Height * (1 - ratio));

				destRect.Y = ch_y;
				destRect.SetBottom( region.Bottom() );
			}
			else
			{
				sourceRect.Y = glyph.SrcY;
				sourceRect.Height = glyph.Height;

				destRect.Y = ch_y;
				destRect.Height = ch_height;
			}

			spriteBatch.Draw(mFontTexture, destRect, &sourceRect, color);
		}
	}
}

void Font::LoadBinary( const String& fileName )
{

}

//void Font::DrawStringWrap( SpriteBatch& spriteBatch, std::wstring& text, int32_t fontSize, int32_t maxWidth, const float2& position, const ColorRGBA& color )
//{	
//	std::wstring mPrintText;
//
//	std::vector<size_t> printToText;
//
//	// Font Scale
//	const float scale = float(fontSize) / float(mCharSize);
//
//	int32_t rowWidth = 0;
//	int32_t rowHeight = mRowHeight;
//
//	printToText.reserve(mPrintText.length());
//
//	size_t nextBreak = 0;
//	size_t lineStart = 0;
//
//	size_t i, j;
//	for (i = 0; i < text.length(); ++i)
//	{
//		wchar_t ch = text[i];
//
//		if (ch != L'\n')
//		{
//			bool ok = true;
//
//			if (nextBreak <= i)
//			{
//				int32_t futureRowWidth = rowWidth;
//
//				// Find next blank or line break
//				for (j = i; j < text.length(); ++j)
//				{
//					if (text[j] == L' ' || text[j] == L'\n')
//					{
//						nextBreak = j;
//						break;
//					}
//
//					futureRowWidth += static_cast<int32_t>( mGlyphs[text[j]].Advance * scale);
//
//					if (futureRowWidth > maxWidth)
//					{
//						ok = false;
//						break;
//					}
//				}
//			}
//
//			if (!ok)
//			{
//				if (lineStart == nextBreak)
//				{
//					// If did not find any breaks on the line
//					while (i < j)
//					{
//						mPrintText.push_back(text[i]);
//						printToText.push_back(i);
//						++i;
//					}
//				}
//
//				mPrintText.push_back(L'\n');
//				printToText.push_back( min(i, text.length() - 1) );
//
//				rowWidth = 0;
//				nextBreak = lineStart = i;
//			}
//
//			if (i < text.length())
//			{
//				ch = text[i];
//
//				rowWidth +=  static_cast<int32_t>( mGlyphs[text[j]].Advance * scale);
//				if (rowWidth <= maxWidth)
//				{
//					mPrintText.push_back(ch);
//					printToText.push_back(i);
//				}
//			}
//		}
//		else
//		{
//			mPrintText.push_back(L'\n');
//			printToText.push_back(i);
//
//			rowWidth = 0;
//			nextBreak = lineStart = i;
//		}
//	}
//
//	float x = position.X();
//	float y = position.Y();
//
//	for (auto iter = mPrintText.begin(); iter != mPrintText.end(); ++iter)
//	{
//		wchar_t ch = *iter;
//
//		if (ch == L'\n')
//		{
//			y += mRowHeight * scale;
//			x = position.X();
//		}
//		else
//		{
//			const Glyph& glyph = mGlyphs[uint32_t(ch)];
//
//			float ch_x = x + glyph.OffsetX * scale;
//			float ch_y = y - glyph.OffsetY * scale;
//
//			IntRect sourceRect(glyph.SrcX, glyph.SrcY, glyph.Width, glyph.Height);
//			Rectanglef destRect(ch_x, ch_y, glyph.Width * scale, glyph.Height* scale);
//
//			spriteBatch.Draw(mFontTexture, destRect, &sourceRect, color);
//
//			x += glyph.Advance * scale;
//		}
//	}
//}

}