#ifndef Bloom_h__
#define Bloom_h__

#include "Common.h"

class RenderTexture;

class FastBloom
{
public:
	FastBloom(void);
	~FastBloom(void);

	void Go(RenderTexture* input);

private:
	GLuint mBlurProgramID, mCombineProgramID;
	GLint mGaussWidthParam, mBlurSrcTexParam, mStepParam;
	GLint mInputTexParam, mBloomTexcParam;

	RenderTexture* mBloomTex[2];
};

#endif // Bloom_h__