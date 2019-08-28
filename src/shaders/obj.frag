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

struct LightPoint {
  vec3 position;

  float constant;
  float linear;
  float quadratic;

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
#define NR_POINT_LIGHTS 4
uniform LightPoint lights_point[NR_POINT_LIGHTS];

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

vec3
calculate_light_point(
  LightPoint light,
  vec3 normal,
  vec3 position_frag,
  vec3 direction_view)
{
  vec3 direction_light = normalize(light.position - position_frag);
  // Diffuse shading.
  float diff = max(dot(normal, direction_light), 0.0f);
  // Specular shading.
  vec3 direction_reflect = reflect(-direction_light, normal);
  float spec = pow(max(dot(direction_view, direction_reflect), 0.0f), material.shininess);

  // Combine results.
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, coords_tex));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, coords_tex));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, coords_tex));

  // Attenuation.
  float dist = length(light.position - position_frag);
  float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return (ambient + diffuse + specular);
}

void main()
{
  // Properties.
  vec3 normal = normalize(Normal);
  vec3 direction_view = normalize(position_view - position_frag);

  // Phase 1: Directional lighting.
  vec3 result = calculate_light_directional(light_directional, normal,
      direction_view);

  // Phase 2: Point lights.
  for (int i=0; i<NR_POINT_LIGHTS; i++) {
    result += calculate_light_point(lights_point[i], normal, position_frag,
        direction_view);
  }

  FragColor = vec4(result, 1.0f);
}
