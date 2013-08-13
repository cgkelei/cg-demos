#include <GUI/ListBox.h>
#include <Graphics/Font.h>
#include <Graphics/SpriteBatch.h>

namespace RcEngine {

ListBox::ListBox()
{

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
}

void ListBox::InsertItem( int32_t index, const String& text )
{
	ListBoxItem* newItem = new ListBoxItem;

	newItem->Text = text;
	newItem->Selected = false;

	mItems.insert(mItems.begin() + index, newItem);
}

void ListBox::RemoveItem( int32_t index )
{
	if (index < 0 || index >= mItems.size())
		return;

	ListBoxItem* item = mItems[index];
	delete item;

	mItems.erase(mItems.begin() + index);

	if (m)
	{
	}
}

void ListBox::RemoveAllItems()
{
	for (ListBoxItem* item : mItems)
		delete item;

	mItems.clear();
	

}

}