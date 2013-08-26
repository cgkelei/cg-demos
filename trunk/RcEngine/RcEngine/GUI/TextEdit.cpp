#include <GUI/TextEdit.h>
#include <GUI/ScrollBar.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Input/InputEvent.h>
#include <Input/InputSystem.h>
#include <Core/Exception.h>
#include <GUI/WordWrap.h>
#include <cctype>

namespace RcEngine {

static const int32_t BorderWidth = 8;
static const int32_t ScrollBarWidth = 20;

TextEdit::TextEdit()
	: mMultiLine(false),
	  mDragMouse(false),
	  mWordWrap(true),
	  mCaretBlinkRate(1.0f),
	  mCaretBlinkTimer(0.0f),
	  mBorder(BorderWidth),
	  mScrollBarWidth(ScrollBarWidth),
	  mTextEditStyle(nullptr),
	  mCaretOn(false)
{
	mTextColor = ColorRGBA( 1.0f, 0.06f, 0.06f, 0.06f );
	mSelTextColor = ColorRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	mSelBkColor = ColorRGBA( 1.0f, 0.156f, 0.196f, 0.36f );
	mCaretColor = ColorRGBA( 1.0f, 0, 0, 0 );

	mSelectStartX = mSelectStartY = 0;
	mCaretY = mCaretX = 0;

	mFirstVisibleLine = 0;

	mVertScrollBar = new ScrollBar(UI_Vertical);
	mVertScrollBar->SetScrollButtonRepeat(true);
	mVertScrollBar->SetVisible(false);
	mVertScrollBar->EventValueChanged.bind(this, &TextEdit::HandleVScrollBar);

	AddChild(mVertScrollBar);
}

TextEdit::~TextEdit()
{

}

bool TextEdit::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

void TextEdit::InitGuiStyle( const GuiSkin::StyleMap* styles /* = nullptr */ )
{
	if (styles)
	{

	}
	else
	{
		// use default
		GuiSkin* defaultSkin = UIManager::GetSingleton().GetDefaultSkin();
		mVertScrollBar->InitGuiStyle(nullptr);


		// background in Normal State
		mTextEditStyle = &defaultSkin->TextEdit;
	}

	// Init row height
	const float fontScale = (float)mTextEditStyle->FontSize / mTextEditStyle->Font->GetFontSize();
	mRowHeight = mTextEditStyle->Font->GetRowHeight() * fontScale;
}

void TextEdit::Update(float dt)
{
	// Blink the caret
	if (mCaretBlinkRate > 0.0f)
		mCaretBlinkTimer = fmodf(mCaretBlinkTimer + mCaretBlinkRate * dt, 1.0f);
	else
		mCaretBlinkTimer = 0.0f;

	if (HasFocus() && mCaretBlinkTimer > 0.5f)
		mCaretOn = true;
}

void TextEdit::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	int2 screenPos = GetScreenPosition();

	// Draw background first
	Rectanglef backRC((float)screenPos.X(), (float)screenPos.Y(), (float)mSize.X(), (float)mSize.Y());
	spriteBatch.Draw(mTextEditStyle->StyleTex, backRC, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, mTextEditStyle->StyleStates[UI_State_Normal].TexColor);

	// Draw selected background if has selection
	if (mSelectStartX != mCaretX || mSelectStartY != mCaretY)
	{	
		Vector<size_t, 2> start(mSelectStartX, mSelectStartY);
		Vector<size_t, 2> end(mCaretX, mCaretY);

		if ( (mSelectStartY > mCaretY) || (mSelectStartY == mCaretY && mSelectStartX > mCaretX) )
			std::swap(start, end);

		Rectanglef rect;
		rect.Height = mRowHeight;

		if (start.Y() != end.Y())
		{
			// First Line
			rect.SetLeft( mCharPositions[start.Y()][start.X()] );
			rect.SetRight( mCharPositions[start.Y()].back() );
			rect.SetTop( float(mTextRect.Y + mRowHeight * (start.Y()-mFirstVisibleLine)) );		
			spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));
			
