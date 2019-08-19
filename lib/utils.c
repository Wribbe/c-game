#include <stdlib.h>
#include <stdio.h>

#include <GLFW/glfw3.h>

GLchar *
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
