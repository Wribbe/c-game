#include "utils.h"

GLchar *
read_file(const char * path_file)
{
  char * buffer = NULL;
  size_t size_string, size_read;
  FILE * handler = fopen(path_file, "r");

  if (handler) {
    fseek(handler, 0, SEEK_END);
    size_string = ftell(handler);
    rewind(handler);
    buffer = malloc(sizeof(char) * size_string+1);
    if (buffer == NULL) {
      fprintf(
        stderr,
        "[Error:] Could not malloc memory for file %s.\n",
        path_file
      );
      goto error;
    }
    size_read = fread(buffer, sizeof(char), size_string, handler);
    buffer[size_string] = '\0';
    if (size_string != size_read) {
      free(buffer);
      buffer = NULL;
      fprintf(
        stderr,
        "[Error:] Read size does not equal assumed value for: %s.\n",
        path_file
      );
      goto error;
    }
  } else {
    fprintf(stderr, "[Error:] No such file %s.\n", path_file);
    goto error;
  }

  return buffer;

error:
  if (buffer) {
    free(buffer);
    buffer = NULL;
  }
  return NULL;
}

GLuint
program_create(const char * source_vertex, const char * source_fragment)
{
  GLint success = 0;
  size_t size_buffer_log_info = 512;
  char buffer_log_info[size_buffer_log_info];

  char * source_shader_vertex = read_file("src/shaders/learnopengl.vert");

  GLuint shader_vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(
    shader_vertex, 1, (const GLchar **)&source_shader_vertex, NULL
  );

  glCompileShader(shader_vertex);
  glGetShaderiv(shader_vertex, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(
        shader_vertex, size_buffer_log_info, NULL, buffer_log_info
    );
    fprintf(
      stderr,
      "[ERROR:] Vertex shader compilation failed with:\n%s\n",
      buffer_log_info
    );
    return 0;
  }

  char * source_shader_fragment = read_file("src/shaders/learnopengl.frag");

  GLuint shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(
    shader_fragment, 1, (const GLchar **)&source_shader_fragment, NULL
  );

  glCompileShader(shader_fragment);
  glGetShaderiv(shader_fragment, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(
        shader_fragment, size_buffer_log_info, NULL, buffer_log_info
    );
    fprintf(
      stderr,
      "[ERROR:] Fragment shader compilation failed with:\n%s\n",
      buffer_log_info
    );
    return 0;
  }

  GLuint program_shader = glCreateProgram();
  glAttachShader(program_shader, shader_vertex);
  glAttachShader(program_shader, shader_fragment);
  glLinkProgram(program_shader);
  glGetProgramiv(program_shader, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(
      program_shader, size_buffer_log_info, NULL, buffer_log_info
    );
    fprintf(stderr, "[ERROR:] Could not link the shader program.\n");
    return 0;
  }

  free(source_shader_vertex);
  free(source_shader_fragment);

  glDeleteShader(shader_vertex);
  glDeleteShader(shader_fragment);

  return program_shader;
}

void
shader_set_int(GLuint id, const char * name, GLint value)
{
  glUniform1i(glGetUniformLocation(id, name), value);
}

void
shader_set_float(GLuint id, const char * name, GLfloat value)
{
  glUniform1f(glGetUniformLocation(id, name), value);
}
