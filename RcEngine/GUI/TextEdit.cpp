#include <GUI/TextEdit.h>
#include <GUI/ScrollBar.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Input/InputEvent.h>
#include <Input/InputSystem.h>
#include <Core/Exception.h>
#include <cctype>

namespace RcEngine {

static const int32_t BorderWidth = 8;
static const int32_t ScrollBarWidth = 20;

TextEdit::TextEdit(TextEditType type)
	: mEditType(type),
	  mMultiLine(false),
	  mDragMouse(false),
	  mWordWrap(true),
	  mCaretBlinkRate(1.0f),
	  mCaretBlinkTimer(0.0f),
	  mBorder(BorderWidth),
	  mScrollBarWidth(ScrollBarWidth),
	  mTextEditStyle(nullptr),
	  mVertScrollBar(nullptr),
	  mCaretOn(false),
	  mNumLines(0),
	  mFirstVisibleY(0),
	  mNumVisibleY(0),
	  mCaretPos(0, 0),
	  mSelectStartPos(0, 0)
{
	mTextColor = ColorRGBA( 0.0f, 0.0f, 0.0f, 1.0f );
	mSelTextColor = ColorRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	mSelBkColor = ColorRGBA( 1.0f, 0.156f, 0.196f, 0.36f );
	mCaretColor = ColorRGBA( 1.0f, 0, 0, 0 );

	if (mEditType == LineEdit)
		mTextAlign = AlignLeft | AlignVCenter;
	else
		mTextAlign = AlignLeft | AlignTop;

	if (mEditType == TextBox)
	{
		mVertScrollBar = new ScrollBar(UI_Vertical);
		mVertScrollBar->SetScrollButtonRepeat(true);
		mVertScrollBar->SetVisible(false);
		mVertScrollBar->EventValueChanged.bind(this, &TextEdit::HandleVScrollBar);

		AddChild(mVertScrollBar);
	}
}

TextEdit::~TextEdit()
{

}

void TextEdit::SetText( const std::wstring& text )
{
	mText = text;
	UpdateText();
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

		if (mEditType == TextBox)
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
	//spriteBatch.Draw(mTextEditStyle->StyleTex, mBackRect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, mTextEditStyle->StyleStates[UI_State_Normal].TexColor);

	// Draw selected background if has selection
	DrawSelection(spriteBatch, spriteBatchFont);

	// Draw Text
	if (mPrintText.size())
	{
		DrawText(spriteBatch, spriteBatchFont);
	}

	// Draw Caret
	if (mCaretOn)
	{
		if (mEditType == LineEdit)
		{
			Rectanglef caretRC;

			caretRC.X = mCharPositions[0][mCaretPos.X()];
			caretRC.Y = mTextRect.Y;
			caretRC.Width = 1.0f;
			caretRC.Height = mRowHeight;

			spriteBatch.Draw(mTextEditStyle->StyleTex, caretRC, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 0, 1));
		}
		else
		{
			int32_t line = mCaretPos.Y() - mFirstVisibleY;

			if (line >= 0 && line < mNumVisibleY)
			{
				Rectanglef caretRC;

				caretRC.X = mCharPositions[mCaretPos.Y()][mCaretPos.X()];
				caretRC.Y = mTextRect.Y + line * mRowHeight;
				caretRC.Width = 1.0f;
				caretRC.Height = mRowHeight;

				spriteBatch.Draw(mTextEditStyle->StyleTex, caretRC, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 0, 1));
			}
		}
		
	}

	// Reset
	mHovering = false;
	mCaretOn = false;
}

void TextEdit::DrawText( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	mTextEditStyle->Font->DrawString(spriteBatchFont, mPrintText, mTextEditStyle->FontSize, mTextAlign, mTextRect, mTextColor);
}