			// Last Line
			rect.SetLeft( mCharPositions[end.Y()].front() );
			rect.SetRight( mCharPositions[end.Y()][end.X()] );
			rect.SetTop( float(mTextRect.Y + mRowHeight * (end.Y()-mFirstVisibleLine)) );		
			spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));

			for (size_t i = start.Y() + 1; i < end.Y(); ++i)
			{
				rect.SetLeft( mCharPositions[i].front() );
				rect.SetRight( mCharPositions[i].back() );
				rect.SetTop( float(mTextRect.Y + mRowHeight * (i-mFirstVisibleLine)));	
				spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));
			}
		}
		else
		{
			rect.SetLeft( mCharPositions[start.Y()][start.X()] );
			rect.SetRight( mCharPositions[end.Y()][end.X()] );
			rect.SetTop( float(mTextRect.Y + mRowHeight * (start.Y()-mFirstVisibleLine)) );		
			spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));
		}
	}

	// Draw Text
	Rectanglef textRC((float)mTextRect.X, (float)mTextRect.Y, (float)mTextRect.Width, (float)mTextRect.Height);
	mTextEditStyle->Font->DrawString(spriteBatchFont, mPrintText, mTextEditStyle->FontSize, AlignTop | AlignLeft, textRC, mTextColor);

	// Draw Caret
	if (mCaretOn)
	{
		Rectanglef caretRC;

		caretRC.X = mCharPositions[mCaretY][mCaretX];
		caretRC.Y = mTextRect.Y + (float)(mCaretY - mFirstVisibleLine) * mRowHeight;
		caretRC.Width = 1.0f;
		caretRC.Height = (float)mRowHeight;

		spriteBatch.Draw(mTextEditStyle->StyleTex, caretRC, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 0, 1));
	}

	// Reset
	mHovering = false;
	mCaretOn = false;
}

void TextEdit::OnDragBegin( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		if (mBackRect.Contains(screenPos.X(), screenPos.Y()))
		{
			GetCaretAt(screenPos, mCaretX, mCaretY);

			mDragMouse = true;

			mSelectStartX = mCaretX;
			mSelectStartY = mCaretY;
		}
	}
}

void TextEdit::OnDragMove( const int2& screenPos, uint32_t buttons )
{
	if ( mDragMouse && (buttons & InputEventType::LeftButtonMask) )
	{
		if (mBackRect.Contains(screenPos.X(), screenPos.Y()))
		{
			GetCaretAt(screenPos, mCaretX, mCaretY);
		}
	}
}

void TextEdit::OnDragEnd( const int2& position )
{
	mDragMouse = false;
}

void TextEdit::GetCaretAt( const int2& screenPos, size_t& caretX, size_t& caretY )
{
	if (!mMultiLine)
	{
		caretY = 0;

		caretX = mCharPositions[caretY].size() - 1;
		for (size_t i = 0; i < mCharPositions[caretY].size() - 1; ++i)
		{
			float half = (mCharPositions[caretY][i] + mCharPositions[caretY][i+1]) * 0.5f;

			if ((float)screenPos.X() < half)
			{
				caretX = i;
				break;
			}
		}
	}
	else
	{
		caretY = Clamp((size_t)floorf((screenPos.Y() - mTextRect.Y) / mRowHeight), size_t(0), mNumVisibleLines - 1);
		caretY += mFirstVisibleLine;

		caretX = mCharPositions[caretY].size() - 1;
		for (size_t i = 0; i < mCharPositions[caretY].size() - 1; ++i)
		{
			float half = (mCharPositions[caretY][i] + mCharPositions[caretY][i+1]) * 0.5f;
		
			if ((float)screenPos.X() < half)
			{
				caretX = i;
				break;
			}
		}
	}
}

void TextEdit::GetCaretPos( size_t index, size_t& caretX, size_t& caretY )
{
	size_t wrappedIndex = index;
	for (size_t i = 0; i < mWrappedBreakAt.size(); ++i)
	{
		if ((int32_t)index > mWrappedBreakAt[i]+1)
			wrappedIndex++;
		else
			break;
	}

	caretX = caretY = 0;
	for (size_t i = 0; i < wrappedIndex; ++i)
	{
		if (mWrappedText[i] == L'\n')
		{
			caretY++;
			caretX = 0;
		}
		else
			caretX++;
	}
}

size_t TextEdit::GetCharIndex( size_t caretX, size_t caretY )
{
	size_t wrappedIndex = 0;
	for (size_t i = 0; i < caretY; ++i)
		wrappedIndex += mCharPositions[i].size();

	wrappedIndex += caretX;

	size_t textIdx = wrappedIndex;
	size_t numPadding = 1;
	for (size_t i = 0; i < mWrappedBreakAt.size(); ++i)
	{
		if (mWrappedBreakAt[i] + numPadding < (int32_t)wrappedIndex)
		{
			textIdx--;
			numPadding++;
		}
		else
			break;
	}
	return textIdx;
}

bool TextEdit::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	mDragMouse = true;

	return false;
}

bool TextEdit::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	mDragMouse = false;

	return true;
}

