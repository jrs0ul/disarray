#ifdef GL_ES
//
precision mediump float;

#endif


uniform sampler2D uTexture;

varying vec2 vUvs;

void main(void)
{
    vec4 texel = texture2D(uTexture, vUvs);
    if (texel.a < 0.5)
    {
        discard;
    }
    gl_FragColor = texel;
}
