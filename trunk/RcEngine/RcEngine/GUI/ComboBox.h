#ifndef ComboBox_h__
#define ComboBox_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport ComboBox : public UIElement
{
public:
	/**
	 * ComboBox selected item changed event
	 */
	typedef fastdelegate::FastDelegate1<bool> SelectionChangedEventHandler;
	SelectionChangedEventHandler EventSelectionChanged;

public:
	ComboBox();
	virtual ~ComboBox();

	void AddItem(const String& text);
	void InsertItem(int32_t index, const String& text);

	void SetSelectedIndex(int32_t index);
	int32_t GetSelectedIndex() const;

protected:
	std::list<String> mItems;

};


}

#endif // ComboBox_h__