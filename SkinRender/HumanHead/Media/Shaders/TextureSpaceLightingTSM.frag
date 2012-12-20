 #version 330

 #include "Common.include.glsl"

#define SHADOW_BAIS 0.0004
#define SHADOW_MAP_SIZE 1024

// parameters
uniform float AlbedoGamma = 2.2;
uniform float DiffuseColorMix = 0.5;
uniform float EnvAmount = 0.2;
uniform float Roughness = 0.3;
uniform float Rho_s = 0.18;


uniform mat4 World;// World Transform

uniform vec3 LightPos;	// Positon in world space
uniform	vec3 LightColor;	
uniform	float LightAmount;

uniform sampler2D   AlbedoTex;
uniform sampler2D   SpecTex; // spec amount in r, g, b, and roughness value over the surface
uniform sampler2D   NormalTex;	    
uniform sampler2D   ShadowTex;
uniform sampler2D   Rho_d_Tex; // Torrance-Sparrow BRDF integrated over hemisphere for range of roughness and incident angles
uniform samplerCube EnvCube;
 

in vec3 oWorldPos;
in vec3 oWorldNormal;
in vec4 oShadowCoord;
in vec2 oTex;

out vec4 FragColor;


void main()
{
	// lighting parameters
	vec3 L0 = normalize( LightPos - oWorldPos ); // point light 0 light vector

	// compute world normal
	vec3 N_nonBumped = normalize( oWorldNormal );

	// compute bumped world normal
    vec3 objNormal = texture( NormalTex, oTex ).xyz * vec3( 2.0, 2.0, 2.0 ) - vec3( 1.0, 1.0, 1.0 );  
    vec3 N_bumped = normalize( mat3(World) * objNormal );

	// N dot L
	float bumpDot_L0 = dot( N_bumped, L0 );

	//float L0atten = 600 * 600 / dot( LightPos - oWorldPos, LightPos - oWorldPos);
	float L0atten = 1.0;

#ifdef SHADOW_PCF	
	float L0Shadow = ShadowPCF(oShadowCoord, ShadowTex, vec2(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE), SHADOW_BAIS, 4, 1);
#else
	float L0Shadow = Shadow(oShadowCoord, ShadowTex, SHADOW_BAIS);
#endif

	vec3 pointLight0Color = LightColor * LightAmount * L0Shadow * L0atten;

	// DIFFUSE LIGHT  
    vec3 Li0cosi = saturate(bumpDot_L0) * pointLight0Color;

	// Specular Constant
	//vec4 specTap = texture( SpecTex, oTex ); // rho_s and roughness
	//float m = specTap.w * 0.09 + 0.23;	 // m is specular roughness
	//float rho_s = specTap.x * 0.16 + 0.18;

	float m = Roughness;	
	float rho_s = Rho_s;

    float rho_dt_L0 = 1.0 - Rho_s * texture( Rho_d_Tex, vec2( bumpDot_L0, m ) ).x;
	vec3 E0 = Li0cosi * rho_dt_L0;

	vec4 albedoTap = texture( AlbedoTex, oTex );
	vec3 albedo = albedoTap.xyz;
    
	//vec3 albedo = pow( albedoTap.xyz, vec3(AlbedoGamma) );
	//float occlusion = albedoTap.w;  
	float occlusion = 1.0;

	//vec3 cubeTap1 = texture( EnvCube, N_nonBumped ).xyz;
	//vec3 envLight = saturate( EnvAmount * cubeTap1.xyz * occlusion);
	vec3 envLight = vec3(0.1, 0.1, 0.1);

	//// start mixing the diffuse lighting - re-compute non-blurred lighting per pixel to get maximum resolutions
    vec3 diffuseContrib = pow( albedo.xyz, vec3(DiffuseColorMix) ) * (E0 + envLight);        
    vec3 finalCol = diffuseContrib.xyz;  


	//----------------------------------------------------------------------------------------------
	// Compute thickness
	vec3 shadowCoordWDiv = oShadowCoord.xyz / oShadowCoord.w;
	float distanceToLight = shadowCoordWDiv.z;

	vec4 TSMTap = texture(ShadowTex, shadowCoordWDiv.xy);
	
	vec3 objNormalBack = texture( NormalTex, TSMTap.yz).xyz * vec3( 2.0, 2.0, 2.0 ) - vec3( 1.0, 1.0, 1.0 );  
    vec3 N_bumpedBack = normalize( mat3(World) * objNormalBack ); 

	float backFacingEst = saturate( -dot( N_bumpedBack, N_bumped ) );  
    float thicknessToLight = distanceToLight - TSMTap.x;  
	
	// Set a large distance for surface points facing the light  
    if( bumpDot_L0 > 0.0 )  
    {  
		thicknessToLight = 100.0;  
    } 
	 
    float correctedThickness = saturate( -bumpDot_L0 ) * thicknessToLight * 10.0;  
    float finalThickness = mix( thicknessToLight, correctedThickness,  
                               backFacingEst );  
   
	FragColor = vec4(finalCol.xyz,  /*finalThickness, */finalThickness);
}
