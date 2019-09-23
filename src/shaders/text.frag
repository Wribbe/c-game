#version 330 core
in vec2 coord_tex;
out vec4 color;

uniform sampler2D text;
uniform vec3 color_text;

void
main()
{
  vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(text, coord_tex).r);
  // color = vec4(color_text, 1.0) * sampled;
  color = vec4(1.0f);
}
