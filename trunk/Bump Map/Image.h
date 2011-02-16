#ifndef _Image__H
#define _Image__H

class Image
{
public:
	Image() {};
	virtual ~Image()  {};

protected:
	int m_width;
	int m_height;
	int m_depth;
};

#endif
