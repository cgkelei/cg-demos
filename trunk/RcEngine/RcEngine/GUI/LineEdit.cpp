#include <GUI/LineEdit.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Input/InputEvent.h>
#include <Input/InputSystem.h>

namespace RcEngine {

LineEdit::LineEdit()
	: mCaretPos(0), 
	  mSelectStartPos(0),
	  mBorder(4),
	  mCaretOn(false),
	  mDragCursor(false),
	  mStyle(nullptr),
	  mCaretBlinkRate(1.0f),
	  mCaretBlinkTimer(0.0f)
{
	mTextColor = ColorRGBA( 1.0f, 0.0f, 0.0f, 1.0f );
	mSelTextColor = ColorRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	mSelBkColor = ColorRGBA(0, 0, 1, 1);
	mCaretColor = ColorRGBA( 0.0f, 0, 0, 1.0f);
}

LineEdit::~LineEdit()
{

}

void LineEdit::Update( float dt )
{
	// Blink the caret
	if (mCaretBlinkRate > 0.0f)
		mCaretBlinkTimer = fmodf(mCaretBlinkTimer + mCaretBlinkRate * dt, 1.0f);
	else
		mCaretBlinkTimer = 0.0f;

	if (HasFocus() && mCaretBlinkTimer > 0.5f)
		mCaretOn = true;
}

void LineEdit::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	int2 screenPos = GetScreenPosition();

	// Draw background first
	Rectanglef rect((float)screenPos.X(), (float)screenPos.Y(), (float)mSize.X(), (float)mSize.Y());
	spriteBatch.Draw(mStyle->StyleTex, rect, &mStyle->StyleStates[UI_State_Normal].TexRegion, mStyle->StyleStates[UI_State_Normal].TexColor);

	if (HasSelection())
	{
		int32_t selStart = mSelectStartPos;
		int32_t selEnd = mCaretPos;

		if (selStart > selEnd)
			std::swap(selStart, selEnd);

		float left = (mCharPositions[selStart] - mVisibleStartX);
		float right = (mCharPositions[selEnd] - mVisibleStartX);
			
		rect.SetLeft((std::max)(left, mTextRect.Left()));
		rect.SetRight((std::min)(right, mTextRect.Right()));
		rect.Y = mTextRect.Top() + (mTextRect.Height - mRowHeight) / 2;
		rect.Height = mRowHeight;

		spriteBatch.Draw(mStyle->StyleTex, rect, &mStyle->StyleStates[UI_State_Normal].TexRegion, mSelBkColor);
	}

	// Draw Text
	{
		Font& font = *mStyle->Font;

		shared_ptr<Texture> fontTex = font.GetFontTexture();

		const float fontScale = mStyle->FontSize / font.GetFontSize();

		// VCenter
		float baseline = mTextRect.Top() + (mTextRect.Height - mRowHeight) / 2 + font.GetBaseLine(fontScale);

		IntRect sourceRect;
		Rectanglef destRect;

		for (size_t i = 0; i < mText.length(); ++i)
		{
			wchar_t ch = mText[i];

			float start = mCharPositions[i] - mVisibleStartX ;
			float end = mCharPositions[i+1] - mVisibleStartX ;

			if (ch == L' ' || ch == L'\t')
				continue;

			const Font::Glyph& glyph = font.GetGlyphInfo(ch);

			float ch_x = start + glyph.OffsetX * fontScale;
			float ch_y = baseline - glyph.OffsetY * fontScale;

			float ch_width = glyph.Width * fontScale;
			float ch_height = glyph.Height* fontScale;

			// Out of region
			if ( (ch_x + ch_width <= mTextRect.Left()) || (ch_x >= mTextRect.Right()))
				continue;

			if (ch_x < mTextRect.Left())
			{
				float ratio = (mTextRect.Left() - ch_x) / ch_width;
				sourceRect.X = glyph.SrcX + int(glyph.Width * ratio);
				sourceRect.Width = int(glyph.Width * (1 - ratio));

				destRect.X = mTextRect.Left();
				destRect.Width = ch_width * (1 - ratio);
			}
			else if (ch_x + ch_width > mTextRect.Right())
			{
				float ratio = (ch_x + ch_width  - mTextRect.Right()) / ch_width;
				sourceRect.X = glyph.SrcX;
				sourceRect.Width = int(glyph.Width * (1 - ratio));

				destRect.X = ch_x;
				destRect.SetRight(mTextRect.Right());
			}
			else
			{
				sourceRect.X = glyph.SrcX;
				sourceRect.Width = glyph.Width;

				destRect.X = ch_x;
				destRect.Width = ch_width;
			}

			sourceRect.Y = glyph.SrcY;
			sourceRect.Height = glyph.Height;

			destRect.Y = ch_y;
			destRect.Height = ch_height;

			spriteBatch.Draw(fontTex, destRect, &sourceRect, mTextColor);
		}
	}

