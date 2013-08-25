#include <GUI/WordWrap.h>
#include <Graphics/Font.h>
#include <cctype>
#include <locale>

namespace RcEngine {

WordWrap::WordWrap( const std::wstring& text, const IntRect& region )
	: mText(text), mRect(region)
{

}

WordWrap* WordWrap::WrapText( const std::wstring& text, const IntRect& region, Font& font, uint32_t fontSize )
{
	WordWrap* wordWrap = new WordWrap(text, region);

	const float fontScale = float(fontSize) / font.GetFontSize();
	const float fontRowHeight = font.GetRowHeight() * fontScale;

	bool isBreakable = false;
	bool wasBreakable = false; // value of isBreakable for last char (i-1)
	bool isParens = false;     // true if one of ({[
	bool wasParens = false;    // value of isParens for last char (i-1)

	float x = 0.0f;
	float y = 0.0f;
	float w = (float)region.Width;
	float lineWidth = 0.0f;
	float textwidth = 0.0f;
	int32_t lastBreak = -1;

	size_t textLen = text.length();
	for (size_t i = 0; i < textLen; ++i)
	{
		wchar_t c = text[i];

		const Font::Glyph& glyph = font.GetGlyphInfo(c);	
		float ww = glyph.Width * fontScale;

		isParens = (c == L'(') || (c == L'[') || (c == L'{');	
		// isBreakable is true when we can break after this character.
		isBreakable = ( std::isspace(c) || std::ispunct(c) ) && !isParens;

		if (!isBreakable && i < textLen - 1)
		{
			wchar_t nextc = text[i+1];
			isBreakable = (nextc == L'(') || (nextc == L'[') || (nextc == L'{');	
		}

		int32_t breakAt = -1;	
		if (x + ww > w && lastBreak != -1) // time to break and we know where   
			breakAt = lastBreak;
		
		if (x + ww > w - 4 && lastBreak == -1) // time to break but found nowhere, break here
			breakAt = i;
		
		// don't leave the last char alone
		if ( (i == textLen - 2) && (x + ww + font.GetGlyphInfo(text[i+1]).Width * fontScale) > w )
			breakAt = (lastBreak == -1 ? i - 1 : lastBreak);

		if (c == L'\n')
		{
			if (breakAt == -1 && lastBreak != -1)
			{
				breakAt = i -1;
				lastBreak = -1;
			}

			//wordWrap->mText.erase(i, 1);
		}

		if (breakAt != -1)
		{
			float thisLineWidth = (lastBreak == -1) ? x + ww : lineWidth;

			wordWrap->mBreakPositions.push_back(breakAt);
			wordWrap->mLineWidth.push_back(thisLineWidth);
			
			textwidth = (std::max)( textwidth, thisLineWidth );
			x = 0;
			y += fontRowHeight;

			wasBreakable = true;
			wasParens = false;
			if ( lastBreak != -1 )
			{
				// Breakable char was found, restart from there
				i = lastBreak;
				lastBreak = -1;
				continue;
			}
		}
		else if (isBreakable)
		{
			lastBreak = i;
			lineWidth = x + ww;
		}

		x += ww;
		wasBreakable = isBreakable;
		wasParens = isParens;
	}

	// Last line
	textwidth = (std::max)( textwidth, x );
	wordWrap->mLineWidth.push_back(x);
	y += fontRowHeight;

	return wordWrap;
}

std::wstring WordWrap::GetWrappedText() const
{
	std::wstring wrapped;
	int start = 0;
	for (int32_t breakAt : mBreakPositions)
	{
		wrapped += mText.substr(start, breakAt - start + 1) + L"\n";
		start = breakAt + 1;
	}

	wrapped += mText.substr( start );

	return wrapped;
}

}