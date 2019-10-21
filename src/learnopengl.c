#include "utils.h"

vec3 camera_position = {0.0f, 0.0f, 3.0f};
vec3 camera_front = {0.0f, 0.0f, -1.0f};
vec3 camera_up = {0.0f, 1.0f, 0.0f};
vec3 camera_target = {0.0f, 0.0f, 0.0f};

float time_delta = 0.0f;
float time_last_frame = 0.0f;
float time_current = 0.0f;
float Y_FLOOR = -1.0f;

float last_x = 400;
float last_y = 300;

float yaw = -90.0f;
float pitch = 0.0f;

GLboolean first_mouse = GL_TRUE;

GLfloat fov = 45.0f;

int64_t flags = {0};

vec3 position_light = {1.0f, 3.0f, 0.4f};

#define NR_POINT_LIGHTS 4

struct status_button {
  GLboolean pressed;
  GLboolean released;
  GLboolean changed;
  GLboolean processed;
};

struct bound_box {
  float p1[2];
  float p2[2];
};

struct status_button statuses_buttons[GLFW_KEY_LAST] = {0};

#define NUM_OBJECTS 256
GLuint obj_last = 0;
GLuint obj_moving = 0;
mat4x4 obj_models[NUM_OBJECTS] = {0};
struct bound_box obj_bounds[NUM_OBJECTS] = {0};
uint32_t obj_flags[NUM_OBJECTS] = {0};

GLuint vao_debug;
GLuint vbo_debug;
GLuint program_debug_points;

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

enum flag {
  SPACE_TOGGLE,
  DOWN_SHIFT,
  DOWN_C,
  DOWN_D,
  NUM_FLAGS,
};

enum flag_obj {
  FLAG_OBJ_GRAVITY,
  NUM_OBJ_FLAGS,
};

void
flags_set(enum flag flag)
{
  if (flag >= NUM_FLAGS) {
    return;
  }
  flags |= (1 << flag);
}

void
flags_clear(enum flag flag)
{
  if (flag >= NUM_FLAGS) {
    return;
  }
  flags &= ~(1 << flag);
}

void
flags_toggle(enum flag flag)
{
  if (flag >= NUM_FLAGS) {
    return;
  }
  flags ^= (1 << flag);
}

GLboolean
flags_get(enum flag flag)
{
  if (flag >= NUM_FLAGS) {
    return GL_FALSE;
  }
  int mask = 1 << flag;
  return (flags & mask) >> flag;
}

void
flag_change(GLuint key, enum flag flag) {
  if (statuses_buttons[key].changed) {
    if (statuses_buttons[key].pressed) {
      flags_set(flag);
    } else {
      flags_clear(flag);
    }
    statuses_buttons[key].changed = GL_FALSE;
    statuses_buttons[key].processed = GL_FALSE;
  }
}

GLboolean
flag_obj_get(GLuint index, enum flag_obj flag)
{
  if (flag >= NUM_OBJ_FLAGS) {
    return GL_FALSE;
  }
  int mask = 1 << flag;
  return (obj_flags[index] & mask) >> flag;
}

void
flag_obj_set(GLuint index, enum flag_obj flag)
{
  if (flag >= NUM_OBJ_FLAGS) {
    return;
  }
  obj_flags[index] |= (1 << flag);
}

void
flag_obj_toggle(GLuint index, enum flag_obj flag)
{
  if (flag >= NUM_OBJ_FLAGS) {
    return;
  }
  obj_flags[index] ^= (1 << flag);
}

void
flag_obj_clear(GLuint index, enum flag_obj flag)
{
  if (flag >= NUM_OBJ_FLAGS) {
    return;
  }
  obj_flags[index] &= ~(1 << flag);
}

