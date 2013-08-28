#include <GUI/Label.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Core/Exception.h>

namespace RcEngine {

Label::Label()
	: mTextAlignment(AlignCenter)
{

}

Label::Label( const int2& pos, const std::wstring& text, Alignment textAlign )
{
	mTextAlignment = textAlign;
	SetPosition(pos);
	SetText(text);
}

Label::~Label()
{

}

void Label::SetFont( const shared_ptr<Font>& font, int32_t fontSize )
{
	if (font != mFont && fontSize != mFontSize)
	{
		mFont = font;
		mFontSize = fontSize;

		UpdateText();
	}
}

void Label::SetText( const std::wstring& text )
{
	mText = text;
	UpdateText();
}

void Label::SetTextAlignment( uint32_t align )
{
	if (mTextAlignment != align)
	{
		mTextAlignment = align;
		UpdateText();
	}
}

void Label::SetWordWrap( bool enable )
{
	if (enable != mWordWrap)
	{
		mWordWrap = enable;
		UpdateText();
	}
}

//void Label::UpdateText()
//{
//	uint32_t width = 0;
//	uint32_t height = 0;
//
//	if (!mFont)
//	{
//		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Font not set", "Label::UpdateText");
//		return;
//	}
//
//	mPrintText.clear();
//	std::vector<size_t> printToText;
//
//	// Font Scale
//	const float scale = float(mFontSize) / mFont->GetFontSize();
//	const int32_t fontRowHeight =  mFont->GetRowHeight();
//
//	int32_t rowWidth = 0;
//	int32_t rowHeight = 0;
//
//	if (!mWordWrap)
//	{
//		mPrintText = mText;
//
//		printToText.resize(mPrintText.length());
//		for (size_t i = 0; i < mPrintText.length(); ++i)
//			printToText[i] = i;
//	}
//	else
//	{
//		// Max label width
//		int32_t maxWidth = GetSize().X(); 
//
//		printToText.reserve(mPrintText.length());
//
//		size_t nextBreak = 0;
//		size_t lineStart = 0;
//
//		size_t i, j;
//		for (i = 0; i < mText.length(); ++i)
//		{
//			wchar_t ch = mText[i];
//
//			if (ch != L'\n')
//			{
//				bool ok = true;
//
//				if (nextBreak <= i)
//				{
//					int32_t futureRowWidth = rowWidth;
//
//					// Find next blank or line break
//					for (j = i; j < mText.length(); ++j)
//					{
//						if (mText[j] == L' ' || mText[j] == L'\n')
//						{
//							nextBreak = j;
//							break;
//						}
//
//						futureRowWidth += static_cast<int32_t>( mFont->GetGlyphInfo(mText[j]).Advance );
//
//						if (futureRowWidth > maxWidth)
//						{
//							ok = false;
//							break;
//						}
//					}
//				}
//
//				if (!ok)
//				{
//					if (lineStart == nextBreak)
//					{
//						// If did not find any breaks on the line
//						while (i < j)
//						{
//							mPrintText.push_back(mText[i]);
//							printToText.push_back(i);
//							++i;
//						}
//					}
//
//					mPrintText.push_back(L'\n');
//					printToText.push_back( min(i, mText.length() - 1) );
//
//					rowWidth = 0;
//					nextBreak = lineStart = i;
//				}
//
//				if (i < mText.length())
//				{
//					ch = mText[i];
//
//					rowWidth +=  static_cast<int32_t>( mFont->GetGlyphInfo(mText[j]).Advance );
//					if (rowWidth <= maxWidth)
//					{
//						mPrintText.push_back(ch);
//						printToText.push_back(i);
//					}
//				}
//			}
//			else
//			{
//				mPrintText.push_back(L'\n');
//				printToText.push_back(i);
//
//				rowWidth = 0;
//				nextBreak = lineStart = i;
//			}
//		}
//	}
//	
//	// Mesure Text Region Size
//	mRowWidths.resize(1, 0);
//	for (size_t i = 0; i < mPrintText.length(); ++i)
//	{
//		wchar_t ch = mPrintText[i];
//
//		if (ch != L'\n')
//		{
//			mRowWidths.back() += static_cast<int32_t>(mFont->GetGlyphInfo(ch).Advance * scale);
//		}
//		else
//		{
//			mRowWidths.push_back(0);
//		}
//	}
//
//	rowWidth = *std::max_element(mRowWidths.begin(), mRowWidths.end());
//	rowHeight = static_cast<int32_t>(mRowWidths.size() * fontRowHeight * scale);
//
//	// Calculate each character's position
//	mCharRegions.resize(mPrintText.length() + 1);
//
//	int32_t rowIdx = 0;
//	float x = (float)GetRowStartPos(rowIdx);
//	float y = 0;
//	for (size_t i = 0; i < mPrintText.length(); ++i)
//	{
//		wchar_t ch = mPrintText[i];
//		const Font::Glyph& glyph = mFont->GetGlyphInfo(ch);
//
//		mCharRegions[printToText[i]].X = x + glyph.OffsetX * scale;
//		mCharRegions[printToText[i]].Y = y - glyph.OffsetY * scale;
//
//		if (ch != L'\n')
//		{			
//			mCharRegions[printToText[i]].Width = glyph.Width * scale;
//			mCharRegions[printToText[i]].Height = glyph.Height * scale;
//			x += glyph.Advance * scale;
//		}
//		else
//		{
//			y += fontRowHeight * scale;
//			x = (float)GetRowStartPos(++rowIdx);
//		}
//	}
//}

void Label::UpdateText()
{
	if (!mFont)
	{
		mFont = UIManager::GetSingleton().GetDefaultFont();
		mFontSize = 30;
	}

	const float scale = float(mFontSize) / mFont->GetFontSize();
	const float rowHeight = mFont->GetRowHeight() * scale;

	mRowWidths.resize(0);
	mRowWidths.push_back(0);
	
	mPrintText = mText;

	// Compute width and height to draw this text
	for (size_t i = 0; i < mText.length(); ++i)
	{
		wchar_t ch = mText[i];

		if (ch != L'\n')
			mRowWidths.back() += mFont->GetGlyphInfo(ch).Advance * scale;
		else
			mRowWidths.push_back(0);
	}
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

void Label::OnResize()
{
	if (mWordWrap)
		UpdateText();
}

void Label::Update( float delta )
{
	
}

void Label::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mFont)
		return;

