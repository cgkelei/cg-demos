#version 120

uniform sampler2D InputTex;	// inputTex ¨C Texture being convolved  
uniform float GaussWidth; // Scale ¨C Used to widen Gaussian taps.  GaussWidth should be the standard deviation. 
  
void main()
{
	float scaleConv = 1.0 / 800.0;  
	vec2 stretch = texture2D( InputTex, gl_TexCoord[0].xy).xy;  
	float netFilterWidth = scaleConv * GaussWidth * stretch.x;

	 // Gaussian curve ¨C standard deviation of 1.0  
    float curve[7] = float[7](0.006,0.061,0.242,0.383,0.242,0.061,0.006);  
    vec2 coords = gl_TexCoord[0].xy - vec2( netFilterWidth * 3.0, 0.0 );  
    vec4 sum = vec4(0.0);  
    for( int i = 0; i < 7; i++ )  
    {  
		vec4 tap = texture2D( InputTex,  coords );  
	    sum += curve[i] * tap;  
		coords += vec2( netFilterWidth, 0.0 );  
    }  

	gl_FragColor = sum;
}