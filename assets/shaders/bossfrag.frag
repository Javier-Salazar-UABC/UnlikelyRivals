#version 110
uniform sampler2D textura;
uniform float distancia;
void main()
{
    vec4 pixeles = texture2D(textura,gl_TexCoord[0].xy);

    vec4 color_final = pixeles;

    if(distancia <200.0)
    {
        if(color_final.a >0.0)
        {
            color_final.rgb = 1.0-color_final.rgb;
        }
    }
    else
    {
        if(color_final.a >0.0)
        {
            color_final = 1.0-color_final;
        }
    }
    color_final.b+=0.1;
    gl_FragColor = color_final;
}