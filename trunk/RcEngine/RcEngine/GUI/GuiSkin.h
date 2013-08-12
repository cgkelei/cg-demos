#ifndef GuiSkin_h__
#define GuiSkin_h__

#include <Core/Prerequisites.h>
#include <Resource/Resource.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

enum Alignment
{
	AlignLeft     = 1UL << 0,
	AlignRight    = 1UL << 1,
	AlignHCenter  = 1UL << 2,

	AlignTop      = 1UL << 3,
	AlignBottom   = 1UL << 4,
	AlignVCenter  = 1UL << 5,

	AlignCenter   = AlignHCenter | AlignVCenter
};

enum UIElementState
{
	UI_State_Normal = 0,
	UI_State_Disable,
	UI_State_Hidden,
	UI_State_Focus,
	UI_State_Hover,
	UI_State_Pressed,
	UI_State_Count
};

/**
 * GuiSkin is a collection of GUIStyles that can be applied to your GUI Controls.
 */
class _ApiExport GuiSkin /*: public Resource*/
{
public:

	struct SytleImage
	{
		IntRect   TexRegion;  
		ColorRGBA TexColor;
	};

	struct GuiStyle
	{
		SytleImage StyleStates[UI_State_Count];

		bool WordWrap;
		Alignment TextAlign;
	};

public:
	//GuiSkin(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	GuiSkin();
	~GuiSkin();

protected:
	void LoadImpl();
	void UnloadImpl();

public:
	//static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);


public:
	
	shared_ptr<Font> mFont;
	int32_t mFontSize;

	shared_ptr<Texture> mSkinTexAtlas; 

	ColorRGBA BackColor;
	ColorRGBA ForeColor;

	// Button
	GuiStyle Button;

	// Check box
	SytleImage CheckBox[2];

	// Slider
	SytleImage HSliderTrack, HSliderThumb;
	SytleImage VSliderTrack, VSliderThumb;
};

}


#endif // GuiSkin_h__
