attribute vec3 aPosition;
attribute vec2 aUvs;

varying vec2 vUvs;
uniform mat4 ModelViewProjection;

void main(void)
{
   gl_Position = ModelViewProjection * vec4(aPosition, 1);
   vUvs = aUvs;
}
