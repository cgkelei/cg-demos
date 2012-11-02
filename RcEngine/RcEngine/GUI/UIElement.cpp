#include <GUI/UIElement.h>
#include <Math/MathUtil.h>

namespace RcEngine {

UIElement::UIElement()
	: mParent(nullptr), mPositionDirty(false)
{

}

UIElement::~UIElement()
{

}

IntVector2 UIElement::GetScreenPosition()
{
	if (mPositionDirty)
	{
		IntVector2 pos = mPosition;
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
				pos.X() += parent->mPosition.X() + parent->mSize.X() / 2 - current->mSize.X() / 2;
				break;

			case HA_Right:
				pos.X() += parent->mPosition.X() + parent->mSize.X() - current->mSize.X();
				break;
			}

			switch (current->mVerticalAlignment)
			{
			case VA_Top:
				pos.Y() += parent->mPosition.Y();
				break;

			case VA_Center:
				pos.Y() += parent->mPosition.Y() + parent->mSize.Y() / 2 - current->mSize.Y() / 2;
				break;

			case VA_Bottom:
				pos.Y() += parent->mPosition.Y() + parent->mSize.Y() - current->mSize.Y();
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

void UIElement::SetName( const String& name )
{
	mName = name;
}

void UIElement::SetPosition( const IntVector2& position )
{
	if (mPosition != position)
	{
		mPosition = position;
		MarkDirty();
	}
}

void UIElement::SetPosition( int32_t x, int32_t y )
{
	SetPosition(IntVector2(x, y));
}

void UIElement::SetSize( int32_t width, int32_t height )
{
	SetSize(IntVector2(width, height));
}

void UIElement::SetSize( const IntVector2& size )
{
	IntVector2 validateSize;
	validateSize.X() = Math<IntVector2::value_type>::Clamp(size.X(), mMinSize.X(), mMaxSize.X());
	validateSize.Y() = Math<IntVector2::value_type>::Clamp(size.Y(), mMinSize.Y(), mMaxSize.Y());
}

void UIElement::MarkDirty()
{

}

void UIElement::OnMouseCover( const IntVector2& position, const IntVector2& screenPosition, uint32_t buttons, uint32_t qualifiers, Cursor* cursor )
{

}

void UIElement::OnMouseDown( const IntVector2& position, const IntVector2& screenPosition, uint32_t buttons, int qualifiers, Cursor* cursor )
{

}

void UIElement::OnMouseUp( const IntVector2& position, const IntVector2& screenPosition, uint32_t buttons, int qualifiers, Cursor* cursor )
{

}

void UIElement::OnMouseWheel( int32_t delta, uint32_t buttons, uint32_t qualifiers )
{

}

void UIElement::OnDragBegin( const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers, Cursor* cursor )
{

}

void UIElement::OnDragMove( const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers, Cursor* cursor )
{

}

void UIElement::OnDragEnd( const IntVector2& position, const IntVector2& screenPosition, Cursor* cursor )
{

}

void UIElement::OnKeyDown( uint8_t key, uint32_t buttons, uint32_t qualifiers )
{

}

void UIElement::OnKeyUp( uint8_t key, uint32_t buttons, uint32_t qualifiers )
{

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

RcEngine::IntVector2 UIElement::ScreenToClient( const IntVector2& screenPosition )
{
	return screenPosition - GetScreenPosition();
}

RcEngine::IntVector2 UIElement::ClientToScreen( const IntVector2& position )
{
	return position + GetScreenPosition();
}

bool UIElement::IsInside(IntVector2 position, bool isScreen )
{
	if (isScreen)
		position = ScreenToClient(position);
	return position.X() >= 0 && position.Y() >= 0 && position.X() < mSize.X() && position.Y() < mSize.Y();
}

const shared_ptr<Material>& UIElement::GetMaterial() const
{
	
}

void UIElement::GetWorldTransforms( Matrix4f* xform ) const
{

}

uint32_t UIElement::GetWorldTransformsCount() const
{
	return 1;
}


}