GLboolean
_objs_collides(GLuint a, GLuint b)
{
  struct bound_box bound_a = obj_bounds[a];
  struct bound_box bound_b = obj_bounds[b];

  vec4 pa1 = {bound_a.p1[0], bound_a.p1[1], 0.0f, 1.0f};
  vec4 pa2 = {bound_a.p2[0], bound_a.p2[1], 0.0f, 1.0f};

  vec4 pb1 = {bound_b.p1[0], bound_b.p1[1], 0.0f, 1.0f};
  vec4 pb2 = {bound_b.p2[0], bound_b.p2[1], 0.0f, 1.0f};

  mat4x4_mul_vec4(pa1, obj_models[a], pa1);
  mat4x4_mul_vec4(pa2, obj_models[a], pa2);

  mat4x4_mul_vec4(pb1, obj_models[b], pb1);
  mat4x4_mul_vec4(pb2, obj_models[b], pb2);

//  GLfloat data_debug[] = {
//    pa1[0], pa1[1], pa1[2],
//    pa2[0], pa2[1], pa2[2],
//    pb1[0], pb1[1], pb1[2],
//    pb2[0], pb2[1], pb2[2]
//  };

  if (pa1[0] >= pb1[0] && pa1[0] <= pb2[0]) {
    printf("pa1[0]:%f pb1[0]:%f pa1[0]:%f bp2[0]:%f\n",
      pa1[0],
      pb1[0],
      pa1[0],
      pb2[0]
    );
    return GL_TRUE;
  }
  return GL_FALSE;
}

GLboolean
obj_collides(GLuint index)
{
  return GL_FALSE;
  for (int jj=0; jj<obj_last; jj++) {
    if (jj == index) {
      continue;
    }
    if (_objs_collides(index, jj)) {
      return GL_TRUE;
    }
  }
  return GL_FALSE;
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
//  float speed_camera_arrows = 60.0f * time_delta;


//  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
//    vec3 temp = {0};
//    vec3_scale(temp, camera_front, camera_speed);
//    vec3_add(camera_position, camera_position, temp);
//  }
//  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
//    vec3 temp = {0};
//    vec3_scale(temp, camera_front, camera_speed);
//    vec3_sub(camera_position, camera_position, temp);
//  }
//  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
//    vec3 temp = {0};
//    vec3_mul_cross(temp, camera_front, camera_up);
//    vec3_norm(temp, temp);
//    vec3_scale(temp, temp, camera_speed);
//    vec3_sub(camera_position, camera_position, temp);
//  }
//  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
//    vec3 temp = {0};
//    vec3_mul_cross(temp, camera_front, camera_up);
//    vec3_norm(temp, temp);
//    vec3_scale(temp, temp, camera_speed);
//    vec3_add(camera_position, camera_position, temp);
//  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera_position[1] += camera_speed;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera_position[1] -= camera_speed;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera_position[0] -= camera_speed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera_position[0] += camera_speed;
  }
