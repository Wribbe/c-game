#ifndef __UTILS_H
#define __UTILS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "linmath.h"
#include "stb_image.h"

#define M_PI 3.14159265358979323846264338327950288

char *
read_file(const char * path_file);

void
shader_set_int(GLuint id, const char * name, GLint value);

void
shader_set_float(GLuint id, const char * name, GLfloat value);

GLuint
program_create(const char * source_vertex, const char * source_fragment);

#endif
