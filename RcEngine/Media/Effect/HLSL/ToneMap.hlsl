Texture2D HDRBuffer;

float4 ToneMapPSMain(in float4 iFragCoord : SV_Position) : SV_Target0
{
	int3 samplePos = int3(iFragCoord.xy, 0);
	return HDRBuffer.Load(samplePos);
}