//  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
//    camera_reorient(-speed_camera_arrows, 0.0f);
//  }
//  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
//    camera_reorient(speed_camera_arrows, 0.0f);
//  }
//  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
//    camera_reorient(0.0f, speed_camera_arrows);
//  }
//  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
//    camera_reorient(0.0f, -speed_camera_arrows);
//  }
  if (statuses_buttons[GLFW_KEY_SPACE].changed) {
    if (statuses_buttons[GLFW_KEY_SPACE].pressed) {
      flags_toggle(SPACE_TOGGLE);
    }
    statuses_buttons[GLFW_KEY_SPACE].changed = GL_FALSE;
  }
  if (statuses_buttons[GLFW_KEY_LEFT_SHIFT].changed) {
    if (statuses_buttons[GLFW_KEY_LEFT_SHIFT].pressed) {
      flags_set(DOWN_SHIFT);
    } else {
      flags_clear(DOWN_SHIFT);
    }
    statuses_buttons[GLFW_KEY_LEFT_SHIFT].changed = GL_FALSE;
  }
  flag_change(GLFW_KEY_C, DOWN_C);
  flag_change(GLFW_KEY_G, DOWN_D);
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
callback_keys(GLFWwindow * window,
  int key,
  int scancode,
  int action,
  int mods)
{
  UNUSED(mods);
  UNUSED(scancode);

  switch(action) {
    case GLFW_PRESS:
      statuses_buttons[key].pressed = GL_TRUE;
      statuses_buttons[key].released = GL_FALSE;
      break;
    case GLFW_RELEASE:
      statuses_buttons[key].released = GL_TRUE;
      statuses_buttons[key].pressed = GL_FALSE;
      break;
    default:
      return;
      break;
  }
  statuses_buttons[key].changed = GL_TRUE;
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

v3 positions_light_point[] = {
  { 0.7f,  0.2f,  2.0f},
  { 2.3f, -3.3f, -4.0f},
  {-4.0f,  2.0f, -12.0f},
  { 0.0f,  0.0f, -3.0f}
};

GLuint vao_text = 0;
GLuint vbo_text = 0;

int
main(void)
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

  glEnable(GL_PROGRAM_POINT_SIZE);

  GLuint program_obj = program_create(
    "src/shaders/obj.vert",
    "src/shaders/obj.frag"
  );

  GLuint program_lamp = program_create(
    "src/shaders/obj.vert",
    "src/shaders/light.frag"
  );

  program_debug_points = program_create(
    "src/shaders/debug_points.vert",
    "src/shaders/debug_points.frag"
  );

  cgltf_options options = {0};
  cgltf_data * data = NULL;
  cgltf_result result = {0};

  const char * path_model = RES("BoxTextured");
  result = cgltf_parse_file(&options, path_model, &data);
  if (result != cgltf_result_success) {
    ERROR("Could not parse file: %s\n", path_model);
    return -1;
  }
  result = cgltf_load_buffers(&options, data, path_model);
  if (result != cgltf_result_success) {
    ERROR("Could not load buffers from %s\n", path_model);
    return -1;
  }

  GLuint vao_gltf_cube = 0;
  glGenVertexArrays(1, &vao_gltf_cube);
  glBindVertexArray(vao_gltf_cube);

  cgltf_primitive * primitive = &data->meshes->primitives[0];

  size_t size_total = 0;
  for (int i=0; i<primitive->attributes_count; i++) {
    size_total += primitive->attributes[i].data->buffer_view->stride;
  }
  cgltf_size stride = size_total;
  size_total *= primitive->indices->count;

  float * buffer_interleaved = malloc(size_total * sizeof(float));
  memset(buffer_interleaved, 0, size_total*sizeof(float));

  GLuint vbo_gltf_cube = 0;
  glGenBuffers(1, &vbo_gltf_cube);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_gltf_cube);

  float * attribs_data[primitive->attributes_count];
  size_t offset = 0;
  for (int i=0; i<primitive->attributes_count; i++) {
    cgltf_attribute * attribute = &primitive->attributes[i];
    attribs_data[i] = (float *)ptr_gltf_data(attribute->data);
    GLuint index_attribute = index_attribute_get(attribute);
    glVertexAttribPointer(index_attribute,
      num_elements_get(attribute),
      GL_FLOAT,
      GL_FALSE,
      stride,
      (void *)offset
    );
    glEnableVertexAttribArray(index_attribute);
    offset += primitive->attributes[i].data->stride;
  }

  unsigned short * ptr_indices = (unsigned short*)ptr_gltf_data(primitive->indices);
  float * ptr_buffer = buffer_interleaved;
  for (size_t i=0; i<primitive->indices->count; i++) {
    for (int j=0; j<primitive->attributes_count; j++) {
      cgltf_attribute * attribute = &primitive->attributes[j];
      cgltf_size num_elements = num_elements_get(attribute);
      for (cgltf_size k=0; k<num_elements; k++) {
        *ptr_buffer++ = attribs_data[j][ptr_indices[i]*num_elements+k];
      }
    }
  }

  glBufferData(
    GL_ARRAY_BUFFER,
    size_total,
    buffer_interleaved,
    GL_STATIC_DRAW
  );
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

