#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "utils.h"

void
framebuffer_size_callback(
    GLFWwindow * window,
    int width,
    int height)
{
  glViewport(0, 0, width, height);
}

void
processingInput(GLFWwindow * window)
{
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

GLfloat vertices[] = {
   0.5f,  0.5f,  0.0f, // top right.
   0.5f, -0.5f,  0.0f, // bottom right.
  -0.5f, -0.5f,  0.0f, // bottom left.
  -0.5f,  0.5f,  0.0f, // top left.
};

GLuint indices[] = {
  0, 1, 3, // First triangle.
  1, 2, 3, // Second triangle.
};

int
main(void)
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow * window = glfwCreateWindow(
    800,
    600,
    "HELLO TRIANGLE",
    NULL,
    NULL
  );

  if (window == NULL) {
    fprintf(stderr, "[ERROR:] Failed to create GLFW window.\n");
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fprintf(stderr, "[ERROR:] Failed to initialize GLAD.\n");
    return -1;
  }

  glViewport(0, 0, 800, 600);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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
    return -1;
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
    return -1;
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
    return -1;
  }

  free(source_shader_vertex);
  free(source_shader_fragment);

  glDeleteShader(shader_vertex);
  glDeleteShader(shader_fragment);

  GLuint VAO = 0;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  GLuint EBO = 0;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW
  );

  GLuint VBO = 0;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);

  glUseProgram(program_shader);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  int location_vertex_color = glGetUniformLocation(program_shader, "ourColor");

  while (!glfwWindowShouldClose(window)) {
    processingInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float timeValue = glfwGetTime();
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    glUniform4f(location_vertex_color, 0.0f, greenValue, 0.0f, 1.0f);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
