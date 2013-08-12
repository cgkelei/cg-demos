#ifndef ScrollBar_h__
#define ScrollBar_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport ScrollBar : public UIElement
{
public:
	ScrollBar();
	virtual ~ScrollBar();



protected:

	Button* mForwardButton;
	Button* mBackButton;

	
	
};


}


#endif // ScrollBar_h__
