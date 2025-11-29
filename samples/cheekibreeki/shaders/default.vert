attribute vec2 aPosition;
attribute vec2 aUvs;
attribute vec4 aColor;

varying vec2 vUvs;
varying vec4 vColor;
uniform mat4 ModelViewProjection;

void main(void)
{
   gl_Position = ModelViewProjection * vec4(aPosition, 0 , 1);
   vUvs = aUvs;
   vColor = aColor;
}
