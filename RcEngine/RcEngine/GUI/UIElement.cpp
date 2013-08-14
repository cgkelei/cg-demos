#include <GUI/UIElement.h>
#include <GUI/UIManager.h>
#include <Math/MathUtil.h>
#include <Core/Exception.h>

namespace RcEngine {

UIElement::UIElement()
	: mParent(nullptr), 
	  mPositionDirty(false), 
	  mHovering(false), 
	  mVisible(true), 
	  mEnabled(true),
	  mPosition(int2::Zero()),
	  mSize(int2::Zero()),
	  mMinSize(int2::Zero()), mMaxSize(INT_MAX, INT_MAX)
{


}

UIElement::~UIElement()
{
	for (UIElement* child : mChildren)
		delete child;
	mChildren.clear();
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
	validateSize.X() = Clamp(size.X(), mMinSize.X(), mMaxSize.X());
	validateSize.Y() = Clamp(size.Y(), mMinSize.Y(), mMaxSize.Y());

	if (mSize != validateSize)
	{
		mSize = validateSize;
		MarkDirty();
		UpdateRect();
	}
}

int2 UIElement::GetScreenPosition()
{
	if (mPositionDirty)
	{
		int2 pos = mPosition;

		const UIElement* current = mParent;

		while(current)
		{
			pos += current->GetPosition();
			current = current->mParent;
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

void UIElement::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
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

void UIElement::OnHover( const int2& screenPos )
{
	mHovering = true;
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

void UIElement::OnDragBegin( const int2& screenPos, uint32_t buttons )
{
}

void UIElement::OnDragMove( const int2& screenPos, uint32_t buttons )
{

}

void UIElement::OnDragEnd( const int2& screenPos )
{

}

void UIElement::OnResize()
{

}

bool UIElement::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	return false;
}

bool UIElement::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	return false;
}

bool UIElement::CanHaveFocus() const
{
	return false;
}

void UIElement::UpdateRect()
{

}

void UIElement::Initialize( const GuiSkin::StyleMap* styles )
{

}



}