	// Draw Caret
	if (mCaretOn)
	{
		rect.X = Clamp(mCharPositions[mCaretPos] - mVisibleStartX, mTextRect.Left(), mTextRect.Right());
		rect.Y = mTextRect.Top() + (mTextRect.Height - mRowHeight) / 2;
		rect.Width = 1.0f;
		rect.Height = mRowHeight;

		spriteBatch.Draw(mStyle->StyleTex, rect, &mStyle->StyleStates[UI_State_Normal].TexRegion, mCaretColor);
	}
}

void LineEdit::InitGuiStyle( const GuiSkin::StyleMap* styles /*= nullptr*/ )
{
	if (styles)
	{
	}
	else
	{
		// Defalut
		GuiSkin* defaultSkin = UIManager::GetSingleton().GetDefaultSkin();

		// background in Normal State
		mStyle = &defaultSkin->TextEdit;
	}

	// Init row height
	const float fontScale = (float)mStyle->FontSize / mStyle->Font->GetFontSize();
	mRowHeight = mStyle->Font->GetRowHeight(fontScale);
}

void LineEdit::OnDragBegin( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton && mText.size())
	{
		if (this->IsInside(screenPos, true))
		{
			mCaretPos = GetCaretAtCuror((float)screenPos.X());
			mDragCursor = true;
			mSelectStartPos = mCaretPos;
		}	
	}
}

void LineEdit::OnDragMove( const int2& screenPos, uint32_t buttons )
{
	if ( mDragCursor && (buttons & InputEventType::LeftButtonMask) && mText.size() )
	{
		if (screenPos.X() < mTextRect.Left())
		{
			float actualLeft = mTextRect.Left() + mVisibleStartX;

			auto first = std::find_if(mCharPositions.rbegin(), mCharPositions.rend(), [=](float value){
												return value < actualLeft;});

			if (first != mCharPositions.rend())
				mVisibleStartX = *first - mTextRect.Left();
		}
		else if (screenPos.X() > mTextRect.Right())
		{
			float actualRight = mTextRect.Right() + mVisibleStartX;
			auto last = std::upper_bound(mCharPositions.begin(), mCharPositions.end(), actualRight);
			if (last != mCharPositions.end())
				mVisibleStartX = *last - mTextRect.Right();
		}

		mCaretPos = GetCaretAtCuror((float)screenPos.X());
	}
}

void LineEdit::OnDragEnd( const int2& screenPos )
{
	mDragCursor = false;
}

bool LineEdit::OnKeyPress( uint16_t key )
{
	switch (key)
	{
	case KC_LeftArrow:
		{
			if (mCaretPos > 0)
				PlaceCaret(mCaretPos - 1);
		}
		break;

	case KC_RightArrow:
		{
			if (mCaretPos < (int32_t)mCharPositions.size() - 1)
				PlaceCaret(mCaretPos + 1);
		}
		break;

	case KC_Delete:
		{
			if (HasSelection())
			{
				int32_t selStart = mSelectStartPos;
				int32_t selEnd = mCaretPos;

				if (selStart > selEnd)
					std::swap(selStart, selEnd);

				mText.erase(selStart, selEnd - selStart);
				UpdateText();

				PlaceCaret(selStart);
			}
			else
			{
				if (mCaretPos < (int32_t)mCharPositions.size() - 1)
				{
					mText.erase(mCaretPos, 1);
					UpdateText();
				}
			}	
		}
		break;

	case KC_BackSpace:
		{
			if (HasSelection())
			{
				int32_t selStart = mSelectStartPos;
				int32_t selEnd = mCaretPos;

				if (selStart > selEnd)
					std::swap(selStart, selEnd);

				mText.erase(selStart, selEnd - selStart );
				UpdateText();

				PlaceCaret(selStart);
			}
			else
			{
				if (mCaretPos > 0)
				{
					mText.erase(mCaretPos-1, 1);
					UpdateText();
					PlaceCaret(mCaretPos - 1);
				}
			}
		};
		break;

	case KC_Enter:
		{
			if (!EventReturnPressed.empty())
				EventReturnPressed(mText);
		}
		break;

	case KC_Home:
		PlaceCaret(0);
		break;

	case KC_End:
		PlaceCaret(mText.length());
		break;
	default:
		break;
	}

	return true;
}

