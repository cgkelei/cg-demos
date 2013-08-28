#include <GUI/ListBox.h>
#include <GUI/ScrollBar.h>
#include <GUI/UIManager.h>
#include <Graphics/Font.h>
#include <Graphics/SpriteBatch.h>
#include <Input/InputSystem.h>

namespace RcEngine {

static const int32_t ScrollBarWidth = 20;

ListBox::ListBox()
	: mSelectedIndex(-1),
	  mNumVisibleItems(0),
	  mScrollBarWidth(ScrollBarWidth),
	  mBorder(6), mMargin(5),
	  mPressed(false)
{
	mVertScrollBar = new ScrollBar(UI_Vertical);
	mVertScrollBar->SetScrollButtonRepeat(false);
	mVertScrollBar->SetVisible(false);

	AddChild(mVertScrollBar);
}

ListBox::~ListBox()
{
	RemoveAllItems();
}

bool ListBox::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

bool ListBox::HasCombinedFocus() const
{
	if (!mVisible || !mEnabled)
		return false;

	if (mVertScrollBar->HasCombinedFocus())
		return true;
	else 
		return HasFocus();
}

void ListBox::AddItem( const std::wstring& text )
{
	mItems.push_back(text);
	UpdateRect();
}

void ListBox::InsertItem( int32_t index, const std::wstring& text )
{
	mItems.insert(mItems.begin() + index, text);
	UpdateRect();
}

void ListBox::RemoveItem( int32_t index )
{
	if (index < 0 || index >= (int32_t)mItems.size())
		return;

	mItems.erase(mItems.begin() + index);

	if( mSelectedIndex >= ( int )mItems.size() )
		mSelectedIndex = mItems.size() - 1;

	UpdateRect();
}

void ListBox::RemoveAllItems()
{
	mItems.clear();
}

void ListBox::UpdateRect()
{
	UIElement::UpdateRect();

	int2 screenPos = GetScreenPosition();

	mSelectionRegion.X = screenPos.X() + mBorder;
	mSelectionRegion.Width = mSize.X() - mBorder - mBorder;
	mSelectionRegion.Y = screenPos.Y() + mBorder;
	mSelectionRegion.Height = mSize.Y() - mBorder - mBorder;

	UpdateVScrollBar();

	mTextRegion = mSelectionRegion;
	mTextRegion.SetLeft( mSelectionRegion.X  + mMargin );
	mTextRegion.SetRight( mSelectionRegion.Right() - mMargin );
}

void ListBox::UpdateVScrollBar()
{
	if (mLisBoxStyle)
	{
		mNumVisibleItems = (int32_t)floorf(mSelectionRegion.Height / mTextRowHeight);

		if ((int)mItems.size() > mNumVisibleItems)
		{
			if (mVertScrollBar->IsVisible() == false)
			{
				mVertScrollBar->SetVisible(true);
				mVertScrollBar->SetPosition(int2(mSize.X() - mScrollBarWidth, 0));
				mVertScrollBar->SetSize(int2(mScrollBarWidth, mSize.Y()));
				mVertScrollBar->SetScrollRange(0, (int)mItems.size() - mNumVisibleItems);

				mSelectionRegion.Width -= mScrollBarWidth;
			}
			else
				mVertScrollBar->SetScrollRange(0, (int)mItems.size() - mNumVisibleItems);
		}
		else
		{
			if (mVertScrollBar->IsVisible())
			{
				mVertScrollBar->SetVisible(false);
				mSelectionRegion.Width += mScrollBarWidth;
			}
		}
	}
}


void ListBox::InitGuiStyle( const GuiSkin::StyleMap* styles /* = nullptr */ )
{
	if (styles)
	{

	}
	else
	{
		// use defualt
		GuiSkin* defaultSkin = UIManager::GetSingleton().GetDefaultSkin();
		mVertScrollBar->InitGuiStyle(nullptr);

		mLisBoxStyle = &defaultSkin->ListBox;
	}

	float fontScale = mLisBoxStyle->FontSize / float(mLisBoxStyle->Font->GetFontSize());
	mTextRowHeight = mLisBoxStyle->Font->GetRowHeight(fontScale);
}

void ListBox::Update( float delta )
{

}

void ListBox::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	int2 screenPos = GetScreenPosition();

