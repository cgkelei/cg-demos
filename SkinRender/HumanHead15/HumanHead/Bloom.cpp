#include "Bloom.h"
#include "Utility.h"
#include "RenderTextureFBO.h"

FastBloom::FastBloom(void)
{
	
}

FastBloom::~FastBloom(void)
{
}

void FastBloom::Go( RenderTexture* input )
{

}

//void FastBloom::Go( RenderTexture* input, RenderTexture* bloom, RenderTexture* temp )
//{
//	temp->Activate();
//
//	glUseProgram(mBlurProgramID);
//	glActiveTexture(GL_TEXTURE0);
//	glEnable(GL_TEXTURE_2D);
//	input->Bind();
//	glUniform1i(mBlurSrcTexParam, 0);
//	
//	// blur twice
//	glUniform1f(mGaussWidthParam, sqrtf(0.008));
//	DrawQuad(temp->GetWidth(), temp->GetHeight());
//
//	temp->Deactivate();
//
//	bloom[0]->
//}
