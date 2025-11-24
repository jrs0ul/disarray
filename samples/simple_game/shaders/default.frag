#ifdef GL_ES
//
precision mediump float;

#endif


uniform sampler2D uTexture;

varying vec2 vUvs;
varying vec4 vColor;

void main(void)
{
   gl_FragColor = texture2D(uTexture, vUvs) * vColor;
}
