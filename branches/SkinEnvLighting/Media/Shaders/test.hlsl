#include "commonlib.cg"

#define SMAP_SIZE 1600
#define SHADOW_EPSILON 0.009f

// application to vertex shader
struct a2v 
{
    float4 Position		: POSITION;
    float3 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD0;
};

// vertex shader to fragment shader
struct v2f 
{
    float4 P	: POSITION;		// position
	float3 N	: TEXCOORD0;	// normal
    float2 T	: TEXCOORD1;	// tex coord
	
	float3 worldCoord	: TEXCOORD2;
	float3 dist			: TEXCOORD3;
	float4 TSMcoord1	: TEXCOORD4;
	float4 TSMcoord2	: TEXCOORD5;
	float4 TSMcoord3	: TEXCOORD6;
};

v2f irradiancesingle_vp(a2v In,
				uniform float4x4	viewProjWin_Target1,
				uniform float4x4	viewProjWin_Target2,
				uniform float4x4	viewProjWin_Target3,
				uniform float4x4	model)
{
    v2f Out;

    // transform position to world space
    float4 worldPos = mul(model, In.Position);

    // transform normal to world space
    float3 N = mul((float3x3) model, In.Normal.xyz);

    Out.N = normalize(N);
    Out.T = In.TexCoord;
	Out.P = v2t(In.TexCoord);
	
	Out.worldCoord = worldPos.xyz;
	
	float4 depthCoord1 = mul(viewProjWin_Target1, worldPos);
	depthCoord1 /= depthCoord1.w;
	Out.TSMcoord1.xyz = 0.5*depthCoord1.xyz + float3(0.5, 0.5, 0.5);
	Out.TSMcoord1.w = 1.0;
	
	float4 depthCoord2 = mul(viewProjWin_Target2, worldPos);
	depthCoord2 /= depthCoord2.w;
	Out.TSMcoord2.xyz = 0.5*depthCoord2.xyz + float3(0.5, 0.5, 0.5);
	Out.TSMcoord2.w = 1.0;
	
	float4 depthCoord3 = mul(viewProjWin_Target3, worldPos);
	depthCoord3 /= depthCoord3.w;
	Out.TSMcoord3.xyz = 0.5*depthCoord3.xyz + float3(0.5, 0.5, 0.5);
	Out.TSMcoord3.w = 1.0;
	
	Out.dist = float3(depthCoord1.z, depthCoord2.z, depthCoord3.z);

	return Out;
}

