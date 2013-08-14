#include <GUI/ListBox.h>
#include <GUI/ScrollBar.h>
#include <GUI/UIManager.h>
#include <Graphics/Font.h>
#include <Graphics/SpriteBatch.h>

namespace RcEngine {

ListBox::ListBox()
	: mSelectedIndex(-1),
	  mNumVisibleItems(0)
{
	mHorzScrollBar = new ScrollBar();
	mHorzScrollBar->SetOrientation( UI_Horizontal );
	mHorzScrollBar->SetVisible(false);

	mVertScrollBar = new ScrollBar();
	mVertScrollBar->SetOrientation( UI_Vertical );
	mVertScrollBar->SetVisible(false);

	AddChild(mHorzScrollBar);
	AddChild(mVertScrollBar);

}

ListBox::~ListBox()
{

}

void ListBox::AddItem( const String& text )
{
	ListBoxItem* newItem = new ListBoxItem;

	newItem->Text = text;
	newItem->Selected = false;

	mItems.push_back(newItem);
	mHorzScrollBar->SetRange(0, mItems.size());
}

void ListBox::InsertItem( int32_t index, const String& text )
{
	ListBoxItem* newItem = new ListBoxItem;

	newItem->Text = text;
	newItem->Selected = false;

	mItems.insert(mItems.begin() + index, newItem);
	mHorzScrollBar->SetRange(0, mItems.size());
}

void ListBox::RemoveItem( int32_t index )
{
	if (index < 0 || index >= (int32_t)mItems.size())
		return;

	ListBoxItem* item = mItems[index];
	delete item;

	mItems.erase(mItems.begin() + index);
	mHorzScrollBar->SetRange(0, mItems.size());

	if( mSelectedIndex >= ( int )mItems.size() )
		mSelectedIndex = mItems.size() - 1;
}

void ListBox::RemoveAllItems()
{
	for (ListBoxItem* item : mItems)
		delete item;

	mItems.clear();
	

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

		mHorzScrollBar->Initialize(nullptr);
		mVertScrollBar->Initialize(nullptr);
	}
}

void ListBox::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	if (mItems.size())
	{
		int mimItem, maxItem;
		mVertScrollBar->GetRange(mimItem, maxItem);

		float fontScale = mBackgroundStyle->mFontSize / float(mBackgroundStyle->mFont->GetFontSize());
		int32_t rowHeight = int32_t(fontScale * mBackgroundStyle->mFont->GetRowHeight());

		for (int i = mVertScrollBar->GetValue(); i < maxItem; ++i)
		{
						
		}
	}
}

void ListBox::Update( float delta )
{

}

void ListBox::UpdateRect()
{
	UIElement::UpdateRect();


}

void ListBox::OnResize()
{

}

}