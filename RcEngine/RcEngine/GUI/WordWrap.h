#ifndef WordWrap_h__
#define WordWrap_h__

#include <Core/Prerequisites.h>
#include <Math/Rectangle.h>

namespace RcEngine {

class _ApiExport WordWrap 
{
public:
	static WordWrap* WrapText(const std::wstring& text, const IntRect& region, Font& font, uint32_t fontSize);
	std::wstring GetWrappedText() const;

private:
	WordWrap(const std::wstring& text, const IntRect& region);
	
private:
	std::vector<int32_t> mBreakPositions;
	std::vector<float> mLineWidth;
	std::wstring mText;
	IntRect mRect;
};




}


#endif // WordWrap_h__
