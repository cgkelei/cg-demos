#ifndef Theme_h__
#define Theme_h__

#include <Core/Prerequisites.h>
#include <Resource/Resource.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

class _ApiExport Theme : public Resource
{
public:

	/**
     * Struct representing margin, border, and padding areas by
     * the width or height of each side.
     */
    struct SideRegions
    {
        SideRegions() : Top(0), Bottom(0), Left(0), Right(0) {}

        int32_t Top, Bottom, Left, Right;
    };
    
    typedef SideRegions Margin;
    typedef SideRegions Border;
    typedef SideRegions Padding;

	struct ThemeImage 
	{
		String Name;
		IntRect Region;
		ColorRGBA BlendColor;
	};

public:
	Theme();
	~Theme();


protected:

	shared_ptr<Texture> mTexture;

};

}


#endif // Theme_h__