float4 irradiancesingle_fp(v2f In, 
				
				uniform float3		s_worldEyePos,
				
				uniform sampler2D	skinTex		: TEXUNIT0,
				uniform sampler2D	normalTex	: TEXUNIT1,
				uniform sampler2D	rhodTex		: TEXUNIT2,			// x: rho_s, y: beckmannTex, z: fresnelReflectance
				uniform sampler2D	TSMTex1		: TEXUNIT3,
				uniform sampler2D	TSMTex2		: TEXUNIT4,
				uniform sampler2D	TSMTex3		: TEXUNIT5,
				uniform float4x4	model
                ) : COLOR
{
	//***1: compute irradiance...
    float3 objNormal = f3tex2D( normalTex, In.T ) * float3( 2.0, 2.0, 2.0 ) - float3( 1.0, 1.0, 1.0 );
	float3 N = normalize( mul( (float3x3)model, objNormal ) );
	float3 vertexN = In.N;
	
	float3 worldLight1 = mul((float3x3) model, glstate.light[0].position.xyz);
	float3 L1 = normalize( worldLight1 );
	
	float3 worldLight2 = mul((float3x3) model, glstate.light[1].position.xyz);
	float3 L2 = normalize( worldLight2 );
	
	float3 worldLight3 = mul((float3x3) model, glstate.light[2].position.xyz);
	float3 L3 = normalize( worldLight3 );
	
	float3 distanceToLight = In.dist;
	float2 TSMcoord1 = In.TSMcoord1.xy/ In.TSMcoord1.w;
	float4 TSMTap1 = f4tex2D( TSMTex1, TSMcoord1 );
	
	float2 TSMcoord2 = In.TSMcoord2.xy/ In.TSMcoord2.w;
	float4 TSMTap2 = f4tex2D( TSMTex2, TSMcoord2 );
	
	float2 TSMcoord3 = In.TSMcoord3.xy/ In.TSMcoord3.w;
	float4 TSMTap3 = f4tex2D( TSMTex3, TSMcoord3 );
	
	
	//** VSM
	float lightShadow1 = chebyshevUpperBound(distanceToLight.x, TSMTap1.ra);
	float lightShadow2 = chebyshevUpperBound(distanceToLight.y, TSMTap2.ra);
	float lightShadow3 = chebyshevUpperBound(distanceToLight.z, TSMTap3.ra);
	
	float m = 0.3;
	float rho_s = 0.18;
	
	float ndotl1 = dot(N, L1);
	float3 lightColor1 = float3(0.3, 0.3, 0.3) * lightShadow1;
	float3 lightCos1 = saturate(ndotl1) * lightColor1;
	float rho_dt_L1 = 1 - rho_s * f3tex2D(rhodTex, float2(ndotl1, m)).x;
	float3 E1 = lightCos1 * rho_dt_L1;
	
	float ndotl2 = dot(N, L2);
	float3 lightColor2 = float3(0.3, 0.3, 0.3) * lightShadow2;
	float3 lightCos2 = saturate(ndotl2) * lightColor2;
	float rho_dt_L2 = 1 - rho_s * f3tex2D(rhodTex, float2(ndotl2, m)).x;
	float3 E2 = lightCos2 * rho_dt_L2;
	
	float ndotl3 = dot(N, L3);
	float3 lightColor3 = float3(0.3, 0.3, 0.3) * lightShadow3;
	float3 lightCos3 = saturate(ndotl3) * lightColor3;
	float rho_dt_L3 = 1 - rho_s * f3tex2D(rhodTex, float2(ndotl3, m)).x;
	float3 E3 = lightCos3 * rho_dt_L3;
	
	float3 texel = tex2D(skinTex, In.T).rgb;
	float3 envLight = float3(0.1, 0.1, 0.1);
	float3 irradiance = (E1 + E2 + E3 + envLight) * sqrt(texel);
	
	return float4( irradiance, 1.0 );
}


//----------------------------------------------------------------------------------------------

#include "nvdemo_fs_lib.fp30"

struct v2fConnector {
  float2 c_texCoord           : TEXCOORD0; // has to be first due to vs generator error
  float4 adriTSLProjCoord     : POSITION;
  float3 worldCoord           : TEXCOORD1; 
  float4 winTarget1DepthCoord : TEXCOORD2;
  float4 winTarget2DepthCoord : TEXCOORD3;
  float4 winTarget3DepthCoord : TEXCOORD4;
  float3 worldNormal          : TEXCOORD5;  
  //float2 WPOS                 : WPOS;
};