	// Draw background first
	Rectanglef backRC((float)screenPos.X(), (float)screenPos.Y(), (float)mSize.X(), (float)mSize.Y());
	spriteBatch.Draw(mLisBoxStyle->StyleTex, backRC, &mLisBoxStyle->StyleStates[UI_State_Normal].TexRegion, mLisBoxStyle->StyleStates[UI_State_Normal].TexColor);

	if (mItems.size())
	{
		int mimItem, maxItem;
		mVertScrollBar->GetScrollRange(&mimItem, &maxItem);

		Rectanglef rc((float)mTextRegion.X, (float)mTextRegion.Y, (float)mTextRegion.Width, (float)mTextRegion.Height);
		rc.SetBottom( rc.Y + mTextRowHeight);

		for (int i = mVertScrollBar->GetScrollValue(); i < maxItem + mNumVisibleItems; ++i)
		{		
			if (rc.Bottom() > (float)mTextRegion.Bottom())
				break;

			bool selectedStyle = false;

			if (i == mSelectedIndex)
				selectedStyle = true;

			if( selectedStyle )
			{
				// Draw selected highlight
				Rectanglef rcSel;
				rcSel.SetLeft((float)mSelectionRegion.Left());
				rcSel.SetRight((float)mSelectionRegion.Right());
				rcSel.SetTop((float)rc.Top());
				rcSel.SetBottom((float)rc.Bottom());
				
				spriteBatch.Draw(mLisBoxStyle->StyleTex, rcSel, &mLisBoxStyle->StyleStates[UI_State_Hover].TexRegion, mLisBoxStyle->StyleStates[UI_State_Hover].TexColor);
			}

			// draw text
			//Rectanglef region((float)rc.X, (float)rc.Y, (float)rc.Width, (float)rc.Height);
			mLisBoxStyle->Font->DrawString(spriteBatchFont, mItems[i], mLisBoxStyle->FontSize,
					 AlignVCenter, rc, mLisBoxStyle->ForeColor); 

			rc.Offset(0, mTextRowHeight);
		}
	}
}

void ListBox::OnResize()
{

}

int32_t ListBox::GetSelectedIndex() const
{
	if (mItems.size() == 0)
		return -1;
	else 
		return mSelectedIndex;
}

void ListBox::SetSelectedIndex( int32_t index, bool fromInput /*= false*/ )
{
	int32_t oldSelectedIndex = mSelectedIndex;

	mSelectedIndex = Clamp(index, 0, (int32_t)mItems.size());

	if (!EventSelection.empty())
		EventSelection(mSelectedIndex);

	if (!EventSelectionChanged.empty() && mSelectedIndex != oldSelectedIndex)
		EventSelectionChanged(mSelectedIndex);
}

void ListBox::SetScrollBarWidth( int32_t width )
{
	mScrollBarWidth = width;
	UpdateRect();
}

void ListBox::SetBorder( int32_t border, int32_t margin )
{
	mBorder = border;
	mMargin = margin;
}

bool ListBox::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	bool eventConsumed = false;
	
	if (button == MS_LeftButton)
	{
		if (mItems.size())
			mPressed = true;

		eventConsumed = true;
	}

	return eventConsumed;
}

bool ListBox::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	bool eventConsumed = false;

	if (button == MS_LeftButton)
	{
		if (mPressed)
		{
			if (mSelectionRegion.Contains(screenPos.X(), screenPos.Y()))
			{
				int32_t selIndex = mVertScrollBar->GetScrollValue() + (int32_t)floorf((screenPos.Y() - mSelectionRegion.Top()) / mTextRowHeight);
				SetSelectedIndex(selIndex, true);			
			}	

			mPressed = false;
			eventConsumed = true;
		}
	}

	return eventConsumed;
}

}