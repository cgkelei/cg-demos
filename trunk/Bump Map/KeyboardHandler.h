#ifndef _KeyboardHandler__H
#define _KeyboardHandler__H

#include <Windows.h>

class KeyboardHandler
{
public:
	KeyboardHandler(void);
	~KeyboardHandler(void);

	static KeyboardHandler* Instance();

public:
	void handleMessage(UINT message, WPARAM wParam, LPARAM lParam);
	bool isKeyPressed(WPARAM wKeyNumber) const;

protected:
	void setKeyPressed(WPARAM wKeyNumber);
	void setKeyReleased(WPARAM wKeyNumber);

private:
	bool m_bKeyPressed[256];
};


#endif