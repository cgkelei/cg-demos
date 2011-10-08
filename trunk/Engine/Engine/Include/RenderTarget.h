#pragma  once 

#ifndef _RenderTarget__H
#define _RenderTarget__H

#include "Prerequisites.h"
#include "Viewport.h"
#include "Listeners.h"
#include "PixelFormat.h"

namespace Engine
{
	

/* Define the number of priority groups for the render system's render targets. */
#ifndef OGRE_NUM_RENDERTARGET_GROUPS
	#define OGRE_NUM_RENDERTARGET_GROUPS 10
	#define OGRE_DEFAULT_RT_GROUP 4
	#define OGRE_REND_TO_TEX_RT_GROUP 2
#endif

	class _ApiExport RenderTarget
	{
	
	public:
		
		RenderTarget();
		virtual ~RenderTarget();

		const String& GetName() const							{ return mName; }
		uint32 GetTop() const									{ return mTop; }
		uint32 GetLeft() const									{ return mLeft; }
		uint32 GetWidth() const									{ return mWidth; }
	    uint32 GetHeight() const								{ return mHeight; }
		uint32 GetColourDepth () const							{ return mColourDepth; }
		
		const Viewport& GetViewport() const					    { return *mViewport; }
		Viewport& GetViewport()      						    { return *mViewport; }
		void SetPriority( uint8 priority )						{ mPriority = priority; }
		uint8 GetPriority() const								{ return mPriority; }
		
		virtual void SetActive( bool state )					{ mActive = state; }	
		virtual bool IsActive() const						    { return mActive; }

		/// <summary>
		/// Swaps the frame buffers to display the next frame. 
		/// </summary>
		virtual void SwapBuffers(bool waitForVSync=true);

		virtual bool RequiresTextureFlipping() const =0;
		
		/// <summary>
		/// Add a listener to this RenderTarget which will be called back before & after rendering. 
		/// </summary>
		virtual void AddListener(RenderTargetListener *listener);

		/// <summary>
		/// Removes a RenderTargetListener previously registered using addListener. 
		/// </summary>
		virtual void RemoveListener(RenderTargetListener *listener);
		
		/// <summary>
		/// Removes all listeners from this instance. 
		/// </summary>
		virtual void RemoveAllListeners(void);
					

		virtual void Update (bool swapBuffers=true);

		/// <summary>
		/// Suggests a pixel format to use for extracting the data in this target, 
		/// when calling copyContentsToMemory.
		/// </summary>
		virtual PixelFormat GetPixelFormat() const { return mColorFormat; }

		/// <summary>
		/// Method for manual management of rendering : fires 'preRenderTargetUpdate' and initialises statistics etc. 
		/// </summary>
		virtual void BeginUpdate();

		/// <summary>
		/// Method for manual management of rendering - finishes statistics calculation and fires 'postRenderTargetUpdate'. 
		/// </summary>
		virtual void EndUpdate ();
		
	protected:
		
		/// <summary>
		/// Internal implementation of update()
		/// </summary>
		virtual void UpdateImpl();
	
		/// <summary>
		/// internal method for firing events 
		/// </summary>
		virtual void FirePreUpdate (void);

		/// <summary>
		/// internal method for firing events 
		/// </summary>
		virtual void FirePostUpdate (void);


	protected:
		
		String mName;
		
		uint8   mPriority;
		
		uint32 mLeft, mTop, mWidth, mHeight;

		PixelFormat mColorFormat;
		uint32	mColourDepth;
		
		bool	mIsDepthBuffered;
		uint32  mDepthBits;
		uint32  mStencilBits;

		Viewport* mViewport;

		bool mActive;
		
		// Hardware sRGB gamma conversion done on write?
		bool mHardwareGamma;
		
		// FSAA performed?
		uint32 mMutilSampleCount;
		uint32 mMutilSampleQuality;
		String mMutilSampleHint;
	
#pragma warning (push)
#pragma warning (disable : 4251)
		
		typedef std::vector<RenderTargetListener*> RenderTargetListenerList;
		RenderTargetListenerList mListeners;

#pragma warning (pop)

		
	};
}


#endif // _RenderTarget__H