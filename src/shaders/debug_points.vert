#version 330 core
layout (location=0) in vec3 aPos;

out vec3 position_frag;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
  position_frag = vec3(model * vec4(aPos, 1.0f));

  gl_PointSize = 3.0f;
  gl_Position = projection * view * vec4(position_frag, 1.0);
}
