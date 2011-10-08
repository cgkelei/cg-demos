#include "RenderWindow.h"

namespace Engine
{


	RenderWindow::RenderWindow()
	{

	}


	RenderWindow::~RenderWindow()
	{

	}

	void RenderWindow::WindowMovedOrResized()
	{

	}


	bool RenderWindow::IsActive( void ) const
	{
		return mActive && IsVisible();
	}


	void RenderWindow::GetMetrics( uint32& width, uint32& height, uint32& colourDepth, int& left, int& top )
	{
		width = mWidth;
		height = mHeight;
		colourDepth = mColourDepth;
		left = mLeft;
		top = mTop;
	}

	void RenderWindow::SetActive( bool state )
	{
		mActive = state;
	}


}