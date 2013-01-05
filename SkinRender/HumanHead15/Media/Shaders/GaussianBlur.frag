#version 120

uniform sampler2D InputTex;
uniform vec2 Step;
uniform float GaussWidth; // Scale ¨C Used to widen Gaussian taps.  GaussWidth should be the standard deviation. 

void main()
{
	 // Gaussian curve ¨C standard deviation of 1.0  
    float weights[7] = float[7](0.006,0.061,0.242,0.383,0.242,0.061,0.006);  
    vec2 coords = gl_TexCoord[0].xy - Step * GaussWidth * 3.0;
    vec4 sum = vec4(0.0);  
    for( int i = 0; i < 7; i++ )  
    {  
	    sum += weights[i] * texture2D( InputTex,  coords );
		coords += Step * GaussWidth;
    }  

	gl_FragColor = sum;
}