//  GLuint VAO_light = 0;
//  glGenVertexArrays(1, &VAO_light);
//  glBindVertexArray(VAO_light);
//  glBindBuffer(GL_ARRAY_BUFFER, vbo_gltf_cube);
//  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
//  glEnableVertexAttribArray(0);

  mat4x4 model, view, projection;
  mat4x4_identity(model);
  mat4x4_identity(view);
  mat4x4_identity(projection);

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

  shader_set_v3(program_obj, "light_directional.direction", (v3){-0.2f, -1.0f, -0.3f});
  shader_set_v3(program_obj, "light_directional.ambient", (v3){0.05f, 0.05f, 0.05f});
  shader_set_v3(program_obj, "light_directional.diffuse", (v3){0.4f, 0.4f, 0.4f});
  shader_set_v3(program_obj, "light_directional.specular", (v3){0.5f, 0.5f, 0.5f});

  for (int i=0; i<NR_POINT_LIGHTS; i++) {
    shader_set_v3(program_obj,
      uniformf("lights_point[%d].position", i),
      positions_light_point[i]
    );
    shader_set_v3(program_obj,
      uniformf("lights_point[%d].ambient", i),
      (v3){0.05f, 0.05f, 0.05f}
    );
    shader_set_v3(program_obj,
      uniformf("lights_point[%d].diffuse", i),
      (v3){0.8f, 0.8f, 0.8f}
    );
    shader_set_v3(program_obj,
      uniformf("lights_point[%d].specular", i),
      (v3){1.0f, 1.0f, 1.0f}
    );
    shader_set_float(program_obj, uniformf("lights_point[%d].constant", i), 1.0f);
    shader_set_float(program_obj, uniformf("lights_point[%d].linear", i), 0.09f);
    shader_set_float(program_obj, uniformf("lights_point[%d].quadratic", i), 0.032f);
  }

  glUseProgram(0);

  glEnable(GL_DEPTH_TEST);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//  glfwSetCursorPosCallback(window, callback_mouse);

  glfwSetScrollCallback(window, callback_scroll);
  glfwSetKeyCallback(window, callback_keys);

  if (!init_utils()) {
    fprintf(stderr, "[Error]: Could not initialize utils.\n");
    return -1;
  }

  GLuint program_text = program_create(
    "src/shaders/text.vert",
    "src/shaders/text.frag"
  );

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

/*
  jsmn_parser parser;
  const char * s = \
    "{\
      name: Jack, \
      age : 27\
      nested : { \
        nested_name: Teeeeeest, \
        array: [1, 2, 3, 4], \
      } \
    }";
  jsmn_init(&parser);
  int num_tokens = jsmn_parse(&parser, s, strlen(s), NULL, 1);
  jsmn_init(&parser);
  jsmntok_t t[num_tokens];
  jsmn_parse(&parser, s, strlen(s), t, num_tokens);
  for (int i=1; i<num_tokens; i++) {
    printf("size of child %d: %d.\n", i, t[i].size);
    printf("first char: %c\n", s[t[i].start]);
  }
*/
  struct timespec time_current = {0};
  struct timespec time_prev = {0};

  timespec_get(&time_prev, TIME_UTC);

  size_t size_buffer_char = 512;
  char buffer_char[size_buffer_char];

  glfwSwapInterval(-1);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

//  for (int i=0; i<10; i++) {
//    object_current->vao = vao_gltf_cube;
//    object_current->program = program_obj;
//    mat4x4_identity(object_current->model);
//    m4_translate(
//      object_current->model,
//      object_current->model,
//      positions_cube[i]
//    );
//    object_current++;
//  }
//  object_current--;

  srand(time(NULL));
  rand(); // Skip first value.

  for (int jj=0; jj<NUM_OBJECTS; jj++) {
    mat4x4_identity(obj_models[jj]);
    flag_obj_set(jj, FLAG_OBJ_GRAVITY);
    obj_bounds[jj].p1[0] = -1.0f;
    obj_bounds[jj].p1[1] = -1.0f;
    obj_bounds[jj].p2[0] =  1.0f;
    obj_bounds[jj].p2[1] =  1.0f;
  }

