#ifndef _MouseHandler__H
#define _MouseHandler__H

#include <Windows.h>

#define KEY_LMOUSE 256
#define KEY_MMOUSE 257
#define KEY_RMOUSE 258

class MouseHandler
{
public:
	MouseHandler(void) : m_moveX(0), m_moveY(0), m_wheelScroll(0.0f) {};
	
	static MouseHandler* Instance();

public:
	void handleMessage(UINT message, WPARAM wParam, LPARAM lParam);
	bool isButtonPressed(UINT button) const;
	bool isButtonHolding(UINT button) const;
	void getMouseMove(int& xPos, int& yPos) const { xPos = m_moveX; yPos = m_moveY; }
	float getWheelScroll() const { return m_wheelScroll; }

public:
	void OnFrameEnd();

protected:
	void setButtonPressed(UINT button);
	void setButtonReleased(UINT button);
	UINT getButtonIndex(UINT button) const;

protected:
	float m_wheelScroll;
	int m_moveX;
	int m_moveY;
	
	bool m_bButtonPressed[3];
	bool m_bButtonHolding[3];
};

#endif
