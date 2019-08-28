#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct LightDirectional {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};


in vec3 Normal;
in vec3 position_frag;
in vec2 coords_tex;

out vec4 FragColor;

uniform vec3 position_view;

uniform Material material;

uniform LightDirectional light_directional;

vec3
calculate_light_directional(
  LightDirectional light,
  vec3 normal,
  vec3 direction_view)
{
  vec3 direction_light = normalize(-light.direction);
  // Diffuse shading.
  float diff = max(dot(normal, direction_light), 0.0f);
  // Specular shading.
  vec3 direction_reflect = reflect(-direction_light, normal);
  float spec = pow(max(dot(direction_view, direction_reflect), 0.0f), material.shininess);
  // Combine results.
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, coords_tex));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, coords_tex));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, coords_tex));

  return (ambient + diffuse + specular);
}

void main()
{
  // Ambient.
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, coords_tex));

  vec3 direction_light = normalize(light.position - position_frag);
  float theta = dot(direction_light, normalize(-light.direction));
  float epsilon = light.cutoff - light.outercutoff;
  float intensity = clamp((theta - light.outercutoff)/epsilon, 0.0f, 1.0f);

  // Diffuse.
  vec3 normal = normalize(Normal);
  float diff = max(dot(normal, direction_light), 0.0f);
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, coords_tex));
  // Specular.
  vec3 direction_view = normalize(position_view - position_frag);
  vec3 direction_reflect = reflect(-direction_light, normal);
  float spec = pow(max(dot(direction_view, direction_reflect), 0.0f), material.shininess);
  vec3 specular = light.specular * spec * vec3(texture(material.specular, coords_tex));

  diffuse *= intensity;
  specular *= intensity;

  FragColor = vec4(ambient + diffuse + specular, 1.0f);
}
