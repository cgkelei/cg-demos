#include <GUI/UIElement.h>
#include <GUI/UIManager.h>
#include <Math/MathUtil.h>

namespace RcEngine {

UIElement::UIElement()
	: mParent(nullptr), 
	  mPositionDirty(false), 
	  mHovering(false), 
	  mVisible(true), 
	  mEnabled(false),
	  mHorizontalAlignment(HA_Left),
	  mVerticalAlignment(VA_Top)
{


}

UIElement::~UIElement()
{

}

void UIElement::SetPosition( const int2& position )
{
	if (mPosition != position)
	{
		mPosition = position;
		MarkDirty();
	}
}


void UIElement::SetVisible( bool visible )
{
	mVisible = visible;
}


void UIElement::SetSize( const int2& size )
{
	int2 validateSize;
	validateSize.X() = Math<int2::value_type>::Clamp(size.X(), mMinSize.X(), mMaxSize.X());
	validateSize.Y() = Math<int2::value_type>::Clamp(size.Y(), mMinSize.Y(), mMaxSize.Y());

	if (mSize != validateSize)
	{
		mSize = validateSize;
		MarkDirty();
	}
}

int2 UIElement::GetScreenPosition()
{
	if (mPositionDirty)
	{
		int2 pos = mPosition;
		const UIElement* parent = mParent;
		const UIElement* current = this;

		while(parent)
		{
			switch (current->mHorizontalAlignment)
			{
			case HA_Left:
				pos.X() += parent->mPosition.X();
				break;

			case HA_Center:
				pos.X() += parent->mPosition.X() + (parent->mSize.X() - current->mSize.X()) / 2;
				break;

			case HA_Right:
				pos.X() += parent->mPosition.X() + (parent->mSize.X() - current->mSize.X());
				break;
			}

			switch (current->mVerticalAlignment)
			{
			case VA_Top:
				pos.Y() += parent->mPosition.Y();
				break;

			case VA_Center:
				pos.Y() += parent->mPosition.Y() + (parent->mSize.Y() - current->mSize.Y()) / 2;
				break;

			case VA_Bottom:
				pos.Y() += parent->mPosition.Y() + (parent->mSize.Y() - current->mSize.Y());
				break;
			}
			current = parent;
			parent = parent->mParent;
		}

		mScreenPosition = pos;
		mPositionDirty = false;
	}

	return mScreenPosition;
}

IntRect UIElement::GetArea() const
{
	return IntRect(mPosition.X(), mPosition.Y(), mSize.X(), mSize.Y());
}

IntRect UIElement::GetGlobalArea()
{
	int2 screenPos = GetScreenPosition();
	return IntRect(screenPos.X(), screenPos.Y(), mSize.X(), mSize.Y());
}

void UIElement::MarkDirty()
{
	mPositionDirty = true;

	for (UIElement* child : mChildren)
		child->MarkDirty();
}

uint32_t UIElement::GetNumChildren( bool recursive /*= false*/ ) const
{
	uint32_t allChildren =  mChildren.size();

	if (recursive)
	{
		for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
			allChildren += (*iter)->GetNumChildren(true);
	}

	return allChildren;
}

UIElement* UIElement::GetChild( const String& name, bool recursive /*= false*/ ) const
{
	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		if ((*iter)->GetName() == name)
		{
			return *iter;
		}

		if (recursive)
		{
			UIElement* element =  (*iter)->GetChild(name, true);
			if (element)
			{
				return element;
			}
		}
	}

	return nullptr;
}

UIElement* UIElement::GetChild( uint32_t index ) const
{
	return index < mChildren.size() ? mChildren[index] : nullptr;
}

RcEngine::int2 UIElement::ScreenToClient( const int2& screenPosition )
{
	return screenPosition - GetScreenPosition();
}

RcEngine::int2 UIElement::ClientToScreen( const int2& position )
{
	return position + GetScreenPosition();
}

bool UIElement::IsInside(int2 position, bool isScreen )
{
	if (isScreen)
		position = ScreenToClient(position);
	return position.X() >= 0 && position.Y() >= 0 && position.X() < mSize.X() && position.Y() < mSize.Y();
}

void UIElement::FlattenChildren( std::vector<UIElement*>& children) const
{
	for (UIElement* element : mChildren)
	{
		children.push_back(element);

		if (element->GetNumChildren())
			element->FlattenChildren(children);
	}
}


void UIElement::Draw()
{

}


void UIElement::BringToFront()
{

}

void UIElement::RemoveChild( UIElement* child )
{
	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		if ((*iter) == child)
		{
			mChildren.erase(iter);
			return;
		}
	}
}

void UIElement::Remove()
{
	if (mParent)
	{
		mParent->RemoveChild(this);
	}
}

void UIElement::AddChild( UIElement* child )
{
	if (child)
	{
		child->Remove();
		child->mParent = this;
		child->MarkDirty();
		mChildren.push_back(child);
	}
}

void UIElement::SetParent( UIElement* parent )
{
	if (parent)
		parent->AddChild(this);
}

bool UIElement::HasFocus() const
{
	return UIManager::GetSingleton().GetFocusElement() == this;
}

void UIElement::Update( float delta )
{

}

bool UIElement::OnMouseHover( const int2& position, uint32_t buttons )
{
	return false;
}

bool UIElement::OnMouseWheel( int32_t delta )
{
	return false;
}

bool UIElement::OnKeyPress( uint16_t key )
{
	return false;
}

bool UIElement::OnKeyRelease( uint16_t key )
{
	return false;
}

bool UIElement::OnTextInput( uint16_t unicode )
{
	return false;
}

void UIElement::OnDragBegin( const int2& position, uint32_t buttons )
{
}

void UIElement::OnDragMove( const int2& position, uint32_t buttons )
{

}

void UIElement::OnDragEnd( const int2& position )
{

}

bool UIElement::OnClick( const int2& position, uint32_t buttons )
{
	return false;
}


}