	const shared_ptr<Texture>& fontTexture = mFont->GetFontTexture();

	const float scale = float(mFontSize) / mFont->GetFontSize();
	const float rowHeight = mFont->GetRowHeight() * scale;

	float width = *std::max_element(mRowWidths.begin(), mRowWidths.end());
	float heigh = mRowWidths.size() * rowHeight;

	size_t rowIdx = 0;

	const int2& ScreenPos = GetScreenPosition();
	const int2& ScreenSize = GetSize();	

	if (mSize.X() == 0 || mSize.Y() == 0)
	{		
		float2 destPos((float)ScreenPos.X(), (float)ScreenPos.Y() + rowHeight); 
		mFont->DrawString(spriteBatchFont, mText, mFontSize, destPos, ColorRGBA(1, 0, 0, 1));
	}
	else
	{
		float x = (float)ScreenPos.X() + GetRowStartPos(mRowWidths[rowIdx], (float)ScreenSize.X(), mTextAlignment);
		float y = (float)ScreenPos.Y();

		Rectanglef region((float)ScreenPos.X(), (float)ScreenPos.Y(), (float)ScreenSize.X(), (float)ScreenSize.Y());

		if (mTextAlignment & AlignTop)
			y += rowHeight;
		else if (mTextAlignment & AlignCenter)
			y +=  (ScreenSize.Y() - heigh) / 2 + rowHeight;
		else if (mTextAlignment & AlignRight)
			y +=  (ScreenSize.Y() - heigh) + rowHeight;
		else
			y +=  rowHeight;

		for (size_t i = 0; i < mPrintText.length(); ++i)
		{
			wchar_t ch = mPrintText[i];

			if (ch == L'\n')
			{
				y += rowHeight;

				rowIdx++;
				x = (float)ScreenPos.X() + GetRowStartPos(mRowWidths[rowIdx], (float)ScreenSize.X(), mTextAlignment);
			}
			else
			{
				const Font::Glyph& glyph = mFont->GetGlyphInfo(ch);

				float ch_x = x + glyph.OffsetX * scale;
				float ch_y = y - glyph.OffsetY * scale;

				float ch_width = glyph.Width * scale;
				float ch_height = glyph.Height* scale;

				x += glyph.Advance * scale;

				//Out of region
				bool flag[] = { (ch_x + ch_width <= region.Left()), (ch_x >= region.Right()),
				                (ch_y >= region.Bottom()), (ch_y + ch_height <= region.Top())  };

				if ( (ch_x + ch_width <= region.Left()) || (ch_x >= region.Right())  ||
					 (ch_y >= region.Bottom()) || (ch_y + ch_height <= region.Top()) )
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

				spriteBatchFont.Draw(fontTexture, destRect, &sourceRect, ColorRGBA(1, 0, 0, 1));
			}
		}
	}
	
	// Reset hovering for next frame
	mHovering = false;
}

void Label::InitGuiStyle( const GuiSkin::StyleMap* styles /*= nullptr*/ )
{

}

}