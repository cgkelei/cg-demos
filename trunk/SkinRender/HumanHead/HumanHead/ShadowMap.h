#pragma once

#include "Common.h"

class RenderTexture;
class Camera;

class ShadowMap
{
public:
	ShadowMap(int width, int height);
	~ShadowMap(void);

	void Begin(const Camera* camera);
	void End();

	RenderTexture* GetRenderTexture() const { return mTexture; }

private:
	RenderTexture* mTexture;
	int mWidth, mHeight;

public:
	static void Init();
	static void Release();
	static void SetWorldMatrix(const glm::mat4& world);

private:
	static GLuint msShadowProgramID;
	static GLint msWorldParam;
	static GLint msViewParam;
	static GLint msProjParam;
	static GLint msGlowParam;
	static GLint msZNearParam;
	static GLint msZFarParam;
};