void TextEdit::DrawSelection( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (mCaretPos != mSelectStartPos)
	{	
		int2 start(mSelectStartPos);
		int2 end(mCaretPos);

		if ( (start.Y() > end.Y()) || (start.Y() == end.Y() && start.X() > end.X()) )
			std::swap(start, end);

		if (mEditType == LineEdit)
		{
			
		}
		else
		{
			Rectanglef rect;
			rect.Height = mRowHeight;

			if (start.Y() != end.Y())
			{
				int32_t line;

				// First Line
				line = start.Y() - mFirstVisibleY;
				if (line >= 0 && line < mNumVisibleY)
				{
					rect.SetLeft( mCharPositions[start.Y()][start.X()] );
					rect.SetRight( mCharPositions[start.Y()].back() );
					rect.SetTop( mTextRect.Y + mRowHeight * line );		
					spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));
				}

				// Last Line
				line = end.Y() - mFirstVisibleY;
				if (line >= 0 && line < mNumVisibleY)
				{

					rect.SetLeft( mCharPositions[end.Y()].front() );
					rect.SetRight( mCharPositions[end.Y()][end.X()] );
					rect.SetTop( mTextRect.Y + mRowHeight * line );		
					spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));
				}

				for (int32_t i = start.Y() + 1; i < end.Y(); ++i)
				{
					line = i - mFirstVisibleY;
					if (line >= 0 && line < mNumVisibleY)
					{
						rect.SetLeft( mCharPositions[i].front() );
						rect.SetRight( mCharPositions[i].back() );
						rect.SetTop( mTextRect.Y + mRowHeight * line);	
						spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));
					}
				}
			}
			else
			{
				int32_t line = start.Y() - mFirstVisibleY;
				if (line >= 0 && line < mNumVisibleY)
				{
					rect.SetLeft( mCharPositions[start.Y()][start.X()] );
					rect.SetRight( mCharPositions[end.Y()][end.X()] );
					rect.SetTop( mTextRect.Y + mRowHeight * line );		
					spriteBatch.Draw(mTextEditStyle->StyleTex, rect, &mTextEditStyle->StyleStates[UI_State_Normal].TexRegion, ColorRGBA(0, 0, 1, 1));
				}
			}
		}
	}
}

void TextEdit::OnDragBegin( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton && mNumLines > 0)
	{
		if (mTextRect.Contains((float)screenPos.X(), (float)screenPos.Y()) )
		{
			mCaretPos = GetCaretAt(screenPos);
			mDragMouse = true;
			mSelectStartPos = mCaretPos;
		}
	}
}

void TextEdit::OnDragMove( const int2& screenPos, uint32_t buttons )
{
	if ( mDragMouse && (buttons & InputEventType::LeftButtonMask) && mNumLines > 0 )
	{
		if (mEditType == TextBox)
		{
			if ((float)screenPos.Y() < mTextRect.Top())
			{
				if (mVertScrollBar->IsVisible())
					mVertScrollBar->Scroll(-1);

				mCaretPos.Y() = (std::max)(0, mCaretPos.Y() - 1);
				mCaretPos.X() = (std::min)(mCaretPos.X(), (int32_t)mCharPositions[mCaretPos.Y()].size() - 1);
			}
			else if (screenPos.Y() > (float)mTextRect.Bottom())
			{
				if (mVertScrollBar->IsVisible())
					mVertScrollBar->Scroll(1);

				mCaretPos.Y() = (std::min)(mNumLines-1, mCaretPos.Y() + 1);
				mCaretPos.X() = (std::min)(mCaretPos.X(), (int32_t)mCharPositions[mCaretPos.Y()].size() - 1);
			}
			else
			{
				if (mTextRect.Contains(Clamp((float)screenPos.X(), mTextRect.Left(), mTextRect.Right()), (float)screenPos.Y()))
					mCaretPos = GetCaretAt(screenPos);
			}	
		}
		else
		{
			//// Line Edit
			//if ((float)screenPos.X() < mTextRect.Left() && mFirstVisibleX > 0)
			//{
			//	mFirstVisibleX -= 1;
			//	mTextAlign = AlignLeft | AlignVCenter;
			//	UpdateText();
			//}
			//else if ((float)screenPos.X() > mTextRect.Right())
			//{
			//	mFirstVisibleX += 1;
			//	mTextAlign = AlignRight | AlignVCenter;
			//	UpdateText();
			//}
			//else 
			//{
			//	float x = Clamp((float)screenPos.X(), mTextRect.Left(), mTextRect.Right());
			//	float y = Clamp((float)screenPos.Y(), mTextRect.Top(), mTextRect.Bottom());
			//	//if (mTextRect.Contains(x))
			//		mCaretPos = GetCaretAt(screenPos);
			//}
		}
	}
}