bool LineEdit::OnTextInput( uint16_t unicode )
{
	bool eventConsumed = true;

	if (HasFocus() && unicode >= 0x20)
	{
		if (HasSelection())
		{
			int32_t selStart = mSelectStartPos;
			int32_t selEnd = mCaretPos;

			if (selStart > selEnd)
				std::swap(selStart, selEnd);

			mText.replace(selStart, selEnd - selStart, 1, (wchar_t)unicode);
			UpdateText();

			PlaceCaret(selStart+1);
		}
		else
		{
			mText.insert(mText.begin() + mCaretPos, (wchar_t)unicode);
			UpdateText();	
			PlaceCaret(mCaretPos+1);
		}
	}

	return eventConsumed;
}

bool LineEdit::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

void LineEdit::SetText( const std::wstring& text )
{
	mText = text;
	UpdateText();
}

void LineEdit::UpdateRect()
{
	int2 screenPos = GetScreenPosition();

	mTextRect.X = float( screenPos.X() + mBorder );
	mTextRect.Y = float( screenPos.Y() + mBorder / 2 );
	mTextRect.Width = float( mSize.X() - mBorder - mBorder );
	mTextRect.Height = float( mSize.Y() - mBorder / 2 - mBorder / 2 );

	mVisibleStartX = 0;

	UpdateText();
}

int32_t LineEdit::GetCaretAtCuror( float screenX ) const
{
	float actualX = screenX + mVisibleStartX;

	int32_t caret = mCharPositions.size() - 1;
	for (size_t i = 0; i < mCharPositions.size() - 1; ++i)
	{
		float half = (mCharPositions[i] + mCharPositions[i+1]) * 0.5f;

		if (actualX < half)
		{
			caret = (int32_t)i;
			break;
		}
	}

	return caret;
}

void LineEdit::UpdateText()
{
	
	const float fontScale = mStyle->FontSize / mStyle->Font->GetFontSize();

	// Keep track of every line char start pos
	float charX = mTextRect.Left();
	mCharPositions.resize(1, charX);
	for (size_t i = 0; i < mText.length(); ++i)
	{
		wchar_t ch = mText[i];

		if (ch == L'\t')
		{
			const Font::Glyph& glyph = mStyle->Font->GetGlyphInfo(L' ');
			charX += glyph.Advance * fontScale * 4;
		}
		else
		{
			const Font::Glyph& glyph = mStyle->Font->GetGlyphInfo(ch);
			charX += glyph.Advance * fontScale;
		}

		mCharPositions.push_back(charX);		
	}
}

void LineEdit::PlaceCaret( int32_t caret )
{
	if (caret != mCaretPos)
	{
		float actualLeft = mVisibleStartX + mTextRect.Left();
		float actualRight = mVisibleStartX + mTextRect.Right();

		if (mCharPositions.back() <= mTextRect.Right())
			mVisibleStartX = 0.0f;
		else 
		{
			if (mCharPositions[caret] < actualLeft)
				mVisibleStartX = (std::max)(mCharPositions[caret] - mTextRect.Left(), 0.0f);
			else if (mCharPositions[caret] > actualRight)
				mVisibleStartX = (std::min)(mCharPositions[caret] - mTextRect.Right(), mCharPositions.back() - mTextRect.Right());
		}

		mCaretPos = caret;
	}

	ClearSelection();
}

}