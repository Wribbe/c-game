#version 330 core

in vec3 color;
out vec4 color_frag;

void main()
{
  color_frag = vec4(color, 1.0);
}