//  glGenVertexArrays(1, &vao_debug);
//  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
//  glEnableVertexAttribArray(0);
//  glBindVertexArray(vao_debug);

  GLfloat data_debug[] = {
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
  };

  glGenBuffers(1, &vbo_debug);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_debug);
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(data_debug),
    data_debug,
    GL_DYNAMIC_DRAW
  );

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);


  GLuint vao_basic = 0;
  GLuint vbo_basic = 0;

  GLuint program_basic = program_create(
    "src/shaders/basic.vert",
    "src/shaders/basic.frag"
  );

  GLfloat data_basic[] = {
   -0.5f, 0.5f, 0.0f,
   -0.5f,-0.5f, 0.0f,
    0.5f,-0.5f, 0.0f,
  };

  glGenVertexArrays(1, &vao_basic);
  glGenBuffers(1, &vbo_basic);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_debug);
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(data_basic),
    data_basic,
    GL_STATIC_DRAW
  );

  glBindVertexArray(vao_basic);
  glVertexAttribPointer(
    0,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    (void*)0
  );
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

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

    if (flags_get(SPACE_TOGGLE)) {
      if (obj_moving == obj_last && obj_last<NUM_OBJECTS) {
        obj_last++;
      }
      obj_models[obj_moving][3][0] = camera_position[0];
      obj_models[obj_moving][3][1] = camera_position[1];
      obj_models[obj_moving][3][2] = -3.0f;
    } else {
      if (obj_moving != obj_last) {
        obj_moving = obj_last;
      }
    }

    for (int jj=0; jj<obj_last; jj++) {
      if (flag_obj_get(jj, FLAG_OBJ_GRAVITY)) {
        obj_models[jj][3][1] -= time_delta * 1.3f;
      }
      if (obj_models[jj][3][1] < Y_FLOOR || obj_collides(jj)) {
        flag_obj_clear(jj, FLAG_OBJ_GRAVITY);
      }
    }

//    if (flags_get(DOWN_C) && !statuses_buttons[GLFW_KEY_C].processed) {
//      if (obj_last < NUM_OBJECTS) {
//        obj_last++;
//        obj_moving = obj_last;
//      }
//      statuses_buttons[GLFW_KEY_C].processed = GL_TRUE;
//    }
    if (flags_get(DOWN_D) && !statuses_buttons[GLFW_KEY_G].processed) {
      if (obj_last > 0) {
        obj_last--;
        obj_moving = obj_last;
      }
      statuses_buttons[GLFW_KEY_G].processed = GL_TRUE;
    }

    glUseProgram(program_obj);

    shader_set_v3(program_obj, "position_view", camera_position);

    shader_set_m4(program_obj, "view", view);
    shader_set_m4(program_obj, "projection", projection);

    v3 color_light = {1.0f, 1.0f, 1.0f};
    shader_set_v3(program_obj, "light.specular", color_light);
    shader_set_v3(program_obj, "light.ambient", color_diffuse);

    vec3_scale(color_light, color_light, 0.8f);
    shader_set_v3(program_obj, "light.diffuse", color_light);

    glBindVertexArray(vao_gltf_cube);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_specular);

    shader_set_int(program_obj, "material.diffuse", 0);
    shader_set_int(program_obj, "material.specular", 1);
    shader_set_v3(program_obj, "light.position", camera_position);
    shader_set_v3(program_obj, "light.direction", camera_front);

    //for (int i=0; i<10; i++) {
    //  mat4x4_identity(model);
    //  m4_translate(model, model, positions_cube[i]);
    //  float angle = 20.0f * i;
    //  mat4x4_rotate(model, model, 1.0f, 0.3f, 0.5f, to_rad(angle));
    //  shader_set_m4(program_obj, "model", model);
    //  glDrawArrays(GL_TRIANGLES, 0, 36);
    //}

    GLuint program_current = program_obj;
    glUseProgram(program_current);
    shader_set_m4(program_current, "view", view);
    shader_set_m4(program_current, "projection", projection);
