#include "Renderer.h"
#include "Camera.h"
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

Renderer::Renderer(HWND hwnd, int32 width, int height)
	:m_hwnd(hwnd), m_height(height), m_width(width)
{
	initDevice();
	InitialOpenGL();
	
}

Renderer::~Renderer(void)
{

}


void Renderer::initDevice()
{
	m_hdc = GetDC(m_hwnd);

	PIXELFORMATDESCRIPTOR pfd; 
	int pixelformat; 

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			
	pfd.nVersion = 1;									
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
	pfd.dwLayerMask = PFD_MAIN_PLANE;					
	pfd.iPixelType = PFD_TYPE_RGBA;						
	pfd.cColorBits = 32;					
	pfd.cDepthBits = 16;					
	pfd.cAccumBits = 0;									
	pfd.cStencilBits = 0;								

	pixelformat = ChoosePixelFormat(m_hdc, &pfd);
	SetPixelFormat(m_hdc, pixelformat, &pfd);
	DescribePixelFormat(m_hdc, pixelformat, sizeof(pfd), &pfd);
	m_hrc = wglCreateContext(m_hdc);
	wglMakeCurrent(m_hdc, m_hrc);
}	


void Renderer::OnSize(const int width, const int height)
{
	m_height = height; 
	m_width = width;
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();									

	gluPerspective(45.0f,(GLfloat)m_width/(GLfloat)m_height,1 ,150.0f);

	glMatrixMode(GL_MODELVIEW);							
	glLoadIdentity();

}

void Renderer::InitialOpenGL()
{
	glViewport(0, 0, m_width, m_height);
	
	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();									


	gluPerspective(45.0f,(GLfloat)m_width/(GLfloat)m_height,1 ,150.0f);

	glMatrixMode(GL_MODELVIEW);							
	glLoadIdentity();	

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

}

void Renderer::beginFrame()
{
	ClearBackBuffer();
	ClearDepthBuffer();

	glMatrixMode(GL_MODELVIEW);							
	glLoadIdentity();

	m_camera->positionCamera(0, 0, 5, 0, 0, 0, 0, 1,0);

	gluLookAt(0,0,5, 0,0,0, 0,1,0);
}

void Renderer::drawFrame()
{
	glBegin (GL_TRIANGLES);								
		glColor3ub(255, 0, 0);							
		glVertex3f(0, 1, 0);							

		glColor3ub(0, 255, 0);							
		glVertex3f(-1, 0, 0);							

		glColor3ub(0, 0, 255);							
		glVertex3f(1, 0, 0);							
	glEnd();

}

void Renderer::endFrame()
{
	glFlush();
	SwapBuffer();
}

void Renderer::ClearBackBuffer()
{
	glClearColor(m_clearColor[0], m_clearColor[1] ,m_clearColor[2],
		m_clearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::ClearDepthBuffer()
{
	glClearDepth((double)m_clearDepth);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Renderer::ClearStencilBuffer()
{
	glClearStencil((GLint)m_clearStencil);
	glClear(GL_STENCIL_BUFFER_BIT);
}

void Renderer::SwapBuffer()
{
	::SwapBuffers(m_hdc);
}

void Renderer::SetViewport(int X, int Y, int width, int height)
{
	m_viewport.m_xPos = X;
	m_viewport.m_yPos = Y;
	m_viewport.m_width = width;
	m_viewport.m_height = height;

	glViewport(m_viewport.m_xPos, m_viewport.m_yPos, 
					m_viewport.m_width, m_viewport.m_height);

}