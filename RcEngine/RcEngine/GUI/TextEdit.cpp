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

	mSelectStart = 0;
	mCaretRowIndex = mCaretColumnIndex = 0;


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

	// Draw Text
	Rectanglef textRC((float)mTextRect.X, (float)mTextRect.Y, (float)mTextRect.Width, (float)mTextRect.Height);
	mTextEditStyle->Font->DrawString(spriteBatchFont, mText, mTextEditStyle->FontSize, AlignTop | AlignLeft, textRC, mTextColor);

	// Draw Caret
	if (mCaretOn)
	{
		Rectanglef caretRC;
		caretRC.X = (float)mCharPositions[mCaretRowIndex][mCaretColumnIndex].Start;
		caretRC.Y = mTextRect.Y + (float)mCaretRowIndex * mRowHeight;
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
			GetCharPos(screenPos, mCaretRowIndex, mCaretColumnIndex);

			mDragMouse = true;
		}
	}
}

void TextEdit::OnDragMove( const int2& position, uint32_t buttons )
{

}

void TextEdit::OnDragEnd( const int2& position )
{

}

void TextEdit::GetCharPos( const int2& screenPos, int32_t& rowIndex, int32_t& columnIndex )
{
	if (!mMultiLine)
	{
		rowIndex = 0;
		//columnIndex = std::distance( mCharPositions[0].begin(), std::upper_bound(mCharPositions[0].begin(), mCharPositions[0].end(), (float)screenPos.X()));
	}
	else
	{
		rowIndex = (std::max)(0, (screenPos.Y() - mTextRect.Y) / mRowHeight);

		if (rowIndex >= (int32_t)mCharPositions.size())
			return;

		columnIndex = 0;
		for (columnIndex = 0; columnIndex < (int32_t)mCharPositions[rowIndex].size(); ++columnIndex)
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

void TextEdit::PlaceCaret( int32_t rowIndex, int32_t columnIndex )
{

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

		}
		break;

	case KC_RightArrow:
		{

		}
		break;

	case KC_UpArrow:
		{

		}
		break;

	case KC_DownArrow:
		{

		}
		break;

	case KC_Delete:
		{

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






}