void TextEdit::OnDragEnd( const int2& position )
{
	mDragMouse = false;
}

bool TextEdit::OnMouseWheel( int32_t delta )
{
	if (mEditType == TextBox && mVertScrollBar->IsVisible())
		mVertScrollBar->Scroll(-delta);

	return true;
}

bool TextEdit::OnKeyPress( uint16_t key )
{
	switch (key)
	{
	case KC_LeftArrow:
		{
			if (mEditType == LineEdit)
			{
				if (mCaretPos.X() > 0)
					PlaceCaret(mCaretPos.X() - 1, mCaretPos.Y());
			}
			else
			{
				if (mCaretPos.X() > 0)
					PlaceCaret(mCaretPos.X() - 1, mCaretPos.Y());
				else if (mCaretPos.X() == 0 && mCaretPos.Y() > 0)
				{
					if (mCaretPos.Y() == mFirstVisibleY && mVertScrollBar->IsVisible())
						mVertScrollBar->Scroll(-1);

					PlaceCaret(mCharPositions[mCaretPos.Y()-1].size() - 1, mCaretPos.Y()-1);
				}
			}	
		}
		break;

	case KC_RightArrow:
		{
			if (mCaretPos.X() < (int32_t)mCharPositions[mCaretPos.Y()].size() - 1)
				PlaceCaret(mCaretPos.X() + 1, mCaretPos.Y());
			else 
			{
				if (mCaretPos.Y() < (int32_t)mCharPositions.size() - 1)
				{
					if ( (mCaretPos.Y() == mFirstVisibleY + mNumVisibleY - 1) && mVertScrollBar->IsVisible() )
						mVertScrollBar->Scroll(1);

					PlaceCaret(0, mCaretPos.Y()+1);
				}
			}
		}
		break;

	case KC_UpArrow:
		{
			if (mMultiLine && mCaretPos.Y() > 0)
			{
				if (mCaretPos.Y() == mFirstVisibleY && mVertScrollBar->IsVisible())
					mVertScrollBar->Scroll(-1);

				PlaceCaret((std::min)(mCaretPos.X(), (int32_t)mCharPositions[mCaretPos.Y()-1].size()-1), mCaretPos.Y()-1);
			}
		}
		break;

	case KC_DownArrow:
		{
			if (mMultiLine && (mCaretPos.Y() < (int32_t)mCharPositions.size() - 1) )
			{
				if ( (mCaretPos.Y() == mFirstVisibleY + mNumVisibleY - 1) && mVertScrollBar->IsVisible() )
					mVertScrollBar->Scroll(1);

				PlaceCaret((std::min)(mCaretPos.X(), (int32_t)mCharPositions[mCaretPos.Y()+1].size()-1), mCaretPos.Y()+1);
			}
		}
		break;

	case KC_PageDown:
		{
			if (mMultiLine && (mFirstVisibleY < mNumLines - mNumVisibleY) && mVertScrollBar->IsVisible() )
				mVertScrollBar->Scroll(mNumVisibleY);
		}
		break;

	case KC_PageUp:
		{
			if (mMultiLine && (mFirstVisibleY > 0) && mVertScrollBar->IsVisible() )
				mVertScrollBar->Scroll(-mNumVisibleY);
		}
		break;

	case KC_Home:
		PlaceCaret(0, mCaretPos.Y());
		break;

	case KC_End:
		PlaceCaret(mCharPositions[mCaretPos.Y()].size()-1 , mCaretPos.Y());
		break;

	case KC_Delete:
		{
			if (mCaretPos != mSelectStartPos)
				DeleteSlectedText();
			else
				DeleteNextChar();
		}
		break;

	case KC_BackSpace:
		{
			if (mCaretPos != mSelectStartPos)
				DeleteSlectedText();
			else
				DeletePreChar();
		}
		break;

	case KC_Enter:
		{
			if( mMultiLine )
			{
				size_t caretIdx = GetCharIndex(mCaretPos);

				mText.insert(mText.begin() + caretIdx, L'\n');
				UpdateText();		
				PlaceCaret(0, mCaretPos.Y() + 1);
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
		if (mSelectStartPos != mCaretPos)
		{
			int2 start(mSelectStartPos);
			int2 end(mCaretPos);

			if ( (start.Y() > end.Y()) || (start.Y() == end.Y() && start.X() > end.X()) )
				std::swap(start, end);

			size_t delStart = GetCharIndex(start);
			size_t delEnd = GetCharIndex(end);

			mText.replace(delStart, delEnd - delStart, 1, (wchar_t)unicode);

			UpdateText();		
			PlaceCaret(start.X() + 1, start.Y());

		}
		else
		{
			size_t start = GetCharIndex(mCaretPos);
			mText.insert(mText.begin() + start, (wchar_t)unicode);
			UpdateText();	

			int2 newCaretPos = GetCaretPos(start+1);		
			PlaceCaret(newCaretPos);

			if (mEditType == TextBox)
			{
				if (mVertScrollBar->IsVisible() &&  mCaretPos.Y() >= mFirstVisibleY + mNumVisibleY )
					mVertScrollBar->Scroll(1);
			}
			else
			{
				//if (mCaretPos.X() > mFirstVisibleX + mNumVisibleX)
				//{
				//	float ww = 0;
				//	mNumVisibleX = 0;
				//	int32_t i;
				//	for (i = mCaretPos.X(); i > 0; --i, ++mNumVisibleX)
				//	{
				//		if (ww > mTextRect.Width)
				//			break;

				//		ww += mCharPositions[0][i] - mCharPositions[0][i-1];
				//	}

				//	mFirstVisibleX = i;
				//	mTextAlign = AlignRight | AlignVCenter;

				//	UpdateVisisbleText();
				//}
			}
		}
	}

	return eventConsumed;
}

void TextEdit::PlaceCaret( const int2& newCaretPos )
{
	if (newCaretPos.Y() != mCaretPos.Y())
	{
		mCaretPos.Y() = Clamp(newCaretPos.Y(), 0, (int32_t)mCharPositions.size());
	}

	if (newCaretPos.X() != mCaretPos.X())
	{
		mCaretPos.X() = Clamp(newCaretPos.X(), 0, (int32_t)mCharPositions[mCaretPos.Y()].size());
	}

	mSelectStartPos = mCaretPos;
}

void TextEdit::PlaceCaret( int32_t newCaretX, int32_t newCaretY )
{
	if (newCaretY != mCaretPos.Y())
	{
		mCaretPos.Y() = Clamp(newCaretY, 0, (int32_t)mCharPositions.size());
	}

	if (newCaretX != mCaretPos.X())
	{
		mCaretPos.X() = Clamp(newCaretX, 0, (int32_t)mCharPositions[mCaretPos.Y()].size());
	}

	mSelectStartPos = mCaretPos;
}

void TextEdit::UpdateRect()
{
	UIElement::UpdateRect();

	if (!mTextEditStyle)
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Init Gui Style First!", "TextEdit::UpdateText");

	int2 screenPos = GetScreenPosition();

	mBackRect.X = (float)screenPos.X();
	mBackRect.Y = (float)screenPos.Y();
	mBackRect.Width = (float)mSize.X();
	mBackRect.Height = (float)mSize.Y();

	mTextRect.X = (float)screenPos.X() + mBorder;
	mTextRect.Width = (float)mSize.X() - mBorder - mBorder;

	mTextRect.Y = (float)screenPos.Y() + mBorder;
	mTextRect.Height = (float)mSize.Y() - mBorder - mBorder;

	if (mEditType == TextBox)
		mNumVisibleY = (int32_t)floor(mTextRect.Height / mRowHeight);
	else
		mNumVisibleY = 1;

	/*if (mEditType == LineEdit || (mEditType == TextBox && !mWordWrap))
		mLineStartX = mTextRect.Left();*/

	int32_t shrink = (int32_t)floor( mTextRect.Height - mNumVisibleY * mRowHeight );

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

	if (mEditType == TextBox)
	{
		WrapText();

		if (mNumLines > mNumVisibleY)
		{
			if ( mVertScrollBar->IsVisible() == false )
			{
				mVertScrollBar->SetVisible(true);
				mVertScrollBar->SetPosition(int2(mSize.X() - mScrollBarWidth, 0));
				mVertScrollBar->SetSize(int2(mScrollBarWidth, mSize.Y()));

				int32_t remain = mNumLines - mNumVisibleY;
				mVertScrollBar->SetScrollRange(0, remain);

				mTextRect.Width -= mScrollBarWidth;
				mBackRect.Width -= mScrollBarWidth;

				if (mWordWrap)
					WrapText();
			}	
			else 
			{
				int32_t remain = mNumLines - mNumVisibleY;
				
				if (mFirstVisibleY > remain)
					mFirstVisibleY = remain;

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

				mFirstVisibleY = 0;
			}
		}
	}
	else
	{
		mWrappedText = mText;
		mNumLines = std::count(mWrappedText.begin(), mWrappedText.end(), L'\n') + 1;
	}

	// Keep track of every line char start pos
	float charX = mTextRect.X;
	mCharPositions.resize(1);
	mCharPositions.back().resize(1, charX);
	for (size_t i = 0; i < mWrappedText.length(); ++i)
	{
		wchar_t ch = mWrappedText[i];

		if (ch == L'\n')
		{
			mCharPositions.resize(mCharPositions.size() + 1);

			charX = (float)mTextRect.X;
			mCharPositions.back().push_back(charX);
		}
		else
		{
			const Font::Glyph& glyph = mTextEditStyle->Font->GetGlyphInfo(ch);

			charX += glyph.Advance * fontScale;
			mCharPositions.back().push_back(charX);		
		}
	}
	
	//if (mEditType == LineEdit && mWrappedText.length())
	//{
	//	mNumVisibleX = 0;
	//	for (int32_t i = mFirstVisibleX; i < (int32_t)mWrappedText.length(); ++i, mNumVisibleX++)
	//	{
	//		if (mCharPositions[0][i+1] > mTextRect.Right())
	//			break;
	//	}
	//}

	UpdateVisisbleText();
}

void TextEdit::UpdateVisisbleText()
{
	if (mEditType == LineEdit)
	{
		//mPrintText = mWrappedText.substr(mFirstVisibleX, mNumVisibleX);
	}
	else
	{
		size_t start = 0;
		size_t end = mWrappedText.length() - 1;

		if (mFirstVisibleY > 0)
		{
			size_t lineIdx = 0;
			for (size_t i = 0; i < mWrappedText.length(); ++i)
			{
				if (mWrappedText[i] == L'\n')
				{
					if (++lineIdx == mFirstVisibleY)
					{
						start = i+1;
						break;
					}
				}
			}
		}

		// Update print text
		size_t numLines = (std::min)(mNumVisibleY, mNumLines);
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

void TextEdit::HandleVScrollBar( int32_t value )
{
	int32_t minValue, maxValue;
	mVertScrollBar->GetScrollRange(&minValue, &maxValue);

	mFirstVisibleY = (size_t)value;

	UpdateVisisbleText();
}

void TextEdit::WrapText()
{
	const float fontScale = (float)mTextEditStyle->FontSize / mTextEditStyle->Font->GetFontSize();

	mNumLines = 0;

	mWrappedText.clear();
	mWrappedBreakAt.clear();

	if (mText.empty())
		return;

	// Wrap Text if enable
	if (mWordWrap && mEditType == TextBox)
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
					x = 0; y += mRowHeight;

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

				x = 0; y += mRowHeight;
				wasBreakable = false;
				wasParens = false;
				lastBreak = -1;
				lineWidth = 0;
				mNumLines++;
			}

		}

		mNumLines++;
		textwidth = (std::max)(textwidth, x);
		y += mRowHeight;
		mWrappedText += mText.substr( start );
	}
	else
	{
		mWrappedText = mText;
		mNumLines = std::count(mWrappedText.begin(), mWrappedText.end(), L'\n') + 1;
	}
}

int2 TextEdit::GetCaretAt( const int2& screenPos) const
{
	int2 caret(0, 0);

	if (mEditType == LineEdit)
	{
		float actualX = (screenPos.X() );

		caret.X() = mCharPositions[0].size() - 1;
		for (size_t i = 0; i < mCharPositions[0].size() - 1; ++i)
		{
			float half = (mCharPositions[0][i] + mCharPositions[0][i+1]) * 0.5f;

			if (actualX < half)
			{
				caret.X() = (int32_t)i;
				break;
			}
		}
	}
	else
	{
		float actualX;

		/*if (!mWordWrap)	
			actualX = mFirstVisibleX + (screenPos.X() - mTextRect.Left()); 
		else
			actualX = screenPos.X();*/

		actualX = screenPos.X();

		caret.Y() = Clamp((int32_t)floorf((screenPos.Y() - mTextRect.Y) / mRowHeight), 0, mNumVisibleY - 1);
		caret.Y() = Clamp( caret.Y() + mFirstVisibleY, 0, mNumLines-1);

		caret.X() = mCharPositions[caret.Y()].size() - 1;
		for (size_t i = 0; i < mCharPositions[caret.Y()].size() - 1; ++i)
		{
			float half = (mCharPositions[caret.Y()][i] + mCharPositions[caret.Y()][i+1]) * 0.5f;

			if (actualX < half)
			{
				caret.X() = (int32_t)i;
				break;
			}
		}
	}

	return caret;
}

int2 TextEdit::GetCaretPos( int32_t index ) const
{
	int2 caret(0, 0);

	size_t wrappedIndex = index;
	for (size_t i = 0; i < mWrappedBreakAt.size(); ++i)
	{
		if ((int32_t)index > mWrappedBreakAt[i]+1)
			wrappedIndex++;
		else
			break;
	}

	for (size_t i = 0; i < wrappedIndex; ++i)
	{
		if (mWrappedText[i] == L'\n')
		{
			caret.Y()++;
			caret.X() = 0;
		}
		else
			caret.X()++;
	}

	return caret;
}

size_t TextEdit::GetCharIndex( const int2& caret ) const
{
	size_t wrappedIndex = 0;
	for (int32_t i = 0; i < caret.Y(); ++i)
		wrappedIndex += mCharPositions[i].size();

	wrappedIndex += caret.X();

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

void TextEdit::DeleteSlectedText()
{
	size_t x = 0, y = 0;
	
	int2 start(mSelectStartPos);
	int2 end(mCaretPos);

	if ( (start.Y() > end.Y()) || (start.Y() == end.Y() && start.X() > end.X()) )
		std::swap(start, end);
		
	size_t delStart = GetCharIndex(start);
	size_t delEnd = GetCharIndex(end);

	mText.erase(delStart, delEnd - delStart);
	UpdateText();
	
	int2 newCaretPos = GetCaretPos(delStart);		
	PlaceCaret(newCaretPos);

	//std::wcout << mText << std::endl;
}

void TextEdit::DeleteNextChar()
{
	size_t delStart = GetCharIndex(mCaretPos);

	if (delStart < mText.size())
	{
		mText.erase(delStart, 1);
		UpdateText();

		int2 newCaretPos = GetCaretPos(delStart);		
		PlaceCaret(newCaretPos);
	}	

	//std::wcout << mText << std::endl;
}

void TextEdit::DeletePreChar()
{
	size_t delStart = GetCharIndex(mCaretPos);
	if (delStart > 0)
	{
		auto c = mText[delStart-1];

		mText.erase(delStart-1, 1);
		UpdateText();

		int2 newCaretPos = GetCaretPos(delStart-1);		
		PlaceCaret(newCaretPos);
	}

	//std::wcout << mText << std::endl;
}



}