uniform sampler2D texture;
       vec3 glyph_color    = vec3(1.0,1.0,1.0);
const float glyph_center   = 0.50;
       vec3 outline_color  = vec3(1.0,0.0,0.0);
const float outline_center = 0.4;
       vec3 glow_color     = vec3(1.0,1.0,1.0);
const float glow_center    = 1.25;
void main(void)
{
    vec4  color = texture2D(texture, gl_TexCoord[0].st);
    float dist  = color.a;
    float width = fwidth(dist);
    float alpha = smoothstep(glyph_center-width, glyph_center+width, dist);
	
	/*
	if( dist > 0.45 && dist < 0.55)
		gl_FragColor = vec4(glyph_color, alpha);
	else if (dist > 0.55)
		gl_FragColor = vec4(1, 0, 0, alpha);
	else
		gl_FragColor = vec4(0, 0, 0, alpha);*/

    // Smooth
    //gl_FragColor = vec4(glyph_color, alpha);

    // Outline
    //float mu = smoothstep(outline_center-width, outline_center+width, dist);
   // vec3 rgb = mix(outline_color, glyph_color, mu);
   // gl_FragColor = vec4(rgb, mu);

    // Glow
    vec3 rgb = mix(glow_color, glyph_color, alpha);
    float mu = smoothstep(glyph_center, glow_center, sqrt(dist));
    gl_FragColor = vec4(rgb, max(alpha,mu));

    // Glow + outline
    //vec3 rgb = mix(glow_color, glyph_color, alpha);
   // float mu = smoothstep(glyph_center, glow_center, sqrt(dist));
   // color = vec4(rgb, max(alpha,mu));
   // float beta = smoothstep(outline_center-width, outline_center+width, dist);
   // rgb = mix(outline_color, color.rgb, beta);
   // gl_FragColor = vec4(rgb, max(color.a,beta));

}


