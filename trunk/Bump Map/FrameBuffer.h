#pragma once
#include "Renderer.h"
#include "Texture.h"

class FrameBuffer
{
public:

	enum FormatType
	{
		FT_FORMAT_NONE,
		FT_FORMAT_RGB,
		FT_FORMAT_RGBA,
		FT_FORMAT_DEPTH
	};

	enum DepthType
	{
		DT_DEPTH_NONE,
		DT_DEPTH_16,
		DT_DEPTH_24,
		DT_DEPTH_32
	};

	enum StencilType
	{
		ST_STENCIL_NONE,
		ST_STENCIL_8
	};

	enum BufferingType
	{
		BT_BUFFERED_SINGLE,
		BT_BUFFERED_DOUBLE
	};

	enum MultisamplingType
	{
		MT_SAMPLING_NONE,
		MT_SAMPLING_2,
		MT_SAMPLING_4
	};

public:
	FrameBuffer(void);
	FrameBuffer (FormatType eFormat, DepthType eDepth, StencilType eStencil,
					BufferingType eBuffering, MultisamplingType eSampling,
						Renderer* pkRenderer, Texture* pkTarget);
	~FrameBuffer(void);


	virtual FrameBuffer* Create (FormatType eFormat, DepthType eDepth, StencilType eStencil,
						BufferingType eBuffering, MultisamplingType eMultisampling, 
							Renderer* pkRenderer, Texture* pkTarget) = 0;
	virtual void Destroy (FrameBuffer* pkBuffer) = 0;

	
	virtual bool initializeBuffer();

	// Member access.
	FormatType getFormatType () const			{ return m_formatType; }
	DepthType getDepthType () const				{ return m_depthType; }  
	StencilType getStencilType () const			{ return m_stencilType; }
	BufferingType getBufferingType () const		{ return m_bufferingType;}
	MultisamplingType getMultisamplingType () const { return m_multisamplingType; }
	Renderer* GetRenderer ()					{ return m_renderer; }
	Texture* GetTarget ()						{ return m_target; }	

	virtual void enable () = 0;
	virtual void disable () = 0;


protected:
	FormatType				m_formatType;
	DepthType				m_depthType;
	StencilType				m_stencilType;
	BufferingType			m_bufferingType;
	MultisamplingType		m_multisamplingType;
	Renderer*				m_renderer;
	Texture*				m_target;

protected: 
	uint32					m_frameBufferID;
	uint32					m_depthBufferID;
	uint32					m_stencilBufferID;
	uint32					m_targetID;
	bool					m_bEnabled;
};
