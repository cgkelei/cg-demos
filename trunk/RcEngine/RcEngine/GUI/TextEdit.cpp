#include <GUI/TextEdit.h>
#include <GUI/ScrollBar.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Input/InputEvent.h>
#include <Input/InputSystem.h>

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

	mVertScrollBar = new ScrollBar();
	mVertScrollBar->SetOrientation( UI_Vertical );
	mVertScrollBar->SetScrollButtonRepeat(false);
	mVertScrollBar->SetVisible(false);

	AddChild(mVertScrollBar);
}

TextEdit::~TextEdit()
{

}

bool TextEdit::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

void TextEdit::Initialize( const GuiSkin::StyleMap* styles /* = nullptr */ )
{
	if (styles)
	{

	}
	else
	{
		// use default
		GuiSkin* defaultSkin = UIManager::GetSingleton().GetDefaultSkin();
		mVertScrollBar->Initialize(nullptr);


		// background in Normal State
		mTextEditStyle = &defaultSkin->TextEdit;
	}

	UpdateText();
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

		if (start.Y() < end.Y())
		{
			// First Line
			if (start.X() < mCharPositions[start.Y()].size())
			{
				const CaretPair& last = mCharPositions[start.Y()].back();
				
				rect.SetLeft( mCharPositions[start.Y()][start.X()].Start );
				rect.SetRight( last.Start + (last.Half - last.Start) * 2.0f );
				rect.SetTop( float(mTextRect.Y + mRowHeight * start.Y()) );		

				spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));
			}

			// Last Line
			if (end.X() > 0)
			{
				rect.SetTop( float(mTextRect.Y + mRowHeight * end.Y()) );
				rect.SetLeft(mCharPositions[end.Y()][0].Start);

				if (end.X() >= mCharPositions[end.Y()].size())
				{
					const CaretPair& last = mCharPositions[end.Y()].back();
					rect.SetRight( last.Start + (last.Half - last.Start) * 2.0f );
				}
				else
				{
					rect.SetRight( mCharPositions[end.Y()][end.X()].Start );
				}
			}
			

			spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));
		}
		else
		{
			rect.SetTop( float(mTextRect.Y + mRowHeight * start.Y()) );
			rect.SetLeft( mCharPositions[start.Y()][start.X()].Start );

			if (end.X() >= mCharPositions[end.Y()].size())
			{
				const CaretPair& last = mCharPositions[end.Y()].back();
				rect.SetRight( last.Start + (last.Half - last.Start) * 2.0f );
			}
			else
			{
				rect.SetRight( mCharPositions[end.Y()][end.X()].Start );
			}

			spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));			
		}

		for (size_t y = start.Y() + 1; y < end.Y(); ++y)
		{
			const CaretPair& last = mCharPositions[y].back();

			rect.SetLeft( mCharPositions[y][0].Start );
			rect.SetRight( last.Start + (last.Half - last.Start) * 2.0f );
			rect.SetTop( float(mTextRect.Y + mRowHeight * y) );

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

		if (mCaretX >= mCharPositions[mCaretY].size())
		{
			const CaretPair& pair = mCharPositions[mCaretY].back();
			caretRC.X = pair.Start + (pair.Half - pair.Start) * 2.0f;
		}
		else
		{
			caretRC.X = (float)mCharPositions[mCaretY][mCaretX].Start;
		}
		
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

		columnIndex = 0;
		for (columnIndex = 0; columnIndex < mCharPositions[rowIndex].size(); ++columnIndex)
		{
			if (mCharPositions[rowIndex][columnIndex].Half > (float)screenPos.X())
				break;
		}
	}
	else
	{
		rowIndex = Clamp(size_t(screenPos.Y() - mTextRect.Y) / mRowHeight, size_t(0), mCharPositions.size() - 1);

		columnIndex = 0;
		for (columnIndex = 0; columnIndex < mCharPositions[rowIndex].size(); ++columnIndex)
		{
			if (mCharPositions[rowIndex][columnIndex].Half > (float)screenPos.X())
				break;
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
		return;

	const float fontScale = (float)mTextEditStyle->FontSize / mTextEditStyle->Font->GetFontSize();
	mRowHeight = static_cast<int32_t>( mTextEditStyle->Font->GetRowHeight() * fontScale );

	mCharPositions.clear();

	size_t numRow = 0;
	mCharPositions.resize(numRow+1);

	float charX = (float)mTextRect.X;
	for (size_t i = 0; i < mText.length(); ++i)
	{
		wchar_t ch = mText[i];

		if (ch == L'\n')
		{
			numRow++;
			mCharPositions.resize(numRow+1);
			charX = (float)mTextRect.X;
		}
		else
		{
			const Font::Glyph& glyph = mTextEditStyle->Font->GetGlyphInfo(ch);

			CaretPair pair;
			pair.Start = charX;
			pair.Half = charX + glyph.Advance * fontScale / 2;
			mCharPositions[numRow].push_back(pair);

			charX += glyph.Advance * fontScale;
		}
	}

	//int32_t numLines = std::count(mText.begin(), mText.end(), L'\n') + 1;
	int32_t numLines = mCharPositions.size();

	if (mMultiLine && (numLines * mRowHeight) > mTextRect.Height )
	{
		mVertScrollBar->SetVisible(true);
		mVertScrollBar->SetPosition(int2(mSize.X() - mScrollBarWidth, 0));
		mVertScrollBar->SetSize(int2(mScrollBarWidth, mSize.Y()));
		mVertScrollBar->SetScrollRange(0, numLines);

		mTextRect.Width -= mScrollBarWidth;

		mBackRect.Width -= mScrollBarWidth;
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
			if (mCaretX < mCharPositions[mCaretY].size())
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
				PlaceCaret(mCaretX, mCaretY-1);
		}
		break;

	case KC_DownArrow:
		{
			if (mCaretY < mCharPositions.size() - 1)
				PlaceCaret(mCaretX, mCaretY+1);
		}
		break;

	case KC_BackSpace:
		{
			if (mSelectStartX != mCaretX || mSelectStartY != mCaretY)
				DeleteSlectedText();
			else
				DeleteChar();
		}
		break;

	case KC_Enter:
		{
			if( mMultiLine )
			{

			}
			else
			{
				if (!EventReturnPressed.empty())
					EventReturnPressed(L"");
			}

		}
		break;

	case KC_Home:
		{

		}
		break;

	default:
		break;
	}

	return true;
}

bool TextEdit::OnTextInput( uint16_t unicode )
{
	bool eventConsumed = false;



	return eventConsumed;
}

void TextEdit::DeleteSlectedText()
{
	size_t x = 0, y = 0;
	
	Vector<size_t, 2> start(mSelectStartX, mSelectStartY);
	Vector<size_t, 2> end(mCaretX, mCaretY);
	
	if ( (mSelectStartY > mCaretY) || (mSelectStartY == mCaretY && mSelectStartX > mCaretX) )
		std::swap(start, end);

	size_t delStart = 0;
	size_t delEnd = 0;

	for (size_t i = 0; i < mText.length(); ++i)
	{
		wchar_t ch = mText[i];

		if (x == start.X() && y == start.Y())
			delStart = i;

		if (x == end.X() && y == end.Y())
		{
			delEnd = i;
			break;
		}

		x++;

		if (ch == L'\n')
		{
			y++;
			x = 0;
		}
	}

	mText.erase(delStart, delEnd - delStart);
	UpdateText();

	PlaceCaret(start.X(), start.Y());
}

void TextEdit::DeleteChar()
{

}






}