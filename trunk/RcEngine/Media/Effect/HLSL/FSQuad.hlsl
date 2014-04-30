void VSMain(in float2 iPos : POSITION,
			in float2 iTex : TEXCOORD0,
			out float2 oTex : TEXCOORD0,
			out float4 oPosCS : SV_POSITION)
{
	oTex = iTex;
	oPosCS = float4(iPos, 0.0, 1.0);
}

Texture2D ColorMap;
SamplerState LinearSampler;

float4 PSMain(in float2 iTex : TEXCOORD0) : SV_Target0
{
	float4 final = ColorMap.Sample(LinearSampler, iTex);
	return final;
}