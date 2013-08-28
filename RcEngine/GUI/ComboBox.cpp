#include <GUI/ComboBox.h>
#include <GUI/ListBox.h>
#include <GUI/Button.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Core/Exception.h>

namespace RcEngine {

ComboBox::ComboBox()
	: UIElement(),
	  mDropHeight(100),
	  mSelectedIndex(0)
{
	mChildOutside = true;

	mDropButton = new Button();
	mDropButton->SetName("ComboBox DropButton");
	mDropButton->SetPressedOffset(int2::Zero());
	mDropButton->SetHoverOffset(int2::Zero());
	mDropButton->EventButtonClicked.bind(this, &ComboBox::ToggleDropDownList);

	mItemList = new ListBox();
	mItemList->SetName("ComboBox ItemList");
	mItemList->SetVisible(false);
	mItemList->SetPriority(UI_DropPopPriority);
	mItemList->EventSelectionChanged.bind(this, &ComboBox::HandleSelection);

	AddChild(mDropButton);
	AddChild(mItemList);
}

ComboBox::~ComboBox()
{

}

bool ComboBox::CanHaveFocus() const
{
	return mEnabled && mVisible;
}

bool ComboBox::HasCombinedFocus() const
{
	if (mItemList->HasCombinedFocus())
		return true;
	else if (mDropButton->HasFocus())
		return true;
	return HasFocus();
}

void ComboBox::AddItem( const std::wstring& text )
{
	mItemList->AddItem(text);
}

void ComboBox::InsertItem( int32_t index, const std::wstring& text )
{
	mItemList->InsertItem(index, text);
}

void ComboBox::SetSelectedIndex( int32_t index )
{
	mSelectedIndex = Clamp(index, 0, mItemList->GetNumItems());
	mItemList->SetSelectedIndex(index);
	mDropButton->SetText(mItemList->GetSelectedItem());
}

int32_t ComboBox::GetSelectedIndex() const
{
	return mItemList->GetSelectedIndex();
}

void ComboBox::InitGuiStyle( const GuiSkin::StyleMap* styles /*= nullptr*/ )
{
	if (styles)
	{

	}
	else
	{
		// Default
		GuiSkin* defalutSkin = UIManager::GetSingleton().GetDefaultSkin();

		GuiSkin::StyleMap styleMap;
		styleMap[Button::StyleName] = &defalutSkin->ComboDropButton;
		mDropButton->InitGuiStyle(&styleMap);

		mItemList->InitGuiStyle(nullptr);
	}
}

void ComboBox::Update( float dt )
{
	if (mItemList->IsVisible() && !HasCombinedFocus())
		ToggleDropDownList();
}

void ComboBox::OnHover( const int2& screenPos )
{

}

void ComboBox::UpdateRect()
{
	UIElement::UpdateRect();

	mDropButton->SetPosition(int2(0, 0));
	mDropButton->SetSize(mSize);

	mItemList->SetPosition(int2(0, mSize.Y()));
	mItemList->SetSize(int2(mSize.X(), mDropHeight));
}

void ComboBox::SetDropHeight( int32_t height )
{
	mDropHeight = height;
	UpdateRect();
}

void ComboBox::ToggleDropDownList()
{
	if (mItemList->IsVisible())
	{
		SetPriority(0);
		mItemList->SetVisible(false);
	}
	else
	{
		// temporary promote ComBoBox priority
		SetPriority(UI_DropPopPriority);
		mItemList->SetVisible(true);
	}

	
}

void ComboBox::HandleSelection( int32_t selIdx )
{
	if (selIdx != mSelectedIndex)
	{
		mDropButton->SetText(mItemList->GetItem(selIdx));
		mSelectedIndex = selIdx;

		if( !EventSelectionChanged.empty() )
			EventSelectionChanged(selIdx);		
	}

	ToggleDropDownList();
}

}