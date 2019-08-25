#version 330 core
out vec4 FragColor;

uniform vec3 color_object;
uniform vec3 color_light;

void main()
{
  FragColor = vec4(color_light * color_object, 1.0f);
}
