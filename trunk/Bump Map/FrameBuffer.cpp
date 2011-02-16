#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(void)
{
}

FrameBuffer::FrameBuffer( FormatType format, DepthType depth, StencilType stencil, 
							BufferingType buffering, MultisamplingType sampling, 
								Renderer* renderer, Texture* target )
{
	m_formatType = format;
	m_target = target;
	m_depthType = depth;
	m_multisamplingType = sampling;
	m_stencilType = stencil;
	m_renderer = renderer;
	m_bufferingType = buffering;
}


FrameBuffer::~FrameBuffer(void)
{
}

bool FrameBuffer::initializeBuffer()
{
	/*glBindTexture(GL_TEXTURE_2D, m_targetID);

	// Attach the texture to the framebuffer.
	glGenFramebuffersEXT(1, &m_frameBufferID);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_frameBufferID);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
		GL_TEXTURE_2D, m_targetID, 0);*/
	return false;
}
