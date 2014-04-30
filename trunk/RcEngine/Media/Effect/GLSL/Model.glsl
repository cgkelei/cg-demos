[[Vertex=VSMain]]

// Shader uniforms	
uniform mat4 WorldView;	
uniform mat4 Proj;

#ifdef _Skinning
	#define MaxNumBone 92
	uniform mat4 SkinMatrices[MaxNumBone];
#endif	

// VS Inputs
in vec3 iPos;

#ifdef _Skinning
	in vec4 iBlendWeights;
	in uvec4 iBlendIndices;
#endif

in vec3 iNormal;
in vec2 iTex;

#ifdef _NormalMap
	in vec3 iTangent;
	in vec3 iBinormal;
#endif

// VS Outputs
out vec4 oPosVS;
out vec2 oTex;

#ifdef _NormalMap
	out mat3 oTangentToView;
#else
	out vec3 oNormalVS;
#endif

void main()
{
#ifdef _Skinning
	mat4 SkinMatrix = SkinMatrices[iBlendIndices[0]] * iBlendWeights[0] +
					  SkinMatrices[iBlendIndices[1]] * iBlendWeights[1] +
					  SkinMatrices[iBlendIndices[2]] * iBlendWeights[2] +
					  SkinMatrices[iBlendIndices[3]] * iBlendWeights[3];
#endif
	
	// calculate position in view space:
#ifdef _Skinning
	mat4 SkinWorldView = SkinMatrix * WorldView;
	oPosVS = vec4(iPos, 1.0)* SkinWorldView;
#else
	oPosVS = vec4(iPos, 1.0) * WorldView;
#endif

	// calculate view space normal.
#ifdef _Skinning
	vec3 normal = normalize(iNormal * mat3(SkinWorldView));
#else
	vec3 normal = normalize(iNormal * mat3(WorldView));
#endif

	// calculate tangent and binormal.
#ifdef _NormalMap
	#ifdef _Skinning
		vec3 tangent = normalize(iTangent * mat3(SkinWorldView));
		vec3 binormal = normalize(iBinormal * mat3(SkinWorldView));
	#else
		vec3 tangent = normalize(iTangent * mat3(WorldView));
		vec3 binormal = normalize(iBinormal * mat3(WorldView));
	#endif

	// actualy this is a world to tangent matrix, because we always use V * Mat.
	oTangentToView = mat3( tangent, binormal, normal);

	// transpose to get tangent to world matrix
	oTangentToView = transpose(oTangentToView);
#else
	oNormalVS = normal;
#endif
	
	// texcoord
	oTex = vec2(iTex.x, 1.0 - iTex.y);
	gl_Position = oPosVS * Proj;
}


[[Fragment=GBufferPSMain]]

uniform sampler2D BestNormalFitMap;

// PS Inputs
in vec4 oPosVS;
in vec2 oTex;

#ifdef _NormalMap
	in mat3 oTangentToView;
#else
	in vec3 oNormalVS;
#endif

// PS Outputs
out vec4 FragColor0; // Normal + shininess
out vec4 FragColor1; // Diffuse + Specular
	
// Best normal fit	
void CompressUnsignedNormalToNormalsBuffer(inout vec3 vNormal)
{
  // renormalize (needed if any blending or interpolation happened before)
  vNormal.rgb = normalize(vNormal.rgb);
  // get unsigned normal for cubemap lookup (note the full float precision is required)
  vec3 vNormalUns = abs(vNormal.rgb);
  // get the main axis for cubemap lookup
  float maxNAbs = max(vNormalUns.z, max(vNormalUns.x, vNormalUns.y));
  // get texture coordinates in a collapsed cubemap
  vec2 vTexCoord = vNormalUns.z<maxNAbs?(vNormalUns.y<maxNAbs?vNormalUns.yz:vNormalUns.xz):vNormalUns.xy;
  vTexCoord = vTexCoord.x < vTexCoord.y ? vTexCoord.yx : vTexCoord.xy;
  vTexCoord.y /= vTexCoord.x;
  
#ifndef Direct3D	
	vTexCoord.y = 1.0 - vTexCoord.y;
#endif
  
  // fit normal into the edge of unit cube
  vNormal.rgb /= maxNAbs;
  // look-up fitting length and scale the normal to get the best fit
  float fFittingScale = texture2D(BestNormalFitMap, vTexCoord).a;
  // scale the normal to get the best fit
  vNormal.rgb *= fFittingScale;
  // squeeze back to unsigned
  vNormal.rgb = vNormal.rgb * .5 + .5;
}

 float Luminance(in vec3 color)
 {
	return dot(color, vec3(0.2126, 0.7152, 0.0722));
 }

void main() 
{	
	// diffuse material
#ifdef _DiffuseMap
	vec4 diffuseTap = texture2D(DiffuseMap, oTex);
	vec3 albedo = diffuseTap.rgb;
	#ifdef _AlphaTest
		if( diffuseTap.a < 0.01 ) discard;
	#endif	
#else
	vec3 albedo = DiffuseColor;
#endif

	// specular material
#ifdef _SpecularMap
	vec4 specularTap = texture2D(SpecularMap, oTex);
	//vec3 specular = specularTap.rgb;
	vec3 specular = specularTap.rrr;
	#ifdef _GlossyMap
		float shininess = specularTap.a;
	#else 
		float shininess = Shininess / 255.0;
	#endif
#else
	vec3 specular = SpecularColor;
	float shininess = Shininess / 255.0;
#endif
	
	// normal map
#ifdef _NormalMap
	vec3 normal = texture2D( NormalMap, oTex ).rgb * 2.0 - 1.0;
	normal = normalize(normal * oTangentToView);
#else
	vec3 normal = normalize(oNormalVS);
#endif	
	
	normal = normal * 0.5 + 0.5;
	//CompressUnsignedNormalToNormalsBuffer(normal);	
	
	FragColor0 = vec4(normal.xyz, shininess);
	FragColor1 = vec4(albedo.rgb, Luminance(specular));	 // Specular luminance
}
