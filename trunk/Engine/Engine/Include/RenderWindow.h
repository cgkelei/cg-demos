#pragma once
#ifndef _RenderWindow__H
#define _RenderWindow__H

#include "Prerequisites.h"
#include "RenderTarget.h"

namespace Engine
{
	class _ApiExport RenderWindow : public RenderTarget
	{
	public:
		RenderWindow();
		virtual ~RenderWindow();

		virtual void Initialize() = 0;
		virtual void Destroy(void) = 0;
		virtual bool IsClosed(void) const = 0;
		
		virtual void Resize(uint32 width, uint32 height) = 0;
		virtual void Reposition(uint32 left, uint32 top) = 0;
		
		// Alter full screen mode options. 
		virtual void SetFullscreen(bool fullScreen) = 0;
		virtual bool IsFullscreen() const { return mIsFullScreen; };

		virtual bool IsVisible(void) const { return true; }

		virtual void SetActive(bool state);
		virtual bool IsActive(void) const;
		
		// Notify that the window has been resized. 
		virtual void WindowMovedOrResized();

		// Override since windows don't usually have alpha
		virtual PixelFormat GetPixelFormat() const { return mColorFormat; }
	

		/** Overloaded version of getMetrics from RenderTarget, including extra details
		specific to windowing systems.
		*/
		virtual void GetMetrics(uint32& width, uint32& height, uint32& colourDepth, 
			int32& left, int32& top);

		/** Returns true if the window will automatically de-activate itself when it loses focus.
		*/
		bool IsDeactivatedOnFocusChange() const { return mAutoDeactivatedOnFocusChange; }

		/** Indicates whether the window will automatically deactivate itself when it loses focus.
		* \param deactivate a value of 'true' will cause the window to deactivate itself when it loses focus.  'false' will allow it to continue to render even when window focus is lost.
		* \note 'true' is the default behavior.
		*/
		void SetDeactivateOnFocusChange(bool deactivate) { mAutoDeactivatedOnFocusChange = deactivate; }



	protected:
		
		bool mIsFullScreen;
		bool mAutoDeactivatedOnFocusChange;
	};
}




#endif // _RenderWindow__H
