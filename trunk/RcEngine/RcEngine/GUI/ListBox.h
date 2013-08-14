#ifndef ListBox_h__
#define ListBox_h__

#include <GUI/UIElement.h>

namespace RcEngine {

struct _ApiExport ListBoxItem 
{
	String Text;
	Rectanglef Region;
	bool Selected;
};

class _ApiExport ListBox : public UIElement
{
public:
	ListBox();
	virtual ~ListBox();

	virtual void OnResize();
	virtual void Initialize(const GuiSkin::StyleMap* styles /* = nullptr */);

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	virtual void UpdateRect();

	void AddItem(const String& text);
	void InsertItem(int32_t index, const String& text);
	void RemoveItem(int32_t index);
	void RemoveAllItems();

	ScrollBar* GetVertScrollBar() const;
	ScrollBar* GetHorzScrollBar() const;

protected:

	std::vector<ListBoxItem*> mItems;

	ScrollBar* mHorzScrollBar;
	ScrollBar* mVertScrollBar;

	int32_t mSelectedIndex;

	int32_t mNumVisibleItems;

	GuiSkin* mBackgroundStyle;
};


}


#endif // ListBox_h__
