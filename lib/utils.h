#ifndef __UTILS_H
#define __UTILS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "linmath.h"
#include "stb_image.h"
#include "msg.h"
#include "ft2build.h"
#include FT_FREETYPE_H

#define JSMN_HEADER
#include "jsmn.h"
#include "cgltf.h"

#define M_PI (float)3.14159265358979323846264338327950288

#define UNUSED(x) (void)x

typedef GLfloat m4[4][4];
typedef GLfloat v3[3];

typedef struct {
  union {
    int32_t a[2];
    struct {
      int32_t x;
      int32_t y;
    };
  };
} vec2i;

typedef struct {
  union {
    float a[2];
    struct {
      float x;
      float y;
    };
  };
} vec2f;

#define SIZE_BUFFER_CHAR 512
#define NUM_CHARACTERS 128
extern char BUFFER_CHAR[];

struct character {
  GLuint id_texture;
  vec2i size;
  vec2i bearing;
  GLuint advance;
};

extern struct character characters[];
extern GLuint SCREEN_WIDTH;
extern GLuint SCREEN_HEIGHT;
extern GLfloat SCREEN_ASPECT;


#define PATH_MODELS_SAMPLE "res/samples/2.0"
#define RES(x) PATH_MODELS_SAMPLE"/"x"/glTF/"x".gltf"


char *
uniformf(const char * fmt, ...);

char *
read_file(const char * path_file);

void
shader_set_int(GLuint id, const char * name, GLint value);

void
shader_set_float(GLuint id, const char * name, GLfloat value);

void
shader_set_v3(GLuint id, const char * name, v3 value);

void
shader_set_m4(GLuint id, const char * name, mat4x4 value);

GLuint
program_create(const char * source_vertex, const char * source_fragment);

void
m4_scale(m4 result, m4 matrix, float value);

void
m4_translate(m4 result, m4 matrix, v3 vec);

GLuint
texture_load(const char * path_file);

void *
ptr_gltf_data(cgltf_accessor * accessor);

GLuint
index_attribute_get(cgltf_attribute * attribute);

cgltf_size
num_elements_get(cgltf_attribute * attribute);

extern GLuint vao_text;
extern GLuint vbo_text;

void
render_text(
  GLuint program_shader,
  const char * text,
  vec2f position,
  GLfloat scale,
  vec3 color,
  mat4x4 projection,
  mat4x4 view
);

void
render_ui(
  GLuint program_shader,
  const char * text,
  vec2f position,
  GLfloat scale,
  vec3 color
);

int
init_utils(void);
#endif
