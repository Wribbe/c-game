#version 330 core

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec3 Normal;
in vec3 position_frag;

out vec4 FragColor;

uniform vec3 position_view;

uniform Material material;
uniform Light light;

void main()
{
  // Ambient.
  vec3 ambient = light.ambient * material.ambient;

  // Diffuse.
  vec3 normal = normalize(Normal);
  vec3 direction_light = normalize(light.position - position_frag);
  float diff = max(dot(normal, direction_light), 0.0f);
  vec3 diffuse = light.diffuse * (diff * material.diffuse);

  // Specular.
  vec3 direction_view = normalize(position_view - position_frag);
  vec3 direction_reflect = reflect(-direction_light, normal);
  float spec = pow(max(dot(direction_view, direction_reflect), 0.0f), material.shininess);
  vec3 specular = light.specular * (spec * material.specular);

  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0f);
}
