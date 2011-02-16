#ifndef _Viewport__H
#define _ViewPort__H

class Viewport
{

public:
	Viewport() : m_xPos(0), m_yPos(0), m_width(0), m_height(0)  { }

public:
	int		m_xPos;
	int		m_yPos;
	int		m_width;
	int		m_height;
};

#endif