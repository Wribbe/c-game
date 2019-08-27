#ifndef __UTILS_H
#define __UTILS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "linmath.h"
#include "stb_image.h"

#define M_PI (float)3.14159265358979323846264338327950288

#define UNUSED(x) (void)x

typedef GLfloat m4[4][4];
typedef GLfloat v3[3];

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

#endif
