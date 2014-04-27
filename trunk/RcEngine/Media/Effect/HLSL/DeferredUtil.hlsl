float LinearizeDepth(in float nonLinearDepth, in float2 nearFar)
{	
	return nearFar.x * nearFar.y / (nearFar.y - (nearFar.y - nearFar.x) * nonLinearDepth);
}
	
float3 PositionVSFromDepth(in float3 viewRay, in float linearDepth)
{
    // Clamp the view space position to the plane at Z = 1
	return viewRay * (linearDepth / viewRay.z);
}