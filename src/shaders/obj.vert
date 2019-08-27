#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 position_frag;
out vec2 coords_tex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
  gl_Position = projection * view * model * vec4(aPos, 1.0f);
  position_frag = vec3(model * vec4(aPos, 1.0f));
  Normal = aNormal;
  coords_tex = aTexCoords;
}
