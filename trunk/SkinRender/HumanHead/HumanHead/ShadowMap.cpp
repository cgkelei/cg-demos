#include "ShadowMap.h"
#include "Utility.h"
#include "RenderTextureFBO.h"

GLuint ShadowMap::msShadowProgramID = 0;
GLint ShadowMap::msWorldParam = -1;
GLint ShadowMap::msViewParam = -1;
GLint ShadowMap::msProjParam = -1;
GLint ShadowMap::msGlowParam = -1;

void ShadowMap::Init()
{
	msShadowProgramID = Utility::LoadShaderEffect("ShadowMap.vert", "ShadowMap.frag");
	
	msWorldParam = glGetUniformLocation(msShadowProgramID, "World");
	ASSERT(msWorldParam >= 0);

	msViewParam = glGetUniformLocation(msShadowProgramID, "View");
	ASSERT(msViewParam >= 0);

	msProjParam = glGetUniformLocation(msShadowProgramID, "Projection");
	ASSERT(msProjParam >= 0);

	msGlowParam = glGetUniformLocation(msShadowProgramID, "Grow");
	//ASSERT(msGlowParam >= 0);
}

void ShadowMap::Release()
{
	glDeleteProgram(msShadowProgramID);
	msShadowProgramID = 0;
}

ShadowMap::ShadowMap( int width, int height )
	: mWidth(width), mHeight(height)
{
	mTexture = new  RenderTexture(width, height, GL_TEXTURE_2D);
	mTexture->InitColor_Tex(0, GL_RGBA32F);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	mTexture->InitDepth_Tex();
}


ShadowMap::~ShadowMap(void)
{
	if (mTexture)
	{
		delete mTexture;
		mTexture = NULL;
	}
}

glm::mat4 gworld;

void ShadowMap::Begin( const glm::mat4& view, const glm::mat4& projection )
{
	mTexture->Activate();

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, mWidth, mHeight);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(msShadowProgramID);
	glUniformMatrix4fv(msViewParam, 1, false, glm::value_ptr(view));
	glUniformMatrix4fv(msProjParam, 1, false, glm::value_ptr(projection));
}

void ShadowMap::SetWorldMatrix( const glm::mat4& world )
{
	gworld = world;
	glUseProgram(msShadowProgramID);
	glUniformMatrix4fv(msWorldParam, 1, false, glm::value_ptr(world));
}

void ShadowMap::End()
{
	mTexture->Deactivate();
	glPopAttrib();
	glUseProgram(0);

	//Utility::SaveTextureToTGA("Depth.tga", mTexture->GetColorTex(), mWidth, mHeight);

	glDisable(GL_CULL_FACE);
}