void TextEdit::PlaceCaret( size_t newCaretX, size_t newCaretY )
{
	if (newCaretY != mCaretY)
	{
		mCaretY = Clamp(newCaretY, size_t(0), mCharPositions.size());
	}

	if (newCaretX != mCaretX)
	{
		mCaretX = Clamp(newCaretX, size_t(0), mCharPositions[mCaretY].size());
	}

	mSelectStartX = mCaretX;
	mSelectStartY = mCaretY;
}

void TextEdit::UpdateRect()
{
	UIElement::UpdateRect();

	if (!mTextEditStyle)
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Init Gui Style First!", "TextEdit::UpdateText");

	int2 screenPos = GetScreenPosition();

	mBackRect.X = screenPos.X();
	mBackRect.Y = screenPos.Y();
	mBackRect.Width = mSize.X();
	mBackRect.Height = mSize.Y();

	mTextRect.X = screenPos.X() + mBorder;
	mTextRect.Width = mSize.X() - mBorder - mBorder;

	mTextRect.Y = screenPos.Y() + mBorder;
	mTextRect.Height = mSize.Y() - mBorder - mBorder;

	mNumVisibleLines = (int32_t)floor(mTextRect.Height / mRowHeight);
	
	int32_t shrink = (int32_t)floor( mTextRect.Height - mNumVisibleLines * mRowHeight );

	mTextRect.Height -= shrink;
	mBackRect.Height -= shrink;
	mSize.Y() -= shrink;
}

void TextEdit::UpdateText()
{
	if (!mTextEditStyle)
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Init Gui Style First!", "TextEdit::UpdateText");

	const float fontScale = (float)mTextEditStyle->FontSize / mTextEditStyle->Font->GetFontSize();
	const float rowHeight = fontScale * mTextEditStyle->Font->GetRowHeight();

	mNumVisibleLines = (int32_t)floor(mTextRect.Height / rowHeight);

	if (mMultiLine)
	{
		WrapText();
		
		if (mNumLines > mNumVisibleLines)
		{
			if ( mVertScrollBar->IsVisible() == false )
			{
				mVertScrollBar->SetVisible(true);
				mVertScrollBar->SetPosition(int2(mSize.X() - mScrollBarWidth, 0));
				mVertScrollBar->SetSize(int2(mScrollBarWidth, mSize.Y()));

				int32_t remain = mNumLines - mNumVisibleLines;
				mVertScrollBar->SetScrollRange(0, remain);

				mTextRect.Width -= mScrollBarWidth;
				mBackRect.Width -= mScrollBarWidth;

				if (mWordWrap)
					WrapText();
			}	
			else 
			{
				int32_t remain = mNumLines - mNumVisibleLines;
				
				if (mFirstVisibleLine > remain)
					mFirstVisibleLine = remain;

				mVertScrollBar->SetScrollRange(0, remain);		
			}
		}
		else
		{
			if (mVertScrollBar->IsVisible())
			{
				mVertScrollBar->SetVisible(false);
				mTextRect.Width += mScrollBarWidth;
				mBackRect.Width += mScrollBarWidth;

				if (mWordWrap)
					WrapText();

				mFirstVisibleLine = 0;
			}
		}
	}
	
	// Keep track of every line char start pos
	float charX = (float)mTextRect.X;

	mCharPositions.resize(1);
	mCharPositions[0].resize(1, charX);

	size_t numRow = 0;
	for (size_t i = 0; i < mWrappedText.length(); ++i)
	{
		wchar_t ch = mWrappedText[i];

		if (ch == L'\n')
		{
			numRow++;
			mCharPositions.resize(numRow+1);
			charX = (float)mTextRect.X;

			mCharPositions.back().push_back(charX);
		}
		else
		{
			const Font::Glyph& glyph = mTextEditStyle->Font->GetGlyphInfo(ch);

			charX += glyph.Advance * fontScale;
			mCharPositions[numRow].push_back(charX);		
		}
	}

	UpdateVisisbleText();
}

