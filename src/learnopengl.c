#include "utils.h"

vec3 camera_position = {0.0f, 0.0f, 3.0f};
vec3 camera_front = {0.0f, 0.0f, -1.0f};
vec3 camera_up = {0.0f, 1.0f, 0.0f};
vec3 camera_target = {0.0f, 0.0f, 0.0f};

float time_delta = 0.0f;
float time_last_frame = 0.0f;
float time_current = 0.0f;

float last_x = 400;
float last_y = 300;

float yaw = -90.0f;
float pitch = 0.0f;

GLboolean first_mouse = GL_TRUE;

GLfloat fov = 45.0f;

vec3 position_light = {1.0f, 3.0f, 0.4f};

float speed_camera_arrows = 0.8f;

void
camera_reorient(GLfloat offset_x, GLfloat offset_y);

void
callback_frabuffer_size(
    GLFWwindow * window,
    int width,
    int height)
{
  glViewport(0, 0, width, height);
}

void
processingInput(GLFWwindow * window)
{

  time_current = glfwGetTime();
  time_delta = time_current - time_last_frame;
  time_last_frame = time_current;

  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  float camera_speed = 2.5f * time_delta;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    vec3 temp = {0};
    vec3_scale(temp, camera_front, camera_speed);
    vec3_add(camera_position, camera_position, temp);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    vec3 temp = {0};
    vec3_scale(temp, camera_front, camera_speed);
    vec3_sub(camera_position, camera_position, temp);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    vec3 temp = {0};
    vec3_mul_cross(temp, camera_front, camera_up);
    vec3_norm(temp, temp);
    vec3_scale(temp, temp, camera_speed);
    vec3_sub(camera_position, camera_position, temp);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    vec3 temp = {0};
    vec3_mul_cross(temp, camera_front, camera_up);
    vec3_norm(temp, temp);
    vec3_scale(temp, temp, camera_speed);
    vec3_add(camera_position, camera_position, temp);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    camera_reorient(-speed_camera_arrows, 0.0f);
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    camera_reorient(speed_camera_arrows, 0.0f);
  }
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    camera_reorient(0.0f, speed_camera_arrows);
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    camera_reorient(0.0f, -speed_camera_arrows);
  }
}

float
to_rad(float degrees)
{
  return degrees * (M_PI/180.0f);
}

void
callback_scroll(GLFWwindow * window, double offset_x, double offset_y)
{
  if (fov >= 1.0f && fov <= 45.0f) {
    fov -= offset_y;
  } else if (fov <= 1.0f) {
    fov = 1.0f;
  } else if (fov >= 45.0f) {
    fov = 45.0f;
  }
}

void
camera_reorient(GLfloat offset_x, GLfloat offset_y)
{
  yaw += offset_x;
  pitch += offset_y;

  if (pitch > 89.0f) {
    pitch = 89.0f;
  }
  if (pitch < -89.0f) {
    pitch = -89.0f;
  }

  GLfloat rpitch = to_rad(pitch);
  GLfloat ryaw = to_rad(yaw);

  camera_front[0] = cosf(rpitch) * cosf(ryaw);
  camera_front[1] = sinf(rpitch);
  camera_front[2] = cosf(rpitch) * sinf(ryaw);

  vec3_norm(camera_front, camera_front);
}

void
callback_mouse(GLFWwindow * window, double pos_x, double pos_y)
{

  if (first_mouse) {
    last_x = pos_x;
    last_y = pos_y;
    first_mouse = GL_FALSE;
  }

  float offset_x = pos_x - last_x;
  float offset_y = last_y - pos_y;
  last_x = pos_x;
  last_y = pos_y;

  float sensitivity = 0.05f;
  offset_x *= sensitivity;
  offset_y *= sensitivity;

  camera_reorient(offset_x, offset_y);
}