float4 main(

	    // shader params
	    uniform float3 s_PointLight0Color,
	    uniform float3 s_PointLight1Color,
	    uniform float3 s_PointLight2Color,
	    uniform float3 s_worldPointLight0Pos,
	    uniform float3 s_worldPointLight1Pos,
	    uniform float3 s_worldPointLight2Pos,
	    uniform float  s_pointLite0Amount,                 
	    uniform float  s_pointLite1Amount,                 
	    uniform float  s_pointLite2Amount,                 
	    uniform float  s_shadowBias,
	    uniform float  s_shadowSpread,	    
	    uniform float  s_diffColMix,
	    uniform float  s_envAmount,
	    uniform float3 s_worldEyePos,
	    
	    uniform float4x4   model, // needed to transform obj space normal map into world space

	    // textures
	    uniform texobj2D   albedoTex,
	    uniform texobj2D   lightDistTex,
	    uniform texobj2D   specTex, // spec amount in r, g, b, and roughness value over the surface
	    uniform texobj2D   nermelTex,	    
	    uniform texobj2D   shadowTex0,
	    uniform texobj2D   shadowTex1,
	    uniform texobj2D   shadowTex2,
	    uniform texobj2D   rho_d_Tex, // Torrance-Sparrow BRDF integrated over hemisphere for range of roughness and incident angles
	    uniform texobjCUBE envDiffCube,
	    uniform texobjCUBE ,
                
	    v2fConnector v2f

	    ) : COLOR {

  // lighting parameters
  float3 L1 = normalize( s_worldPointLight0Pos - v2f.worldCoord ); // point light 0 light vector
  //float3 L2 = normalize( s_worldPointLight1Pos - v2f.worldCoord ); // point light 0 light vector
  float3 L3 = normalize( s_worldPointLight2Pos - v2f.worldCoord ); // point light 0 light vector
  float standardShadowTap_L1 = shadow4( shadowTex0, v2f.winTarget1DepthCoord, -3.0, s_shadowSpread );
 // float standardShadowTap_L2 = shadow4( shadowTex1, v2f.winTarget2DepthCoord, -3.0, s_shadowSpread );
  float standardShadowTap_L3 = shadow4( shadowTex2, v2f.winTarget3DepthCoord, -3.0, s_shadowSpread );
  float L1Shadow = standardShadowTap_L1;
  //float L2Shadow = standardShadowTap_L2;
  float L3Shadow = standardShadowTap_L3;
  
  float3 NS = normalize( v2f.worldNormal );
  
  // compute world normal
  float3 objNormal = f3tex2D( nermelTex, v2f.c_texCoord.xy ) * float3( 2.0, 2.0, 2.0 ) - float3( 1.0, 1.0, 1.0 );  
  float3 worldNormal = normalize( vecMul( model, objNormal ) );
  float3 N = worldNormal;
  
  // compute view vector
  float3 worldEyeVec = normalize( v2f.worldCoord - s_worldEyePos );
  float3 V = -worldEyeVec;  
    
  float3 R_bumped = ( reflect( V, N ) );
  float3 R = reflect( V, NS );
  
  float bumpDot_L1 = dot( N, L1 );
 // float bumpDot_L2 = dot( N, L2 );
  float bumpDot_L3 = dot( N, L3 );
  
  float L1atten = 600 * 600 / dot( s_worldPointLight0Pos - v2f.worldCoord, s_worldPointLight0Pos - v2f.worldCoord);
  float L3atten = 600 * 600 / dot( s_worldPointLight2Pos - v2f.worldCoord, s_worldPointLight2Pos - v2f.worldCoord); 
    
  float3 pointLight0Color = s_PointLight0Color * L1Shadow * s_pointLite0Amount * L1atten;
  //float3 pointLight1Color = s_PointLight1Color * L2Shadow * s_pointLite1Amount;
  float3 pointLight2Color = s_PointLight2Color * L3Shadow * s_pointLite2Amount * L3atten;
  
  float4 specTap = f4tex2D( specTex, v2f.c_texCoord.xy ); // specular amount in R, G, B, and roughness
  float surface_roughness = specTap.w * 0.09 + 0.23;
  float rho_s = specTap.x * 0.16 + 0.18;
    
  // DIFFUSE LIGHT  
  float3 Li1cosi = saturate( bumpDot_L1 ) * pointLight0Color;
  //float3 Li2cosi = saturate( bumpDot_L2 ) * pointLight1Color;
  float3 Li3cosi = saturate( bumpDot_L3 ) * pointLight2Color;

  float3 rho_dt_L1 = 1.0 - rho_s * f1tex2D( rho_d_Tex, float2( bumpDot_L1, surface_roughness ) );
  //float3 rho_dt_L2 = 1.0 - rho_s * f1tex2D( rho_d_Tex, float2( bumpDot_L2, surface_roughness ) );
  float3 rho_dt_L3 = 1.0 - rho_s * f1tex2D( rho_d_Tex, float2( bumpDot_L3, surface_roughness ) );

  float3 E1 = Li1cosi * rho_dt_L1;
  //float3 E2 = Li2cosi * rho_dt_L2;
  float3 E3 = Li3cosi * rho_dt_L3;
    
  float4 albedoTap = f4tex2D( albedoTex, v2f.c_texCoord.xy );
  float3 albedo = pow( albedoTap.xyz, 2.2 );  
  float occlusion = albedoTap.w;  
    
  float3 cubeTap1 = pow( f3texCUBE( envDiffCube, N ).xyz, 1.0 );
    
  // start mixing the diffuse lighting - re-compute non-blurred lighting per pixel to get maximum resolutions
  float3 diffuseContrib = pow( albedo.xyz, s_diffColMix ) * (E1 + E3 + saturate( s_envAmount * cubeTap1.xyz * occlusion ));        
  float3 finalCol = diffuseContrib.xyz;      
  return float4( finalCol, 1.0 );

}


#include "commonlib.cg"

#define SMAP_SIZE 1600
#define SHADOW_EPSILON 0.009f

// application to vertex shader
struct a2v 
{
    float4 Position		: POSITION;
    float3 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD0;
};

