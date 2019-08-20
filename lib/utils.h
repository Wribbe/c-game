#ifndef __UTILS_H
#define __UTILS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "stb_image.h"

char *
read_file(const char * path_file);

void
shader_set_int(GLuint id, const char * name, GLint value);

void
shader_set_float(GLuint id, const char * name, GLfloat value);

GLuint
program_create(const char * source_vertex, const char * source_fragment);

#endif
