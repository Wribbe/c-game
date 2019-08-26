#version 330 core

in vec3 Normal;
in vec3 position_frag;

out vec4 FragColor;

uniform vec3 color_object;
uniform vec3 color_light;
uniform vec3 position_light;

void main()
{
  float strength_ambient = 0.1f;
  vec3 ambient = strength_ambient * color_light;

  vec3 normal = normalize(Normal);
  vec3 direction_light = normalize(position_light - position_frag);

  float diff = max(dot(normal, direction_light), 0.0f);
  vec3 diffuse = diff * color_light;

  vec3 result = (ambient + diffuse) * color_object;
  FragColor = vec4(result, 1.0f);
}
