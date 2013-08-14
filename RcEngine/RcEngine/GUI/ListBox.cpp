#include <GUI/ListBox.h>
#include <GUI/ScrollBar.h>
#include <GUI/UIManager.h>
#include <Graphics/Font.h>
#include <Graphics/SpriteBatch.h>
#include <Input/InputSystem.h>

namespace RcEngine {

ListBox::ListBox()
	: mSelectedIndex(-1),
	  mNumVisibleItems(0),
	  mScrollBarWidth(16),
	  mBorder(6), mMargin(5),
	  mPressed(false)
{
	mVertScrollBar = new ScrollBar();
	mVertScrollBar->SetOrientation( UI_Vertical );
	//mVertScrollBar->SetVisible(false);

	AddChild(mVertScrollBar);
}

ListBox::~ListBox()
{
	RemoveAllItems();
}

void ListBox::AddItem( const std::wstring& text )
{
	mItems.push_back(text);
	mVertScrollBar->SetRange(0, (int)mItems.size());
}

void ListBox::InsertItem( int32_t index, const std::wstring& text )
{
	mItems.insert(mItems.begin() + index, text);
	mVertScrollBar->SetRange(0, (int)mItems.size());
}

void ListBox::RemoveItem( int32_t index )
{
	if (index < 0 || index >= (int32_t)mItems.size())
		return;

	mItems.erase(mItems.begin() + index);
	mVertScrollBar->SetRange(0, (int)mItems.size());

	if( mSelectedIndex >= ( int )mItems.size() )
		mSelectedIndex = mItems.size() - 1;
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
	mSelectionRegion.Width = mSize.X() - mScrollBarWidth - mBorder - mBorder;
	mSelectionRegion.Y = screenPos.Y() + mBorder;
	mSelectionRegion.Height = mSize.Y() - mBorder - mBorder;

	mTextRegion = mSelectionRegion;
	mTextRegion.SetLeft( mSelectionRegion.X  + mMargin );
	mTextRegion.SetRight( mSelectionRegion.Right() - mMargin );

	mVertScrollBar->SetPosition(int2(mSize.X() - mScrollBarWidth, 0));
	mVertScrollBar->SetSize(int2(mScrollBarWidth, mSize.Y()));
}

void ListBox::Initialize( const GuiSkin::StyleMap* styles /* = nullptr */ )
{
	if (styles)
	{

	}
	else
	{
		// use defualt
		GuiSkin* defaultSkin = UIManager::GetSingleton().GetDefaultSkin();
		mVertScrollBar->Initialize(nullptr);

		mLisBoxStyle = &defaultSkin->ListBox;
	}

	float fontScale = mLisBoxStyle->FontSize / float(mLisBoxStyle->Font->GetFontSize());
	mTextRowHeight = int32_t(fontScale * mLisBoxStyle->Font->GetRowHeight());

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
		mVertScrollBar->GetRange(mimItem, maxItem);

		IntRect rc = mTextRegion;
		rc.SetBottom( rc.Y + mTextRowHeight);

		//mSelectedIndex =1;

		for (int i = mVertScrollBar->GetValue(); i < maxItem; ++i)
		{		
			if (rc.Bottom() > mTextRegion.Bottom())
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
			Rectanglef region((float)rc.X, (float)rc.Y, (float)rc.Width, (float)rc.Height);
			mLisBoxStyle->Font->DrawString(spriteBatchFont, mItems[i], mLisBoxStyle->FontSize,
					 AlignVCenter, region, mLisBoxStyle->ForeColor); 

			rc.Offset(0, mTextRowHeight);
		}
	}
}

void ListBox::OnResize()
{

}

int32_t ListBox::GetSelectecIndex() const
{
	if (mItems.size() == 0)
		return -1;
	else 
		return mSelectedIndex;
}

void ListBox::SetSelectedIndex( int32_t index )
{
	int32_t oldSelectedIndex = mSelectedIndex;

	mSelectedIndex = Clamp(index, 0, (int32_t)mItems.size());

	if (mSelectedIndex != oldSelectedIndex)
	{
		// Adjust scroll bar
		//mVertScrollBar->set
	}
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
				SetSelectedIndex( mVertScrollBar->GetValue() + (screenPos.Y() - mSelectionRegion.Top()) / mTextRowHeight );			
			}	

			mPressed = false;
			eventConsumed = true;
		}
	}

	return eventConsumed;
}

bool ListBox::CanHaveFocus() const
{
	return mVisible && mEnabled;
}



}