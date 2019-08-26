#version 330 core

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

in vec3 Normal;
in vec3 position_frag;

out vec4 FragColor;

uniform vec3 color_light;
uniform vec3 position_light;
uniform vec3 position_view;

uniform Material material;

void main()
{
  // Ambient.
  vec3 ambient = color_light * material.ambient;

  // Diffuse.
  vec3 normal = normalize(Normal);
  vec3 direction_light = normalize(position_light - position_frag);
  float diff = max(dot(normal, direction_light), 0.0f);
  vec3 diffuse = color_light * (diff * material.diffuse);

  // Specular.
  vec3 direction_view = normalize(position_view - position_frag);
  vec3 direction_reflect = reflect(-direction_light, normal);
  float spec = pow(max(dot(direction_view, direction_reflect), 0.0f), material.shininess);
  vec3 specular = color_light * (spec * material.specular);

  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0f);
}
