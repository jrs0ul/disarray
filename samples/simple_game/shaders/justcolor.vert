attribute vec2 aPosition;
attribute vec4 aColor;

varying vec4 vColor;
uniform mat4 ModelViewProjection;

void main(void)
{
   gl_Position = ModelViewProjection * vec4(aPosition, 0, 1);
   vColor = aColor;
}
