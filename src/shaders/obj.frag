#version 330 core
out vec4 FragColor;

uniform vec3 color_object;
uniform vec3 color_light;

void main()
{
  float strength_ambient = 0.1f;
  vec3 ambient = strength_ambient * color_light;

  vec3 result = color_object * ambient;
  FragColor = vec4(result, 1.0f);
}
