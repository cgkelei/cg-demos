#pragma once
#ifndef _RenderSystem__H
#define _RenderSystem__H

#include "Prerequisites.h"
#include "RenderSettings.h"
#include "GraphicsCommon.h"
#include "ColorRGBA.h"
#include "Matrix.h"

namespace Engine
{
#define MAX_LIGHTS 8

	typedef std::map< String, RenderTarget* >		RenderTargetMap;
	typedef RenderTargetMap::iterator				RenderTargetMapIter;
	typedef std::multimap< uint8, RenderTarget* >	RenderTargetPriorityMap;

	class _ApiExport RenderSystem 
	{
	public:
		RenderSystem(void);
		virtual ~RenderSystem(void);

		
		virtual const String& GetName() const = 0;

		virtual void BeignFrame() = 0;
		virtual void EndFrame() = 0;

		virtual RenderWindow* CreateRenderWindow(const String &name, const RenderSettings& settings) = 0;

		/// <summary>
		// Destroys a render window
		/// </summary>
		virtual void DestroyRenderWindow(const String& name);


		/// <summary>
		// Returns a pointer to the render target with the passed name
		// If that render target cannot be found, return NULL
		/// </summary> 
		virtual RenderTarget* GetRenderTarget (const String &name);
		
		/// <summary>
		//  Attaches the passed render target to the render system.
		/// </summary> 
		virtual void AttachRenderTarget( RenderTarget &target );

		/// <summary>
		//  Detaches the render target with the passed name from the render system and 
		//  returns a pointer to it.  If the render target not found, return NULL
		/// </summary> 
		virtual RenderTarget* DetachRenderTarget (const String &name);

		/// <summary>
		// Destroys a render target
		/// </summary>
		virtual void DestroyRenderTarget(const String& name);
		
		/// <summary>
		/// fullscreen render windows wait for the vertical blank before flipping buffers. 
		/// </summary>
		void SetWaitForVerticalBlank (bool enabled);
		
		/// <summary>
		/// Returns true if the system is synchronising frames with the monitor vertical blank. 
		/// </summary>
		bool GetWaitForVerticalBlank (void) const;
			
			
		/// <summary>
		// Sets the provided viewport as the active one for future rendering operations. 
		/// </summary> 
		virtual void SetViewport (Viewport *vp) = 0;
		
		/// <summary>
		// Get the current active viewport for rendering.
		/// </summary> 	
		virtual Viewport* GetViewport();
	
		/// <summary>
		// Sets the view transform matrix. 
		/// </summary> 	
		virtual void SetViewMatrix (const Matrix4f& m) = 0;

		/// <summary>
		// Sets the world transform matrix. 
		/// </summary> 	
		virtual void SetWorldMatrix (const Matrix4f& m) = 0;
			
		/// <summary>
		// Sets multiple world matrices (vertex blending).  
		/// </summary> 	
		virtual void SetWorldMatrices (const Matrix4f* m, unsigned short count);
			

		/// <summary>
		// Sets the projection transform matrix. 
		/// </summary> 	
		virtual void SetProjectionMatrix (const Matrix4f& m) = 0;
			
			
		virtual RenderWindow* Initialize(bool autoCreateWindow, const String& windowTitle = "Render Window");
		

		/// <summary>
		// Restart the renderer (normally following a change in settings). 
		/// </summary>
		virtual void Reinitialize (void) = 0;
			
		
		virtual void Shutdown();
		
		/// <summary>
		// Get the render state manager of  acticve device
		/// </summary>
		virtual RenderStateManager* GetRenderStateManager() = 0;
		
		/// <summary>
		// Sets the type of light shading required (default = Gouraud).
		/// </summary>
		virtual void SetShadingType(ShadingType st) = 0;
		
		/// <summary>
		// Sets the colour & strength of the ambient (global directionless) light in the world.
		/// </summary>
		virtual void SetAmbientLight(float r, float g, float b) = 0;
		

		// The RenderSystem will keep a count of tris rendered, this resets the count.
		void BeginGeometryCount(void) { mBatchCount = mFaceCount = mVertexCount = 0; }

		/// <summary>
		// Reports the number of vertices passed to the renderer since the last BeginGeometryCount call. 
		/// </summary>
		uint32 GetFaceCount () const  { return mFaceCount; }
		uint32 GetBatchCount() const { return mBatchCount; }
		uint32 GetVertexCount () const { return mVertexCount; }
		

		/** Sets the current vertex declaration, ie the source of vertex data. */
		virtual void SetVertexDeclaration(VertexDeclaration* decl) = 0;

		/** Sets the current vertex buffer binding state. */
		//virtual void SetVertexBufferBinding(VertexBufferBinding* binding) = 0;


		virtual void Clear(uint32 buffers = CO_Target | CO_DepthBuffer, const ColorRGBA& colour = ColorRGBA::Black, 
			float depth = 1.0f, uint16 stencil = 0) = 0;


		virtual void UpdateAllRenderTargets(bool swapBuffers = true);
			
	protected:
		
#pragma warning (push)
#pragma warning (disable : 4251)

		RenderSettings mRenderSettings;

		/** The render targets. */
		RenderTargetMap mRenderTargets;

		/** The render targets, ordered by priority. */
		RenderTargetPriorityMap mPrioritisedRenderTargets;

		/** The Active render target. */
		RenderTarget * mActiveRenderTarget;

		// Active viewport (dest for future rendering operations)
		Viewport* mActiveViewport;

		// List of names of events this rendersystem may raise
		StringVector mEventNames;

#pragma warning (pop)

		bool	mVSync;
		uint32	mVSyncInterval;
		bool	mWBuffer;
		
		uint32	mBatchCount;
		uint32	mFaceCount;
		uint32	mVertexCount;
		
		CullingMode mCullMode;

	};
}




#endif