GLfloat vertices[] = {
  // positions          // normals           // texture coords
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

v3 positions_cube[] = {
  { 0.0f,  0.0f,  0.0f},
  { 2.0f,  5.0f, -15.0f},
  {-1.5f, -2.2f, -2.5f},
  {-3.8f, -2.0f, -12.3f},
  { 2.4f, -0.4f, -3.5f},
  {-1.7f,  3.0f, -7.5f},
  { 1.3f, -2.0f, -2.5f},
  { 1.5f,  2.0f, -2.5f},
  { 1.5f,  0.2f, -1.5f},
  {-1.3f,  1.0f, -1.5f}
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

  glfwSetFramebufferSizeCallback(window, callback_frabuffer_size);

  GLuint program_obj = program_create(
    "src/shaders/obj.vert",
    "src/shaders/obj.frag"
  );

  GLuint VAO_obj = 0;
  glGenVertexArrays(1, &VAO_obj);
  glBindVertexArray(VAO_obj);

  GLuint VBO = 0;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(3*sizeof(GLfloat))
  );
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(6*sizeof(GLfloat))
  );
  glEnableVertexAttribArray(2);

  GLuint VAO_light = 0;
  glGenVertexArrays(1, &VAO_light);
  glBindVertexArray(VAO_light);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);

  mat4x4 model, view, projection;
  mat4x4_identity(model);
  mat4x4_identity(view);
  mat4x4_identity(projection);

  glUseProgram(program_obj);

  GLuint location_model = glGetUniformLocation(program_obj, "model");
  GLuint location_view = glGetUniformLocation(program_obj, "view");
  GLuint location_projection = glGetUniformLocation(
    program_obj, "projection"
  );

  glUseProgram(program_obj);

  vec3 color_diffuse = {1.0f, 0.5f, 0.31f};
  vec3_scale(color_diffuse, color_diffuse, 0.2f);

  shader_set_v3(program_obj, "position_view", camera_position);
  shader_set_float(program_obj, "material.shininess", 32.0f);
  shader_set_float(program_obj, "light.constant", 1.0f);
  shader_set_float(program_obj, "light.linear", 0.09f);
  shader_set_float(program_obj, "light.quadratic", 0.032f);

  shader_set_float(program_obj, "light.cutoff", cosf(to_rad(12.5f)));
  shader_set_float(program_obj, "light.outercutoff", cosf(to_rad(17.5f)));

  GLuint texture_diffuse = texture_load("res/container2.png");
  GLuint texture_specular = texture_load("res/container2_specular.png");

  glUseProgram(0);

  glEnable(GL_DEPTH_TEST);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, callback_mouse);

  glfwSetScrollCallback(window, callback_scroll);

  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();
    processingInput(window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3_add(camera_target, camera_position, camera_front);

    mat4x4_look_at(view,
      camera_position,
      camera_target,
      camera_up
    );

    mat4x4_perspective(
      projection, to_rad(fov), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT,
      0.1f, 100.0f
    );

    glUseProgram(program_obj);

    shader_set_v3(program_obj, "position_view", camera_position);

    glUniformMatrix4fv(location_view, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(location_projection, 1, GL_FALSE, &projection[0][0]);

    v3 color_light = {1.0f, 1.0f, 1.0f};
    shader_set_v3(program_obj, "light.specular", color_light);
    shader_set_v3(program_obj, "light.ambient", color_diffuse);

    vec3_scale(color_light, color_light, 0.8f);
    shader_set_v3(program_obj, "light.diffuse", color_light);

    glBindVertexArray(VAO_obj);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_specular);

    shader_set_int(program_obj, "material.diffuse", 0);
    shader_set_int(program_obj, "material.specular", 1);
    shader_set_v3(program_obj, "light.position", camera_position);
    shader_set_v3(program_obj, "light.direction", camera_front);

    for (int i=0; i<10; i++) {
      mat4x4_identity(model);
      m4_translate(model, model, positions_cube[i]);
      float angle = 20.0f * i;
      mat4x4_rotate(model, model, 1.0f, 0.3f, 0.5f, to_rad(angle));
      glUniformMatrix4fv(location_model, 1, GL_FALSE, &model[0][0]);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
