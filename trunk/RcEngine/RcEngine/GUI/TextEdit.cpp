#include <GUI/TextEdit.h>
#include <GUI/ScrollBar.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Input/InputEvent.h>
#include <Input/InputSystem.h>
#include <Core/Exception.h>

namespace RcEngine {

static const int32_t BorderWidth = 8;
static const int32_t ScrollBarWidth = 20;

TextEdit::TextEdit()
	: mMultiLine(false),
	  mDragMouse(false),
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
	mRowHeight = static_cast<int32_t>( mTextEditStyle->Font->GetRowHeight() * fontScale );
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
		rect.Height = (float)mRowHeight;

		if (start.Y() != end.Y())
		{
			// First Line
			rect.SetLeft( mCharPositions[start.Y()][start.X()] );
			rect.SetRight( mCharPositions[start.Y()].back() );
			rect.SetTop( float(mTextRect.Y + mRowHeight * start.Y()) );		
			spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));
			
			// Last Line
			rect.SetLeft( mCharPositions[end.Y()].front() );
			rect.SetRight( mCharPositions[end.Y()][end.X()] );
			rect.SetTop( float(mTextRect.Y + mRowHeight * end.Y()) );		
			spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));

			for (size_t i = start.Y() + 1; i < end.Y(); ++i)
			{
				rect.SetLeft( mCharPositions[i].front() );
				rect.SetRight( mCharPositions[i].back() );
				rect.SetTop( float(mTextRect.Y + mRowHeight * i) );	
				spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));
			}
		}
		else
		{
			rect.SetLeft( mCharPositions[start.Y()][start.X()] );
			rect.SetRight( mCharPositions[end.Y()][end.X()] );
			rect.SetTop( float(mTextRect.Y + mRowHeight * start.Y()) );		
			spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));
		}
	}

	// Draw Text
	Rectanglef textRC((float)mTextRect.X, (float)mTextRect.Y, (float)mTextRect.Width, (float)mTextRect.Height);
	mTextEditStyle->Font->DrawString(spriteBatchFont, mText, mTextEditStyle->FontSize, AlignTop | AlignLeft, textRC, mTextColor);

	// Draw Caret
	if (mCaretOn)
	{
		Rectanglef caretRC;

		caretRC.X = mCharPositions[mCaretY][mCaretX];
		caretRC.Y = mTextRect.Y + (float)mCaretY * mRowHeight;
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
			GetCharPos(screenPos, mCaretY, mCaretX);

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
			GetCharPos(screenPos, mCaretY, mCaretX);
		}
	}
}

void TextEdit::OnDragEnd( const int2& position )
{
	mDragMouse = false;
}

void TextEdit::GetCharPos( const int2& screenPos, size_t& rowIndex, size_t& columnIndex )
{
	if (!mMultiLine)
	{
		rowIndex = 0;

		columnIndex = mCharPositions[rowIndex].size() - 1;
		for (size_t i = 0; i < mCharPositions[rowIndex].size() - 1; ++i)
		{
			float half = (mCharPositions[rowIndex][i] + mCharPositions[rowIndex][i+1]) * 0.5f;

			if ((float)screenPos.X() < half)
			{
				columnIndex = i;
				break;
			}
		}
	}
	else
	{
		rowIndex = Clamp(size_t(screenPos.Y() - mTextRect.Y) / mRowHeight, size_t(0), mCharPositions.size() - 1);

		columnIndex = mCharPositions[rowIndex].size() - 1;
		for (size_t i = 0; i < mCharPositions[rowIndex].size() - 1; ++i)
		{
			float half = (mCharPositions[rowIndex][i] + mCharPositions[rowIndex][i+1]) * 0.5f;
		
			if ((float)screenPos.X() < half)
			{
				columnIndex = i;
				break;
			}
		}
	}
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

	int2 screenPos = GetScreenPosition();

	mBackRect.X = screenPos.X();
	mBackRect.Y = screenPos.Y();
	mBackRect.Width = mSize.X();
	mBackRect.Height = mSize.Y();

	mTextRect.X = screenPos.X() + mBorder;
	mTextRect.Width = mSize.X() - mBorder - mBorder;

	mTextRect.Y = screenPos.Y() + mBorder;
	mTextRect.Height = mSize.Y() - mBorder - mBorder;
}