void TextEdit::WrapText()
{
	const float fontScale = (float)mTextEditStyle->FontSize / mTextEditStyle->Font->GetFontSize();
	const float rowHeight = fontScale * mTextEditStyle->Font->GetRowHeight();

	mNumVisibleLines = (int32_t)floor(mTextRect.Height / rowHeight);
	mNumLines = 0;

	mWrappedText.clear();
	mWrappedBreakAt.clear();

	// Wrap Text if enable
	if (mWordWrap)
	{
		bool isBreakable = false;
		bool wasBreakable = false; // value of isBreakable for last char (i-1)
		bool isParens = false;     // true if one of ({[
		bool wasParens = false;    // value of isParens for last char (i-1)

		float x = (float)0;
		float y = (float)0;
		float w = (float)mTextRect.Width;
		float lineWidth = 0.0f;
		float textwidth = 0.0f;
		int32_t lastBreak = -1;
		size_t start = 0;

		Font& font = *mTextEditStyle->Font;
		for (size_t i = 0; i < mText.length(); ++i)
		{
			wchar_t c = mText[i];

			if (c != L'\n')
			{
				const Font::Glyph& glyph = font.GetGlyphInfo(c);	
				float ww = glyph.Advance * fontScale;

				isParens = (c == L'(') || (c == L'[') || (c == L'{');	
				// isBreakable is true when we can break after this character.
				isBreakable = ( std::isspace(c) || std::ispunct(c) ) && !isParens;

				if (!isBreakable && i < mText.length() - 1)
				{
					wchar_t nextc = mText[i+1];
					isBreakable = (nextc == L'(') || (nextc == L'[') || (nextc == L'{');	
				}

				int32_t breakAt = -1;	
				bool wrappedBreak = true;

				//if (x + ww > w && lastBreak != -1) // time to break and we know where   
				//	breakAt = lastBreak;

				//if (x + ww > w - 4 && lastBreak == -1) // time to break but found nowhere, break here
				//	breakAt = i;

				if (x + ww > w)
				{
					if (lastBreak == -1)
						breakAt = i - 1;
					else
						breakAt = lastBreak;
				}

				// don't leave the last char alone
				if ( (i == mText.length() - 2) && (x + ww + font.GetGlyphInfo(mText[i+1]).Advance * fontScale) > w )  
					breakAt = (lastBreak == -1) ? i - 1 : lastBreak;

				if (breakAt != -1)
				{
					float thisLineWidth = (lastBreak == -1) ? x + ww : lineWidth;

					mWrappedBreakAt.push_back(breakAt);

					mWrappedText += mText.substr(start, breakAt-start+1) + L'\n';
					start = breakAt + 1;

					mNumLines++;

					textwidth = (std::max)(textwidth, thisLineWidth);
					x = 0; y += rowHeight;

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
			else
			{
				mWrappedText += mText.substr(start, i-start+1);
				start = i + 1;

				x = 0; y += rowHeight;
				wasBreakable = false;
				wasParens = false;
				lastBreak = -1;
				lineWidth = 0;
				mNumLines++;
			}

		}

		mNumLines++;
		textwidth = (std::max)(textwidth, x);
		y += rowHeight;
		mWrappedText += mText.substr( start );
	}
	else
		mWrappedText = mText;

}

void TextEdit::SetText( const std::wstring& text )
{
	mText = text;
	UpdateText();
}

bool TextEdit::OnKeyPress( uint16_t key )
{
	switch (key)
	{
	case KC_LeftArrow:
		{
			if (mCaretX > 0)
				PlaceCaret(mCaretX - 1, mCaretY);
			else if (mCaretX == 0 && mCaretY > 0)
			{
				PlaceCaret(mCharPositions[mCaretY-1].size() - 1, mCaretY-1);
			}
		}
		break;

	case KC_RightArrow:
		{
			if (mCaretX < mCharPositions[mCaretY].size() - 1)
				PlaceCaret(mCaretX + 1, mCaretY);
			else 
			{
				if (mCaretY < mCharPositions.size() - 1)
					PlaceCaret(0, mCaretY+1);
			}
		}
		break;

	case KC_UpArrow:
		{
			if (mCaretY> 0)
				PlaceCaret((std::min)(mCaretX, mCharPositions[mCaretY-1].size()-1), mCaretY-1);
		}
		break;

	case KC_DownArrow:
		{
			if (mCaretY < mCharPositions.size() - 1)
				PlaceCaret((std::min)(mCaretX, mCharPositions[mCaretY+1].size()-1), mCaretY+1);
		}
		break;

	case KC_Home:
		{
			PlaceCaret(0, mCaretY);
		}
		break;

	case KC_End:
		{
			PlaceCaret(mCharPositions[mCaretY].size()-1 , mCaretY);
		}
		break;

	case KC_Delete:
		{
			if (mSelectStartX != mCaretX || mSelectStartY != mCaretY)
				DeleteSlectedText();
			else
				DeleteNextChar();
		}
		break;

	case KC_BackSpace:
		{
			if (mSelectStartX != mCaretX || mSelectStartY != mCaretY)
				DeleteSlectedText();
			else
				DeletePreChar();
		}
		break;

	case KC_Enter:
		{
			if( mMultiLine )
			{
				size_t caretIdx = GetCharIndex(mCaretX, mCaretY);

				mText.insert(mText.begin() + caretIdx, L'\n');
				UpdateText();		
				PlaceCaret(0, mCaretY + 1);
			}
			else
			{
				if (!EventReturnPressed.empty())
					EventReturnPressed(L"");
			}

		}
		break;
	default:
		break;
	}

	return true;
}

bool TextEdit::OnTextInput( uint16_t unicode )
{
	bool eventConsumed = true;
	
	if (HasFocus() && unicode >= 0x20)
	{
		if (mSelectStartX != mCaretX || mSelectStartY != mCaretY)
		{
			Vector<size_t, 2> start(mSelectStartX, mSelectStartY);
			Vector<size_t, 2> end(mCaretX, mCaretY);

			if ( (mSelectStartY > mCaretY) || (mSelectStartY == mCaretY && mSelectStartX > mCaretX) )
				std::swap(start, end);

			size_t delStart = GetCharIndex(start.Y(), start.X());
			size_t delEnd = GetCharIndex(end.Y(), end.X());

			mText.replace(delStart, delEnd - delStart, 1, (wchar_t)unicode);

			UpdateText();		
			PlaceCaret(start.X() + 1, start.Y());

		}
		else
		{
			size_t start = GetCharIndex(mCaretX, mCaretY);
			mText.insert(mText.begin() + start, (wchar_t)unicode);
			UpdateText();		

			size_t newCaretX, newCaretY;
			GetCaretPos(start+1, newCaretX, newCaretY);
			PlaceCaret(newCaretX,newCaretY);
		}
	}

	return eventConsumed;
}

void TextEdit::DeleteSlectedText()
{
	size_t x = 0, y = 0;
	
	Vector<size_t, 2> start(mSelectStartX, mSelectStartY);
	Vector<size_t, 2> end(mCaretX, mCaretY);
	
	if ( (mSelectStartY > mCaretY) || (mSelectStartY == mCaretY && mSelectStartX > mCaretX) )
		std::swap(start, end);

	size_t delStart = GetCharIndex( start.X(), start.Y());
	size_t delEnd = GetCharIndex(end.X(), end.Y());

	mText.erase(delStart, delEnd - delStart);
	UpdateText();

	size_t newCaretX, newCaretY;
	GetCaretPos(delStart, newCaretX, newCaretY);
	PlaceCaret(newCaretX,newCaretY);

	std::wcout << mText << std::endl;
}

void TextEdit::DeleteNextChar()
{
	size_t delStart = GetCharIndex(mCaretX, mCaretY);

	if (delStart < mText.size())
	{
		mText.erase(delStart, 1);
		UpdateText();

		size_t newCaretX, newCaretY;
		GetCaretPos(delStart, newCaretX, newCaretY);
		PlaceCaret(newCaretX,newCaretY);
	}	

	//std::wcout << mText << std::endl;
}

void TextEdit::DeletePreChar()
{
	size_t delStart = GetCharIndex(mCaretX, mCaretY);
	if (delStart > 0)
	{
		auto c = mText[delStart-1];

		mText.erase(delStart-1, 1);
		UpdateText();

		size_t newCaretX, newCaretY;
		GetCaretPos(delStart-1, newCaretX, newCaretY);
		PlaceCaret(newCaretX,newCaretY);
	}

	//std::wcout << mText << std::endl;
}

void TextEdit::HandleVScrollBar( int32_t value )
{
	int32_t minValue, maxValue;
	mVertScrollBar->GetScrollRange(&minValue, &maxValue);

	mFirstVisibleLine = (size_t)value;
	
	UpdateVisisbleText();
}

void TextEdit::UpdateVisisbleText()
{
	size_t start = 0;
	size_t end = mWrappedText.length() - 1;

	if (mFirstVisibleLine > 0)
	{
		size_t lineIdx = 0;
		for (size_t i = 0; i < mWrappedText.length(); ++i)
		{
			if (mWrappedText[i] == L'\n')
			{
				if (++lineIdx == mFirstVisibleLine)
				{
					start = i+1;
					break;
				}
			}
		}
	}

	// Update print text
	size_t numLines = (std::min)(mNumVisibleLines, mNumLines);
	for (size_t i = start; i < mWrappedText.length(); ++i)
	{
		if (mWrappedText[i] == L'\n')
		{
			if (--numLines == 0)
			{
				end = i;
				break;
			}
		}	
	}

	mPrintText = mWrappedText.substr(start, end - start + 1);
}






}