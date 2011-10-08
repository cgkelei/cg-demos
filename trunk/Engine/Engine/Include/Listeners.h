#pragma once
#ifndef _Listeners__H
#define _Listeners__H

#include "Prerequisites.h"

namespace Engine
{

	/** Struct containing information about a RenderTarget event.
	*/
	struct RenderTargetEvent
	{
		// The source of the event being raised
		RenderTarget* source;
	};

	/** Struct containing information about a RenderTarget Viewport-specific event.
	*/
	struct RenderTargetViewportEvent
	{
		// The source of the event being raised
		Viewport* source;
	};

	class _ApiExport RenderTargetListener
	{
	public:
		virtual ~RenderTargetListener () {}
		
		// Called just before a RenderTarget is about to be rendered into. 
		virtual void PreRenderTargetUpdate (const RenderTargetEvent &evt)
		{ (void)evt; }
		
		// Called just after a RenderTarget has been rendered into. 
		virtual void PostRenderTargetUpdate (const RenderTargetEvent &evt)
		{ (void)evt; }
		
		// Called just before a Viewport on a RenderTarget is to be updated.
		virtual void PreViewportUpdate (const RenderTargetViewportEvent &evt)
		{ (void)evt; }
		
		// Called just after a Viewport on a RenderTarget is to be updated.
		virtual void PostViewportUpdate (const RenderTargetViewportEvent &evt)
		{ (void)evt; }
		
		// Called to notify listener that a Viewport has been added to the target in question.
		virtual void ViewportAdded (const RenderTargetViewportEvent &evt)
		{ (void)evt; }
			
		// Called to notify listener that a Viewport has been removed from the target in question.
		virtual void ViewportRemoved (const RenderTargetViewportEvent &evt)
		{ (void)evt; }

	};
	

	/**
	@Remarks
	Callback class used to send out window events to client app
	*/
	class _ApiExport  WindowEventListener
	{
	public:
		virtual ~WindowEventListener() {}

		/**
		@Remarks
		Window has moved position
		@param rw
		The RenderWindow which created this events
		*/
		virtual void WindowMoved(RenderWindow* rw)
		{ (void)rw; }

		/**
		@Remarks
		Window has resized
		@param rw
		The RenderWindow which created this events
		*/
		virtual void WindowResized(RenderWindow* rw)
		{ (void)rw; }

		/**
		@Remarks
		Window is closing (Only triggered if user pressed the [X] button)
		@param rw
		The RenderWindow which created this events
		@return True will close the window(default).
		*/
		virtual bool WindowClosing(RenderWindow* rw)
		{ (void)rw; return true; }

		/**
		@Remarks
		Window has been closed (Only triggered if user pressed the [X] button)
		@param rw
		The RenderWindow which created this events
		@note
		The window has not actually close yet when this event triggers. It's only closed after
		all windowClosed events are triggered. This allows apps to deinitialise properly if they
		have services that needs the window to exist when deinitialising.
		*/
		virtual void WindowClosed(RenderWindow* rw)
		{ (void)rw; }

		/**
		@Remarks
		Window has lost/gained focus
		@param rw
		The RenderWindow which created this events
		*/
		virtual void WindowFocusChange(RenderWindow* rw)
		{ (void)rw; }
	};


	struct FrameEvent
	{
		/// <summary>
		/// Elapsed time in seconds since the last event.This gives you time between 
		/// frame start & frame end,and between frame end and next frame start.
		/// </summary>
		/// <remarks>
		/// This may not be the elapsed time but the average elapsed time between recently fired events.
		/// </remarks>
		float timeSinceLastEvent;

		/// <summary>
		/// Elapsed time in seconds since the last event of the same type,
		/// i.e. time for a complete frame.
		/// </summary>
		/// <remarks>
		/// This may not be the elapsed time but the average
		/// elapsed time between recently fired events of the same type.
		/// </remarks>
		float timeSinceLastFrame;
	};



	class _ApiExport FrameListener
	{
	public:
		/// <summary>
		/// Called when a frame is about to begin rendering.
		/// </summary>
		/// <remarks> This event happens before any render targets have begun updating. </remarks>
		/// <return>
		/// True to go ahead, false to abort rendering and drop out of the rendering loop.
		/// </return>
		virtual bool FrameStarted(const FrameEvent& evt)
		{ (void)evt; return true; }

		/// <summary>
		///  Called just after a frame has been rendered.
		/// </summary>
		/// <remarks> 
		/// This event happens after all render targets have been fully updated and the buffers switched.
		/// </remarks>
		/// <return>
		/// True to continue with the next frame, false to drop out of the rendering loop.
		/// </return>
		virtual bool FrameEnded(const FrameEvent& evt)
		{ (void)evt; return true; }

	};
}


#endif // _Listeners__H