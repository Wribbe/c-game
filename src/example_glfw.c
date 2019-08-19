#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include "linmath.h"

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] = {
  { -0.6f, -0.4f, 1.f, 0.f, 0.f },
  {  0.6f, -0.4f, 0.f, 1.f, 0.f },
  {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

char *
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


static void
error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int
main(int argc, char** argv)
{
  GLchar * vertex_shader_text = read_file("src/shaders/example.vert");
  GLchar * fragment_shader_text = read_file("src/shaders/example.frag");

  size_t size_buffer_status = 512;
  char buffer_status[size_buffer_status];

  GLFWwindow* window;
  GLuint vertex_buffer, vertex_shader, fragment_shader, program;
  GLint mvp_location, vpos_location, vcol_location;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);
  // NOTE: OpenGL error checks have been omitted for brevity
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(
    vertex_shader,
    1,
    (const GLchar **)&vertex_shader_text,
    NULL
  );
  glCompileShader(vertex_shader);
  GLint success = 0;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(
      vertex_shader,
      size_buffer_status,
      NULL,
      buffer_status
    );
    fprintf(stderr, "[Error:] vertex shader failed: %s\n", buffer_status);
  }
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(
    fragment_shader,
    1,
    (const GLchar **)&fragment_shader_text,
    NULL
  );
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(
      fragment_shader,
      size_buffer_status,
      NULL,
      buffer_status
    );
    fprintf(stderr, "[Error:] frag shader failed: %s\n", buffer_status);
  }
  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  mvp_location = glGetUniformLocation(program, "MVP");
  vpos_location = glGetAttribLocation(program, "vPos");
  vcol_location = glGetAttribLocation(program, "vCol");
  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void*) 0);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void*) (sizeof(float) * 2));
  while (!glfwWindowShouldClose(window))
  {
    float ratio;
    int width, height;
    mat4x4 m, p, mvp;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    mat4x4_identity(m);
    mat4x4_rotate_Z(m, m, (float) glfwGetTime());
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);
    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
