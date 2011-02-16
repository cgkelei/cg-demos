#include "MouseHandler.h"

void MouseHandler::handleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	m_bButtonHolding[getButtonIndex(KEY_LMOUSE)] = isButtonHolding(KEY_LMOUSE);
	m_bButtonHolding[getButtonIndex(KEY_RMOUSE)] = isButtonHolding(KEY_RMOUSE);

	switch (message)
	{
	case WM_LBUTTONDOWN:
		setButtonPressed(KEY_LMOUSE);
		break;

	case WM_LBUTTONUP:
		setButtonReleased(KEY_LMOUSE);
		break;

	case WM_RBUTTONDOWN:
		setButtonPressed(KEY_RMOUSE);
		break;

	case WM_RBUTTONUP:
		setButtonReleased(KEY_RMOUSE);
		break;
	
	case WM_MOUSEMOVE:
		{
			static bool bFirst = true;
			static int prevX;
			static int prevY;

			const int currX = (short) LOWORD(lParam);
			const int currY = (short) HIWORD(lParam);

			if (bFirst)
			{
				bFirst = false;
			}
			else
			{
				m_moveX = prevX - currX;
				m_moveY = prevY - currY;
			}

			prevX = currX;
			prevY = currY;
		}
		break;

	case WM_MOUSEWHEEL:
		{
			int mouseMovement = (short) HIWORD(wParam);
			m_wheelScroll = ((float) mouseMovement) / 300.0f;
		}
		break;

	default:
		break;
	}

}

void MouseHandler::setButtonPressed(UINT button)
{
	if(button >= KEY_LMOUSE && button <= KEY_RMOUSE)
		m_bButtonPressed[getButtonIndex(button)] =  true;
}

void MouseHandler::setButtonReleased(UINT button)
{
	if(button >= KEY_LMOUSE && button <= KEY_RMOUSE)
	{
		m_bButtonPressed[getButtonIndex(button)] =  false;
		m_bButtonHolding[getButtonIndex(button)] =  false;
	}
}

bool MouseHandler::isButtonPressed(UINT button) const 
{
	if(button >= KEY_LMOUSE && button <= KEY_RMOUSE)
		return m_bButtonPressed[button];
	return false;
}

bool MouseHandler::isButtonHolding(UINT button) const 
{
	if(button >= KEY_LMOUSE && button <= KEY_RMOUSE)
		return m_bButtonHolding[button];
	return false;
}


UINT MouseHandler::getButtonIndex(UINT button) const 
{
	return button - KEY_LMOUSE;
}


void MouseHandler::OnFrameEnd()
{
	m_wheelScroll = 0.0f;
	m_moveX = 0;
	m_moveY = 0;
}

MouseHandler* MouseHandler::Instance()
{
	static MouseHandler mouse;
	return &mouse;
}
