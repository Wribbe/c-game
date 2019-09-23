#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 coord_tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void
main()
{
  coord_tex = vertex.zw;
  vec3 position_frag = vec3(model * vec4(vertex.xy, 0.0f, 1.0f));
  gl_Position = projection * view * vec4(position_frag, 1.0);
}
