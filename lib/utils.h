#ifndef __UTILS_H
#define __UTILS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "linmath.h"
#include "stb_image.h"
#include "msg.h"
#include "cgltf.h"

#define M_PI (float)3.14159265358979323846264338327950288

#define UNUSED(x) (void)x

typedef GLfloat m4[4][4];
typedef GLfloat v3[3];

#define SIZE_BUFFER_CHAR 512
extern char BUFFER_CHAR[];

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

#endif
