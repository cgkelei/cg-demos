
Texture2D GBuffer0;
Texture2D GBuffer1;
Texture2D<float> DepthBuffer;

RWTexture2D oBuffer;


// threads per group
#define WORK_GROUP_SIZE  16

[numthreads(WORK_GROUP_SIZE, WORK_GROUP_SIZE, 1)]
void TiledDeferrdCSMain(
	uint3 )
{


}