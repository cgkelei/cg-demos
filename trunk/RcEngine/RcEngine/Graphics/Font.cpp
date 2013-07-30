#include <Graphics/Font.h>
#include <Graphics/Texture.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/SpriteBatch.h>
#include <Core/Context.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <Math/Rectangle.h>
#include <Resource/ResourceManager.h>

namespace RcEngine {

class FontLoader
{
public:
	FontLoader(const String& fontFile, Font* fnt)
		: mFontFile(fontFile), mFont(fnt)
	{ 
		mTextureWidth = (float)mFont->mFontTexture->GetWidth(0);
		mTextureHeight = (float)mFont->mFontTexture->GetHeight(0);
	}

	bool Load();

protected:
	int SkipWhiteSpace(std::string &str, int start);
	int FindEndOfToken(std::string &str, int start);

	void InterpretInfo(std::string &str, int start);
	void InterpretChars(std::string &str, int start);
	void InterpretChar(std::string &str, int start);

	void AddChar(int id, int x, int y, int w, int h, float xoffset, float yoffset, float xadvance, int page, int chnl);

private:
	String mFontFile;
	String mFontName;
	size_t mNumChars;
	float mTextureWidth;
	float mTextureHeight;
	Font* mFont;
};

bool FontLoader::Load()
{
	std::string line;

	FILE* file = fopen(mFontFile.c_str(), "r");
	if (!file)
		return false;

	while( !feof(file) )
	{
		// Read until line feed (or EOF)
		line = "";
		line.reserve(256);
		while( !feof(file) )
		{
			char ch;
			if( fread(&ch, 1, 1, file) )
			{
				if( ch != '\n' ) 
					line += ch; 
				else
					break;
			}
		}

		if (line.empty())
			break;

		// Skip white spaces
		int pos = SkipWhiteSpace(line, 0);

		// Read token
		int pos2 = FindEndOfToken(line, pos);
		std::string token = line.substr(pos, pos2-pos);

		// Interpret line
		if( token == "info" )
			InterpretInfo(line, pos2);
		else if( token == "chars" )
			InterpretChars(line, pos2);
		else if( token == "char" )
			InterpretChar(line, pos2);
	}

	fclose(file);

	assert(mNumChars == mFont->mGlyphs.size());

	// Success
	return 1;
}

int FontLoader::SkipWhiteSpace( std::string &str, int start )
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

int FontLoader::FindEndOfToken( std::string &str, int start )
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

void FontLoader::InterpretChar( std::string &str, int start )
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

	// Store the attributes
	AddChar(id, x, y, width, height, xoffset, yoffset, xadvance, page, chnl);	
}

void FontLoader::InterpretChars( std::string &str, int start )
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
		{
			mNumChars = (size_t)strtol(value.c_str(), 0, 10);
		}

		if( pos2 == str.size() ) break;
	}
}

void FontLoader::InterpretInfo( std::string &str, int start )
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

		if (token == "face")
			mFont->mFontName = value;
		else if (token == "size")
			mFont->mCharSize = (int)strtol(value.c_str(), 0, 10);

		if( pos2 == str.size() ) break;
	}
}

void FontLoader::AddChar( int id, int x, int y, int w, int h, float xoffset, float yoffset, float xadvance, int page, int chnl )
{
	if( id >= 0 )
	{
		Font::Glyph glyph;
		glyph.SrcX = x;
		glyph.SrcY = y;
		glyph.Width = w;
		glyph.Height = h;
		glyph.Advance = xadvance;
		glyph.OffsetX = xoffset;
		glyph.OffsetY = yoffset;
		
		mFont->mRowHeight = (std::max)(mFont->mRowHeight, h);
		mFont->mGlyphs.insert(std::make_pair(uint32_t(id), glyph));
	}
}


////////////////////////////////////////////////////////////////////////////
Font::Font( ResourceTypes resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(resType, creator, handle, name, group), mRowHeight(0), mCharSize(0)
{
	
}

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
	FontLoader loader(filePath, this);
	if (!loader.Load())
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filePath + "not found!", "Font::LoadImpl");
}

void Font::UnloadImpl()
{

}

void Font::DrawString(SpriteBatch& spriteBatch, std::wstring& text, uint32_t fontSize, const float2& position, const ColorRGBA& color)
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
			const Glyph& glyph = mGlyphs[uint32_t(ch)];

			float ch_x = x + glyph.OffsetX * scale;
			float ch_y = y - glyph.OffsetY * scale;

			IntRect sourceRect(glyph.SrcX, glyph.SrcY, glyph.Width, glyph.Height);
			Rectanglef destRect(ch_x, ch_y, glyph.Width * scale, glyph.Height* scale);
			
			spriteBatch.Draw(mFontTexture, destRect, &sourceRect, color);
		
			x += glyph.Advance * scale;
		}
	}
}

void Font::MeasureString( const std::wstring& text, uint32_t fontSize, uint32_t* widthOut, uint32_t* heightOut )
{
	const float scale = float(fontSize) / mCharSize;

	std::vector<uint32_t> rowWidths(1, 0);

	for (size_t i = 0, length = text.length(); i < length; ++i)
	{
		wchar_t ch = text[i];

		if (ch != L'\n')
		{
			rowWidths.back() += static_cast<uint32_t>(mGlyphs[uint32_t(ch)].Advance * scale);
		}
		else
		{
			rowWidths.push_back(0);
		}
	}

	if(widthOut)  *widthOut = *std::max_element(rowWidths.begin(), rowWidths.end());
	if(heightOut) *heightOut = static_cast<uint32_t>(rowWidths.size() * mRowHeight * scale);
}

int32_t Font::GetRowHeight( int32_t fontSize ) const
{
	const float scale = float(fontSize) / mCharSize;
	return static_cast<uint32_t>(mRowHeight * scale);
}

shared_ptr<Resource> Font::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<Font>(RT_Font, creator, handle, name, group);
}





}