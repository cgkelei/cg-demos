#version 330

#include "Common.include.glsl"

#define LIGHTCOUNT 3

uniform Light Lights[LIGHTCOUNT];
uniform mat4 World;

uniform sampler2D AlbedoTex;
uniform sampler2D NormalTex;
uniform sampler2D Rho_d_Tex; 
uniform sampler2D SpecTex;
uniform sampler2D StretchTex;
uniform sampler2D IrradTex[6*LIGHTCOUNT];
uniform sampler2D ShadowTex[LIGHTCOUNT];
uniform samplerCube IrradEnvMap;
uniform samplerCube GlossyEnvMap;
uniform sampler2D SeamMaskTex;

uniform vec3 GaussWeights[6];
uniform vec3 EyePos;

uniform float DiffuseColorMix = 0.5;
uniform float Roughness = 0.3;	
uniform float Rho_s = 0.18;
uniform float DepthScale = 10.0;
uniform float EnvAmount = 0.2;
uniform float SpecularIntensity = 1.88;

in vec3 oWorldPos;
in vec3 oWorldNormal;
in vec4 oShadowCoord[LIGHTCOUNT];
in float oLightDist[LIGHTCOUNT];
in vec2 oTex;

out vec4 FragColor;

void main()
{
	// lighting parameters
	vec3 L0 = normalize( Lights[0].Position - oWorldPos ); // point light 0 light vector
	vec3 L1 = normalize( Lights[1].Position - oWorldPos ); // point light 1 light vector
    vec3 L2 = normalize( Lights[2].Position - oWorldPos ); // point light 2 light vector

	// View vector
	vec3 V = normalize( EyePos - oWorldPos );
	
	vec3 N_nonBumped = normalize( oWorldNormal );
	// compute bumped world normal
    vec3 objNormal = texture( NormalTex, oTex ).xyz * vec3( 2.0, 2.0, 2.0 ) - vec3( 1.0, 1.0, 1.0 );  
    vec3 N_bumped = normalize( mat3(World) * objNormal );

	
	float depth0 = NormalizedDepth(oLightDist[0], Lights[0].NearFarPlane.x, Lights[0].NearFarPlane.y);
	float depth1 = NormalizedDepth(oLightDist[1], Lights[1].NearFarPlane.x, Lights[1].NearFarPlane.y);
	float depth2 = NormalizedDepth(oLightDist[2], Lights[2].NearFarPlane.x, Lights[2].NearFarPlane.y);

	vec2 UV0 = ShadowTexCoord(oShadowCoord[0]);
	vec2 UV1 = ShadowTexCoord(oShadowCoord[1]);
	vec2 UV2 = ShadowTexCoord(oShadowCoord[2]);

#ifdef SHADOW_PCF	
	float L0Shadow = ShadowPCF(UV0, ShadowTex[0], depth0 - SHADOW_BIAS, 4, 1);
	float L1Shadow = ShadowPCF(UV1, ShadowTex[1], depth1 - SHADOW_BIAS, 4, 1);
	float L2Shadow = ShadowPCF(UV2, ShadowTex[2], depth2 - SHADOW_BIAS, 4, 1);
#else
	#ifdef SHADOW_VSM 
	float L0Shadow = chebyshevUpperBound(depth0, texture2D(ShadowTex[0], UV0).ra);
	float L1Shadow = chebyshevUpperBound(depth1, texture2D(ShadowTex[1], UV1).ra);
	float L2Shadow = chebyshevUpperBound(depth2, texture2D(ShadowTex[2], UV2).ra);	
	#else
	float L0Shadow = Shadow(UV0, ShadowTex[0], depth0 - SHADOW_BIAS);
	float L1Shadow = Shadow(UV1, ShadowTex[1], depth1 - SHADOW_BIAS);
	float L2Shadow = Shadow(UV2, ShadowTex[2], depth2 - SHADOW_BIAS);
	#endif
#endif
	
	float bumpDot_L0 = dot( N_bumped, L0 );
    float bumpDot_L1 = dot( N_bumped, L1 );
    float bumpDot_L2 = dot( N_bumped, L2 );

	vec3 pointLight0Color = Lights[0].Color * Lights[0].Amount * L0Shadow ;
    vec3 pointLight1Color = Lights[1].Color * Lights[1].Amount * L1Shadow ;
    vec3 pointLight2Color = Lights[2].Color * Lights[2].Amount * L2Shadow ;


	//Specular Constant
    //vec4 specTap = texture( SpecTex, oTex ); // rho_s and roughness
    //float m = specTap.w * 0.09 + 0.23;	 // m is specular roughness
    //float rho_s = specTap.x * 0.16 + 0.18;
	
	float m = Roughness;	 // m is specular roughness
    float rho_s = Rho_s;

	//// DIFFUSE LIGHT  
    vec3 Li0cosi = saturate(bumpDot_L0) * pointLight0Color;
    vec3 Li1cosi = saturate(bumpDot_L1) * pointLight1Color;
    vec3 Li2cosi = saturate(bumpDot_L2) * pointLight2Color;

    float rho_dt_L0 = 1.0 - rho_s * texture( Rho_d_Tex, vec2( bumpDot_L0, m ) ).x;
    float rho_dt_L1 = 1.0 - rho_s * texture( Rho_d_Tex, vec2( bumpDot_L1, m ) ).x;
    float rho_dt_L2 = 1.0 - rho_s * texture( Rho_d_Tex, vec2( bumpDot_L2, m ) ).x;

	vec3 E0 = Li0cosi * rho_dt_L0;
    vec3 E1 = Li1cosi * rho_dt_L1;
    vec3 E2 = Li2cosi * rho_dt_L2;

	// material diffuse 
	vec4 albedoTap = texture( AlbedoTex, oTex );
	vec3 albedo = albedoTap.xyz;

	vec3 diffuseLight = vec3(0.0);
	
	// Deal with seam
	float seamMask = texture(SeamMaskTex, oTex).x;
    float alterSeamMask = pow( 1.0 - seamMask, 0.03);

	vec3 cubeTap1 = texture( IrradEnvMap, N_bumped ).xyz;
	vec3 envLight = saturate( EnvAmount * cubeTap1.xyz );

	// correct seam problems
    vec3 comparativeLocalLightColor = pow(albedo, vec3(DiffuseColorMix) ) * ( E0 + E1 + E2 + envLight );

	// Light0 
	vec4 irrad0Tap0 = texture(IrradTex[0 * 6 + 0], oTex);
	vec4 irrad1Tap0 = texture(IrradTex[0 * 6 + 1], oTex);
	vec4 irrad2Tap0 = texture(IrradTex[0 * 6 + 2], oTex);
	vec4 irrad3Tap0 = texture(IrradTex[0 * 6 + 3], oTex);
	vec4 irrad4Tap0 = texture(IrradTex[0 * 6 + 4], oTex);
	vec4 irrad5Tap0 = texture(IrradTex[0 * 6 + 5], oTex);

	diffuseLight += GaussWeights[0] * irrad0Tap0.xyz;  
	diffuseLight += GaussWeights[1] * irrad1Tap0.xyz;  
	diffuseLight += GaussWeights[2] * irrad2Tap0.xyz;  
	diffuseLight += GaussWeights[3] * irrad3Tap0.xyz;  
	diffuseLight += GaussWeights[4] * irrad4Tap0.xyz;  
	diffuseLight += GaussWeights[5] * irrad5Tap0.xyz; 

	// Light1
	vec4 irrad0Tap1 = texture(IrradTex[1 * 6 + 0], oTex);
	vec4 irrad1Tap1 = texture(IrradTex[1 * 6 + 1], oTex);
	vec4 irrad2Tap1 = texture(IrradTex[1 * 6 + 2], oTex);
	vec4 irrad3Tap1 = texture(IrradTex[1 * 6 + 3], oTex);
	vec4 irrad4Tap1 = texture(IrradTex[1 * 6 + 4], oTex);
	vec4 irrad5Tap1 = texture(IrradTex[1 * 6 + 5], oTex);

	diffuseLight += GaussWeights[0] * irrad0Tap1.xyz;  
	diffuseLight += GaussWeights[1] * irrad1Tap1.xyz;  
	diffuseLight += GaussWeights[2] * irrad2Tap1.xyz;  
	diffuseLight += GaussWeights[3] * irrad3Tap1.xyz;  
	diffuseLight += GaussWeights[4] * irrad4Tap1.xyz;  
	diffuseLight += GaussWeights[5] * irrad5Tap1.xyz; 

	// Light2
	vec4 irrad0Tap2 = texture(IrradTex[2 * 6 + 0], oTex);
	vec4 irrad1Tap2 = texture(IrradTex[2 * 6 + 1], oTex);
	vec4 irrad2Tap2 = texture(IrradTex[2 * 6 + 2], oTex);
	vec4 irrad3Tap2 = texture(IrradTex[2 * 6 + 3], oTex);
	vec4 irrad4Tap2 = texture(IrradTex[2 * 6 + 4], oTex);
	vec4 irrad5Tap2 = texture(IrradTex[2 * 6 + 5], oTex);

	diffuseLight += GaussWeights[0] * irrad0Tap2.xyz;  
	diffuseLight += GaussWeights[1] * irrad1Tap2.xyz;  
	diffuseLight += GaussWeights[2] * irrad2Tap2.xyz;  
	diffuseLight += GaussWeights[3] * irrad3Tap2.xyz;  
	diffuseLight += GaussWeights[4] * irrad4Tap2.xyz;  
	diffuseLight += GaussWeights[5] * irrad5Tap2.xyz; 

	// Renormalize diffusion profiles to white  
    vec3 normConst = GaussWeights[0] + GaussWeights[1] + GaussWeights[2] + GaussWeights[3] + GaussWeights[4] + GaussWeights[5];  
    diffuseLight /= normConst; // Renormalize to white diffuse light   

	// correct seam problems
	diffuseLight =  mix( diffuseLight, comparativeLocalLightColor, alterSeamMask);  

//---------------------------------------------------------------------------------------------------------------------
	vec2 stretchTap = texture( StretchTex, oTex ).xy;
	float stretchval = 0.5 * (stretchTap.x + stretchTap.y);
	vec4 a_values = vec4(0.433, 0.753, 1.412, 2.722);
	vec4 inv_a = -1.0 / (2.0 * a_values * a_values );
	float textureScale = 1024.0 * 0.1 / stretchval;
	
	// Compute global scatter from modified TSM  
    float texDist, blend, blendFactor3, blendFactor4, blendFactor5;
	vec4 thickness_mm, fades;

	// Light0 
	vec3 TSMtap0 = texture( ShadowTex[0], UV0).xyz; 
	texDist = length(oTex.xy - TSMtap0.yz);
		
	// Four average thicknesses through the object (in mm)  
	thickness_mm = DepthScale * RealDepth ( vec4( irrad1Tap0.w, irrad2Tap0.w,  irrad3Tap0.w, irrad4Tap0.w ), Lights[0].NearFarPlane);
	fades = exp( thickness_mm * thickness_mm * inv_a );  
	blend = textureScale *  texDist;

	blendFactor3 = saturate(blend / ( a_values.y * 6.0) );  
	blendFactor4 = saturate(blend / ( a_values.z * 6.0) );  
	blendFactor5 = saturate(blend / ( a_values.w * 6.0) );  
		
	diffuseLight += GaussWeights[3]  * fades.y * blendFactor3 * texture( IrradTex[0 * 6 + 3], TSMtap0.yz ).xyz / normConst;  
	diffuseLight += GaussWeights[4]  * fades.z * blendFactor4 * texture( IrradTex[0 * 6 + 4], TSMtap0.yz ).xyz / normConst; 
	diffuseLight += GaussWeights[5]  * fades.w * blendFactor5 * texture( IrradTex[0 * 6 + 5], TSMtap0.yz ).xyz / normConst;

	// Light1 
	vec3 TSMtap1 = texture( ShadowTex[1], UV1 ).xyz; 
	texDist = length(oTex.xy - TSMtap1.yz);
		
	// Four average thicknesses through the object (in mm)  		
	thickness_mm = DepthScale * RealDepth ( vec4( irrad1Tap1.w, irrad2Tap1.w,  irrad3Tap1.w, irrad4Tap1.w ), Lights[0].NearFarPlane);
	fades = exp( thickness_mm * thickness_mm * inv_a );  

	blend = textureScale *  texDist;

	blendFactor3 = saturate(blend / ( a_values.y * 6.0) );  
	blendFactor4 = saturate(blend / ( a_values.z * 6.0) );  
	blendFactor5 = saturate(blend / ( a_values.w * 6.0) );  
		
	diffuseLight += GaussWeights[3]  * fades.y * blendFactor3 * texture( IrradTex[1 * 6 + 3], TSMtap1.yz ).xyz / normConst;  
	diffuseLight += GaussWeights[4]  * fades.z * blendFactor4 * texture( IrradTex[1 * 6 + 4], TSMtap1.yz ).xyz / normConst; 
	diffuseLight += GaussWeights[5]  * fades.w * blendFactor5 * texture( IrradTex[1 * 6 + 5], TSMtap1.yz ).xyz / normConst;

	// Light2 
	vec3 TSMtap2 = texture( ShadowTex[2], UV2 ).xyz; 
	texDist = length(oTex.xy - TSMtap2.yz);
		
	// Four average thicknesses through the object (in mm)  		
	thickness_mm = DepthScale * RealDepth ( vec4( irrad1Tap2.w, irrad2Tap2.w,  irrad3Tap2.w, irrad4Tap2.w ), Lights[1].NearFarPlane);
	fades = exp( thickness_mm * thickness_mm * inv_a );  

	blend = textureScale *  texDist;

	blendFactor3 = saturate(blend / ( a_values.y * 6.0) );  
	blendFactor4 = saturate(blend / ( a_values.z * 6.0) );  
	blendFactor5 = saturate(blend / ( a_values.w * 6.0) );  
		
	diffuseLight += GaussWeights[3]  * fades.y * blendFactor3 * texture( IrradTex[2 * 6 + 3], TSMtap2.yz ).xyz / normConst;  
	diffuseLight += GaussWeights[4]  * fades.z * blendFactor4 * texture( IrradTex[2 * 6 + 4], TSMtap2.yz ).xyz / normConst; 
	diffuseLight += GaussWeights[5]  * fades.w * blendFactor5 * texture( IrradTex[2 * 6 + 5], TSMtap2.yz ).xyz / normConst;

	// Determine skin color from a diffuseColor map
	//vec3 albedo = texture( AlbedoTex, oTex ).xyz;
	diffuseLight *=  pow( albedo.xyz, vec3(1.0 - DiffuseColorMix) );

	// Constant for specular calculation
	// Use constant parameters m(roughness) and rho_s(intensity) for specular calculation
	
	//Energy conservation (optional) - rho_s and m can be painted
	float finalScale = 1 - rho_s * texture(Rho_d_Tex, vec2(dot(N_bumped, V), m)).x;
	diffuseLight *= finalScale;

	vec3 specularLight = vec3(0);  
    // Compute specular for each light  
    specularLight += Lights[0].Color * Lights[0].Amount * L0Shadow * KS_Skin_Specular(N_bumped, L0, V, m, rho_s, Rho_d_Tex );
	specularLight += Lights[1].Color * Lights[1].Amount * L1Shadow * KS_Skin_Specular(N_bumped, L1, V, m, rho_s, Rho_d_Tex );
	specularLight += Lights[2].Color * Lights[2].Amount * L2Shadow * KS_Skin_Specular(N_bumped, L2, V, m, rho_s, Rho_d_Tex );

	// Compute specular for env light 
	vec3 R_bumped = normalize( reflect( -V, N_bumped ) );	// refelct vector
    vec3 R_nonBumped = normalize( reflect( -V, N_bumped ) );

	// Gloss is the [0..1] value from your gloss map not decompressed in specular power
	float MipmapIndex = (1 - m) * (GlossyNumMipmap - 1);  
	vec3 AmbientSpecular = textureLod(GlossyEnvMap, R_bumped, MipmapIndex).xyz;
	specularLight += EnvAmount * AmbientSpecular * FresnelReflectance( normalize(R_bumped + V), V,  0.028 );

	specularLight *= SpecularIntensity;
	//FragColor = pow( FragColor.xyz, vec3(1.0 / 2.2) );

	//FragColor = vec4(alterSeamMask, alterSeamMask, alterSeamMask, diffuseLight + specularLight); 
	//FragColor = vec4(diffuseLight , specularLight );  
	FragColor = vec4(diffuseLight + specularLight, 1.0 );  
}