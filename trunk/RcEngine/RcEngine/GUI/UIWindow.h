#ifndef UIWindow_h__
#define UIWindow_h__

#include <Core/Prerequisites.h>
#include <GUI/UIElement.h>

namespace RcEngine {


class _ApiExport UIWindow : public UIElement
{
public:
	UIWindow();
	~UIWindow();

	void OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers);
	void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers);
	void OnDragEnd(const IntVector2& position, const IntVector2& screenPosition);

protected:

	bool mMinimized, mMaximized;

};


}

#endif // UIWindow_h__