//    glBindVertexArray(vao_gltf_cube);
    for (int jj=0; jj < obj_last; jj++) {
//      mat4x4_identity(model);
//      m4_translate(model, model, positions_cube[i]);
//      float angle = 20.0f * i++;
//      mat4x4_rotate(model, model, 1.0f, 0.3f, 0.5f, to_rad(angle));
      //shader_set_m4(program_lamp, "model", obj_models[jj]);
      shader_set_m4(program_current, "model", obj_models[jj]);
      //glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glUseProgram(0);
    glBindVertexArray(0);

//    for (struct object * obj = objects; obj != object_current; obj++) {
//
//      glBindVertexArray(obj->vao);
//      glUseProgram(obj->program);
//
//      shader_set_int(obj->program, "material.diffuse", 0);
//      shader_set_int(obj->program, "material.specular", 1);
//      shader_set_v3(obj->program, "light.position", camera_position);
//      shader_set_v3(obj->program, "light.direction", camera_front);
//      shader_set_v3(obj->program, "position_view", camera_position);
//      shader_set_m4(obj->program, "view", view);
//      shader_set_m4(obj->program, "projection", projection);
//      v3 color_light = {1.0f, 1.0f, 1.0f};
//      shader_set_v3(obj->program, "light.specular", color_light);
//      shader_set_v3(obj->program, "light.ambient", color_diffuse);
//      vec3_scale(color_light, color_light, 0.8f);
//      shader_set_v3(obj->program, "light.diffuse", color_light);
//
//      glActiveTexture(GL_TEXTURE0);
//      glBindTexture(GL_TEXTURE_2D, texture_diffuse);
//      glActiveTexture(GL_TEXTURE1);
//      glBindTexture(GL_TEXTURE_2D, texture_specular);
//
//      glDrawArrays(GL_TRIANGLES, 0, 36);
//      glBindVertexArray(0);
//      glUseProgram(0);
//
//    }

    glUseProgram(program_lamp);
    glBindVertexArray(vao_gltf_cube);

    shader_set_m4(program_lamp, "view", view);
    shader_set_m4(program_lamp, "projection", projection);

    for (int i=0; i<NR_POINT_LIGHTS; i++) {
      mat4x4_identity(model);
      m4_scale(model, model, 0.2f);
      m4_translate(model, model, positions_light_point[i]);
      shader_set_m4(program_lamp, "model", model);
      //glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    timespec_get(&time_current, TIME_UTC);
    float ms_frame = \
      ((time_current.tv_sec - time_prev.tv_sec)*1e3) +
      ((time_current.tv_nsec - time_prev.tv_nsec)/1e6);

    time_prev.tv_sec = time_current.tv_sec;
    time_prev.tv_nsec = time_current.tv_nsec;

    snprintf(buffer_char, size_buffer_char, "ms-frame: %.2fms", ms_frame);
    render_ui(
      program_text,
      buffer_char,
      (vec2f){{{0.75f, 0.95f}}},
      0.001f,
      (vec3){1.0f, 0.0f, 0.0f}
    );

    snprintf(buffer_char,
      size_buffer_char,
      "Space toggle: %s",
      flags_get(SPACE_TOGGLE) ? "ON" : "OFF"
    );
    render_ui(
      program_text,
      buffer_char,
      (vec2f){{{0.75f, 0.90f}}},
      0.001f,
      (vec3){1.0f, 0.0f, 0.0f}
    );

    snprintf(buffer_char,
      size_buffer_char,
      "Shift down: %s",
      flags_get(DOWN_SHIFT) ? "ON" : "OFF"
    );
    render_ui(
      program_text,
      buffer_char,
      (vec2f){{{0.75f, 0.85f}}},
      0.001f,
      (vec3){1.0f, 0.0f, 0.0f}
    );

    snprintf(buffer_char,
      size_buffer_char,
      "C down: %s",
      flags_get(DOWN_C) ? "ON" : "OFF"
    );
    render_ui(
      program_text,
      buffer_char,
      (vec2f){{{0.75f, 0.80f}}},
      0.001f,
      (vec3){1.0f, 0.0f, 0.0f}
    );

    snprintf(buffer_char,
      size_buffer_char,
      "C down: %s",
      flags_get(DOWN_C) ? "ON" : "OFF"
    );

    render_ui(
      program_text,
      buffer_char,
      (vec2f){{{0.75f, 0.80f}}},
      0.001f,
      (vec3){1.0f, 0.0f, 0.0f}
    );

    snprintf(buffer_char,
      size_buffer_char,
      "Num: %d",
      obj_last
    );

    render_ui(
      program_text,
      buffer_char,
      (vec2f){{{0.75f, 0.75f}}},
      0.001f,
      (vec3){1.0f, 0.0f, 0.0f}
    );

//    glUseProgram(program_debug_points);
//    glBindVertexArray(vao_debug);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo_debug);
//
//    shader_set_m4(program_debug_points, "view", view);
//    shader_set_m4(program_debug_points, "projection", projection);
//
//    mat4x4_identity(model);
//
//    //for (int jj=0; jj<obj_last; jj++) {
//    for (int jj=0; jj<5; jj++) {
//      printf("%d\n", jj);
//      glBufferSubData(
//        GL_ARRAY_BUFFER,
//        0,
//        sizeof(data_debug),
//        data_debug
//      );
//      shader_set_m4(program_debug_points, "model", model);
//      glDrawArrays(GL_POINTS, 0, 3*4);
//    }
//
    program_current = program_debug_points;
//    program_current = program_lamp;
    glUseProgram(program_current);
//    glBindVertexArray(vao_gltf_cube);

    shader_set_m4(program_current, "view", view);
    shader_set_m4(program_current, "projection", projection);


    GLuint vao_test = 0;
    glGenVertexArrays(1, &vao_test);
    glBindVertexArray(vao_test);
    glVertexAttribPointer(
      0,
      3,
      GL_FLOAT,
      GL_FALSE,
      3*sizeof(GLfloat),
      (void*)0
    );
    glEnableVertexAttribArray(0);

 //   GLuint vbo_test = 0;
 //   glGenBuffers(1, &vbo_test);
 //   glBindBuffer(GL_ARRAY_BUFFER, vbo_test);
 //   glBufferData(
 //     GL_ARRAY_BUFFER,
 //     sizeof(GLfloat)*3,
 //     NULL,
 //     GL_DYNAMIC_DRAW
 //   );

//    glDrawArrays(GL_POINTS, 0, 9);
//    for (float ii=-1.0; ii<1; ii += 0.1f) {
//      for (float jj=-1.0; jj<1; jj += 0.1f) {
//        for (float kk=-1.0; kk<1; kk += 0.1f) {
//          //GLfloat data[] = {ii/10.0f, jj/10.0f, kk/10.0f};
//          GLfloat data[] = {-ii, -jj, -kk};
          //printf("%f, %f, %f\n", ii/100.0f, jj/100.0f, kk/100.0f);
//          glBufferSubData(
//            GL_ARRAY_BUFFER,
//            0,
//            sizeof(data),
//            data
//          );
//          glDrawArrays(GL_POINTS, 0, 9);
//        }
//      }
//    }

    glUseProgram(program_basic);
    glBindVertexArray(vao_basic);
    glDrawArrays(GL_TRIANGLES, 0, 9);
    glBindVertexArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
