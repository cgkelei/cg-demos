
float LinearizeDepth(in float nonLinearDepth, in vec2 nearFar)
{	
	return nearFar.x * nearFar.y / (nearFar.y - (nearFar.y - nearFar.x) * nonLinearDepth);
}
