#pragma once

#include "Common.h"

class RenderTexture;

class ShadowMap
{
public:
	ShadowMap(int width, int height);
	~ShadowMap(void);

	void Begin(const glm::mat4& view, const glm::mat4& projection);
	void End();

	RenderTexture* GetTexture() const { return mTexture; }

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
};

