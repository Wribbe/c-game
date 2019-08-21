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
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

vec3 positions_cubes[] = {
  { 0.0f,  0.0f,   0.0f},
  { 2.0f,  5.0f, -15.0f},
  {-1.5f, -2.2f,  -2.5f},
  {-3.8f, -2.0f, -12.3f},
  { 2.4f, -0.4f,  -3.5f},
  {-1.7f,  3.0f,  -7.5f},
  { 1.3f, -2.0f,  -2.5f},
  { 1.5f,  2.0f,  -2.5f},
  { 1.5f,  0.2f,  -1.5f},
  {-1.3f,  1.0f,  -1.5f}
};

int
main(void)
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLuint SCREEN_WIDTH = 800;
  GLuint SCREEN_HEIGHT = 600;

  GLFWwindow * window = glfwCreateWindow(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    "Learnopengl",
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

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  GLuint program_shader = program_create(
    "src/shaders/learnopengl.vert",
    "src/shaders/learnopengl.frag"
  );

  GLuint VAO = 0;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  GLuint VBO = 0;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(
      1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void *)(3*sizeof(float))
  );
  glEnableVertexAttribArray(1);

  glUseProgram(program_shader);

  int width, height, nrChannels;

  const char * res_texture = NULL;
  unsigned char * data = NULL;

  stbi_set_flip_vertically_on_load(GL_TRUE);

  GLuint texture1 = 0;
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  res_texture = "res/container.jpg";
  data = stbi_load(
    res_texture, &width, &height, &nrChannels, 0
  );
  if (data) {
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB, width, height,
      0, GL_RGB, GL_UNSIGNED_BYTE, data
    );
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    fprintf(stderr, "Could not create texture from %s.\n", res_texture);
    return -1;
  }
  stbi_image_free(data);

  GLuint texture2 = 0;
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  res_texture = "res/awesomeface.png";
  data = stbi_load(
    res_texture, &width, &height, &nrChannels, 0
  );
  if (data) {
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGBA, width, height,
      0, GL_RGBA, GL_UNSIGNED_BYTE, data
    );
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    fprintf(stderr, "Could not create texture from %s.\n", res_texture);
    return -1;
  }
  stbi_image_free(data);

  glUniform1i(glGetUniformLocation(program_shader, "texture1"), 0);
  glUniform1i(glGetUniformLocation(program_shader, "texture2"), 1);

  mat4x4 model, view, projection;
  mat4x4_identity(model);
  mat4x4_identity(view);
  mat4x4_identity(projection);

  mat4x4_perspective(
      projection, M_PI/2.0, (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT,
      0.1f, 100.0f
  );

  GLuint location_model = glGetUniformLocation(program_shader, "model");
  GLuint location_view = glGetUniformLocation(program_shader, "view");
  GLuint location_projection = glGetUniformLocation(
    program_shader, "projection"
  );

  glEnable(GL_DEPTH_TEST);

  vec3 camera_position = {0.0f, 0.0f, 3.0f};
  vec3 camera_target = {0.0f, 0.0f, 0.0f};
  vec3 camera_direction = {0};
  vec3_sub(camera_direction, camera_position, camera_target);
  vec3_norm(camera_position, camera_position);

  vec3 direction_up = {0.0, 1.0f, 0.0f};
  vec3 camera_right = {0};
  vec3_mul_cross(camera_right, direction_up, camera_direction);
  vec3_norm(camera_right, camera_right);

  vec3 camera_up = {0};
  vec3_mul_cross(camera_up, camera_direction, camera_right);

  glUniformMatrix4fv(location_projection, 1, GL_FALSE, &projection[0][0]);

  while (!glfwWindowShouldClose(window)) {
    processingInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    float radius = 10.0f;
    float cam_x = sinf(glfwGetTime()) * radius;
    float cam_z = cosf(glfwGetTime()) * radius;

    camera_position[0] = cam_x;
    camera_position[1] = 0.0f;
    camera_position[2] = cam_z;

    camera_target[0] = 0.0f;
    camera_target[1] = 0.0f;
    camera_target[2] = 0.0f;

    camera_up[0] = 0.0f;
    camera_up[1] = 1.0f;
    camera_up[2] = 0.0f;

    mat4x4_look_at(view,
      camera_position,
      camera_target,
      camera_up
    );

    glUniformMatrix4fv(location_view, 1, GL_FALSE, &view[0][0]);

    glBindVertexArray(VAO);
    for (int i=0; i<10; i++) {
      mat4x4_identity(model);
      mat4x4_translate(
        model,
        positions_cubes[i][0],
        positions_cubes[i][1],
        positions_cubes[i][2]
      );
      float angle = 0.45f * i;
      mat4x4_rotate(model, model, 1.0f, 0.3f, 0.5f, angle);
      glUniformMatrix4fv(location_model, 1, GL_FALSE, &model[0][0]);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
