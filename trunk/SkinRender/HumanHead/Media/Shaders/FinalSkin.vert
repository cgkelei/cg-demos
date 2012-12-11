 #version 330

#define LIGHTCOUNT 3

 uniform sampler2D Irrad0Tex, Irrad1Tex, Irrad2Tex, Irrad3Tex, Irrad4Tex, Irrad5Tex;
 uniform sampler2D


 
 float4 finalSkinShader(float3 position : POSITION,  
  float2 texCoord : TEXCOORD0,  
  float3 normal : TEXCOORD1,  
  // Shadow map coords for the modified translucent shadow map  
   float4 TSM_coord : TEXCOORD2,  
  // Blurred irradiance textures  
   uniform texobj2D irrad1Tex,  
  . . .  
  uniform texobj2D irrad6Tex,  
  // RGB Gaussian weights that define skin profiles  
   uniform float3 gauss1w,  
  . . .  
  uniform float3 gauss6w,  
  uniform float mix, // Determines pre-/post-scatter texturing  
   uniform texobj2D TSMTex,  
  uniform texobj2D rhodTex )  
{  
  // The total diffuse light exiting the surface  
   float3 diffuseLight = 0;  
  float4 irrad1tap = f4tex2D( irrad1Tex, texCoord );  
  . . .  
  float4 irrad6tap = f4tex2D( irrad6Tex, texCoord );  
  diffuseLight += gauss1w * irrad1tap.xyz;  
  . . .  
  diffuseLight += gauss6w * irrad6tap.xyz;  
  // Renormalize diffusion profiles to white  
   float3 normConst = gauss1w + guass2w + . . . + gauss6w;  
  diffuseLight /= normConst; // Renormalize to white diffuse light  
   // Compute global scatter from modified TSM  
   // TSMtap = (distance to light, u, v)  
   float3 TSMtap = f3tex2D( TSMTex, TSM_coord.xy / TSM_coord.w );  
// Four average thicknesses through the object (in mm)  
   float4 thickness_mm = 1.0 * -(1.0 / 0.2) *  
                      log( float4( irrad2tap.w, irrad3tap.w,  
                                   irrad4tap.w, irrad5tap.w ));  
float2 stretchTap = f2tex2D( stretch32Tex, texCoord );  
float stretchval = 0.5 * ( stretchTap.x + stretchTap.y );  
float4 a_values = float4( 0.433, 0.753, 1.412, 2.722 );  
float4 inv_a = -1.0 / ( 2.0 * a_values * a_values );  
float4 fades = exp( thickness_mm * thickness_mm * inv_a );  
float textureScale = 1024.0 * 0.1 / stretchval;  
float blendFactor4 = saturate(textureScale *  
                              length( v2f.c_texCoord.xy - TSMtap.yz ) /  
                              ( a_values.y * 6.0 ) );  
float blendFactor5 = saturate(textureScale *  
                              length( v2f.c_texCoord.xy - TSMtap.yz ) /  
                              ( a_values.z * 6.0 ) );  
float blendFactor6 = saturate(textureScale *  
                              length( v2f.c_texCoord.xy - TSMtap.yz ) /  
                              ( a_values.w * 6.0 ) );  
diffuseLight += gauss4w / normConst * fades.y * blendFactor4 *  
                f3tex2D( irrad4Tex, TSMtap.yz ).xyz;  
diffuseLight += gauss5w / normConst * fades.z * blendFactor5 *  
                f3tex2D( irrad5Tex, TSMtap.yz ).xyz;  
diffuseLight += gauss6w / normConst * fades.w * blendFactor6 *  
                f3tex2D( irrad6Tex, TSMtap.yz ).xyz;  
// Determine skin color from a diffuseColor map  
diffuseLight *= pow(f3tex2D( diffuseColorTex, texCoord ), 1.0¨Cmix);  
// Energy conservation (optional) ¨C rho_s and m can be painted  
   // in a texture  
   float finalScale = 1 ¨C rho_s*f1tex2D(rhodTex, float2(dot(N, V), m);  
diffuseLight *= finalScale;  
float3 specularLight = 0;  
  // Compute specular for each light  
   for (<em>each light</em>)  
    specularLight += lightColor[i] * lightShadow[i] *  
                KS_Skin_Specular( N, L[i], V, m, rho_s, beckmannTex );  
  return float4( diffuseLight + specularLight, 1.0 );  
}  