void TextEdit::UpdateText()
{
	if (!mTextEditStyle)
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Init Gui Style First!", "TextEdit::UpdateText");

	// Update ScrollBar
	UpdateVScrollBar();

	const float fontScale = (float)mTextEditStyle->FontSize / mTextEditStyle->Font->GetFontSize();

	float charX = (float)mTextRect.X;

	mCharPositions.resize(1);
	mCharPositions[0].resize(1, charX);

	size_t numRow = 0;
	for (size_t i = 0; i < mText.length(); ++i)
	{
		wchar_t ch = mText[i];

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
				PlaceCaret(mCharPositions[mCaretY-1].size(), mCaretY-1);
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
				size_t caretIdx = GetCharIndex(mCaretY, mCaretX);

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
			size_t caretIdx = GetCharIndex(mCaretY, mCaretX);

			mText.insert(mText.begin() + caretIdx, (wchar_t)unicode);

			UpdateText();		
			PlaceCaret(mCaretX + 1, mCaretY);
		}
	}

	return eventConsumed;
}

size_t TextEdit::GetCharIndex( size_t rowIndex, size_t columnIndex )
{
	size_t retVal = 0;

	size_t i = 0;
	while (i < rowIndex)
	{
		retVal += mCharPositions[i].size();
		i++;
	}

	retVal += columnIndex;

	return retVal;
}

void TextEdit::DeleteSlectedText()
{
	size_t x = 0, y = 0;
	
	Vector<size_t, 2> start(mSelectStartX, mSelectStartY);
	Vector<size_t, 2> end(mCaretX, mCaretY);
	
	if ( (mSelectStartY > mCaretY) || (mSelectStartY == mCaretY && mSelectStartX > mCaretX) )
		std::swap(start, end);

	size_t delStart = GetCharIndex(start.Y(), start.X());
	size_t delEnd = GetCharIndex(end.Y(), end.X());


	mText.erase(delStart, delEnd - delStart);
	UpdateText();

	PlaceCaret(start.X(), start.Y());
}

void TextEdit::DeleteNextChar()
{
	size_t delStart = GetCharIndex(mCaretY, mCaretX);

	if (delStart < mText.size())
	{
		mText.erase(delStart, 1);
		UpdateText();
		PlaceCaret(mCaretX, mCaretY);
	}	
}

void TextEdit::DeletePreChar()
{
	size_t delStart = GetCharIndex(mCaretY, mCaretX);
	if (delStart != 0)
	{
		mText.erase(delStart-1, 1);
		UpdateText();

		if (mCaretX > 0)
			PlaceCaret(mCaretX - 1, mCaretY);
		else if (mCaretX == 0 && mCaretY > 0)
		{
			size_t i = 0;
			while (i < mCaretY - 1)
			{
				delStart -= mCharPositions[i].size();
				i++;
			}

			PlaceCaret(delStart-1, mCaretY-1);
		}
	}
}

void TextEdit::UpdateVScrollBar()
{
	int32_t numLines = std::count(mText.begin(), mText.end(), L'\n') + 1;

	/*if ( mMultiLine && (numLines * mRowHeight) > mTextRect.Height )
	{
		if ( mVertScrollBar->IsVisible() == false )
		{
			mVertScrollBar->SetVisible(true);
			mVertScrollBar->SetPosition(int2(mSize.X() - mScrollBarWidth, 0));
			mVertScrollBar->SetSize(int2(mScrollBarWidth, mSize.Y()));
			
			int32_t remain = numLines - mTextRect.Height / mRowHeight;
			mVertScrollBar->SetScrollRange(0, remain);

			mTextRect.Width -= mScrollBarWidth;
			mBackRect.Width -= mScrollBarWidth;
		}	
	}
	else
	{
		if ( mVertScrollBar->IsVisible() )
		{
			mVertScrollBar->SetVisible(false);
			mTextRect.Width += mScrollBarWidth;
			mBackRect.Width += mScrollBarWidth;
		}
	}*/
}

void TextEdit::HandleVScrollBar( int32_t value )
{
	int32_t minValue, maxValue;
	mVertScrollBar->GetScrollRange(&minValue, &maxValue);

	mFirstVisibleLine = (size_t)value;

	UpdateText();
}






}