// vertex shader to fragment shader
struct v2f 
{
    float4 P	: POSITION;		// position
	float3 N	: TEXCOORD0;	// normal
    float2 T	: TEXCOORD1;	// tex coord
	
	float3 worldCoord	: TEXCOORD2;
	float dist			: TEXCOORD3;
	float4 TSMcoord	: TEXCOORD4;
};

v2f irradiance_vp(a2v In,
				uniform float4x4	viewProjWin_Target,
				//uniform float4x4	viewProjWin_Target2,
				//uniform float4x4	viewProjWin_Target3,
				uniform float4x4	model)
{
    v2f Out;

    // transform position to world space
    float4 worldPos = mul(model, In.Position);

    // transform normal to world space
    float3 N = mul((float3x3) model, In.Normal.xyz);

    Out.N = normalize(N);
    Out.T = In.TexCoord;
	Out.P = v2t(In.TexCoord);
	
	Out.worldCoord = worldPos.xyz;
	
	float4 depthCoord = mul(viewProjWin_Target, worldPos);
	depthCoord /= depthCoord.w;
	Out.TSMcoord.xyz = 0.5*depthCoord.xyz + float3(0.5, 0.5, 0.5);
	Out.TSMcoord.w = 1.0;
	
	Out.dist = depthCoord.z;

	return Out;
}

float4 irradiance_fp(v2f In, 
				
				uniform float3		s_worldEyePos,
				
				uniform sampler2D	skinTex		: TEXUNIT0,
				uniform sampler2D	normalTex	: TEXUNIT1,
				uniform sampler2D	rhodTex		: TEXUNIT2,			// x: rho_s, y: beckmannTex, z: fresnelReflectance
				uniform sampler2D	TSMTex		: TEXUNIT3,
				uniform float4x4	model
                ) : COLOR
{
	//***1: compute irradiance...
    float3 objNormal = f3tex2D( normalTex, In.T ) * float3( 2.0, 2.0, 2.0 ) - float3( 1.0, 1.0, 1.0 );
	float3 N = normalize( mul( (float3x3)model, objNormal ) );
	float3 vertexN = In.N;
    	
    //s_worldEyePos = mul((float3x3)model, s_worldEyePos);
	//float3 V = normalize( s_worldEyePos - In.worldCoord );
	
	float3 worldLight = mul((float3x3) model, glstate.light[0].position.xyz);
	float3 L = normalize( worldLight );
	
	
	float distanceToLight = In.dist;
	float2 TSMcoord = In.TSMcoord.xy/ In.TSMcoord.w;
	float4 TSMTap = f4tex2D( TSMTex, TSMcoord );
	
	
	//** VSM
	float lightShadow = chebyshevUpperBound(distanceToLight, TSMTap.ra);
	
	float m = 0.3;
	float rho_s = 0.18;
	
	float ndotl = dot(N, L);
	float3 lightColor = float3(0.3, 0.3, 0.3) * lightShadow;
	float3 lightCos = saturate(ndotl) * lightColor;
	float rho_dt_L = 1 - rho_s * f3tex2D(rhodTex, float2(ndotl, m)).x;
	float3 E = lightCos * rho_dt_L;
	
	float3 texel = tex2D(skinTex, In.T).rgb;
	float3 envLight = float3(0.03, 0.03, 0.03);
	float3 irradiance = (E + envLight) * sqrt(texel);
	
	//** warp light
	//float3 wlight = wrapLight(N, L);
	//irradiance += wlight;
	
	//***3: compute alpha...
	// Find normal on back side of object, Ni
	float3 objNi = f3tex2D( normalTex, TSMTap.yz ) * float3( 2.0, 2.0, 2.0 ) - float3( 1.0, 1.0, 1.0 );
	float3 Ni = mul( (float3x3)model, objNi);
	
	float backFacingEst = saturate( -dot( Ni, vertexN ));
	float thicknessToLight = distanceToLight - TSMTap.x;

	thicknessToLight = max(thicknessToLight, 0.0);
	
	// Set a large distance for surface points facing the light
	float vertexNdotL = dot(L, vertexN);
	if( vertexNdotL > 0.0 )
		thicknessToLight = 100.0;//500.0*vertexNdotL;
	
	float correctedThickness = saturate( -vertexNdotL ) * thicknessToLight;
	float finalThickness = lerp( thicknessToLight, correctedThickness, backFacingEst );

	//***4: return irradiance and finalThickness
	return float4( irradiance, finalThickness );
}