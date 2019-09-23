#define STB_IMAGE_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION

#include "utils.h"

char BUFFER_CHAR[SIZE_BUFFER_CHAR] = {0};
#define NUM_CHARACTERS 128
struct character characters[NUM_CHARACTERS];

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
      ERROR("Could not malloc memory for file %s.\n", path_file);
      goto error;
    }
    size_read = fread(buffer, sizeof(char), size_string, handler);
    buffer[size_string] = '\0';
    if (size_string != size_read) {
      free(buffer);
      buffer = NULL;
      ERROR("Read size does not equal assumed value for: %s.\n", path_file);
      goto error;
    }
  } else {
    ERROR("No such file %s.\n", path_file);
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

GLuint
program_create(const char * source_vertex, const char * source_fragment)
{
  GLint success = 0;
  size_t size_buffer_log_info = 512;
  char buffer_log_info[size_buffer_log_info];

  char * source_shader_vertex = read_file(source_vertex);

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
    ERROR("Vertex shader compilation failed with:\n%s\n", buffer_log_info);
    return 0;
  }

  char * source_shader_fragment = read_file(source_fragment);

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
    ERROR("Fragment shader compilation failed with:\n%s\n", buffer_log_info);
    return 0;
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
    ERROR("Could not link the shader program.\n");
    return 0;
  }

  free(source_shader_vertex);
  free(source_shader_fragment);

  glDeleteShader(shader_vertex);
  glDeleteShader(shader_fragment);

  return program_shader;
}

void
shader_set_int(GLuint id, const char * name, GLint value)
{
  glUniform1i(glGetUniformLocation(id, name), value);
}

void
shader_set_float(GLuint id, const char * name, GLfloat value)
{
  glUniform1f(glGetUniformLocation(id, name), value);
}

void
shader_set_v3(GLuint id, const char * name, v3 value)
{
  glUniform3fv(glGetUniformLocation(id, name), 1, value);
}

void
shader_set_m4(GLuint id, const char * name, mat4x4 value)
{
  glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &value[0][0]);
}

void
m4_scale(m4 result, m4 matrix, float value)
{
  result[0][0] = matrix[0][0] * value;
  result[1][1] = matrix[1][1] * value;
  result[2][2] = matrix[2][2] * value;
  result[3][3] = 1.0f;
}

void
m4_translate(m4 result, m4 matrix, v3 vec)
{
  for (int i=0; i<3; i++) {
    result[3][i] = matrix[3][i] + vec[i];
  }
}


GLuint
texture_load(const char * path_file)
{
  int width, height, nrChannels;
  unsigned char * data = stbi_load(
    path_file,
    &width,
    &height,
    &nrChannels,
    0
  );

  if (!data) {
    ERROR("No such file for texture data: %s\n", path_file);
    return 0;
  }

  GLuint id_texture = 0;
  glGenTextures(1, &id_texture);
  glBindTexture(GL_TEXTURE_2D, id_texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);

  return id_texture;
}

char *
uniformf(const char * fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vsnprintf(BUFFER_CHAR, SIZE_BUFFER_CHAR, fmt, args);
  va_end(args);
  return BUFFER_CHAR;
}


void *
ptr_gltf_data(cgltf_accessor * accessor)
{
  uint8_t * ptr_data = (uint8_t *)accessor->buffer_view->buffer->data;
  ptr_data += accessor->offset;
  ptr_data += accessor->buffer_view->offset;
  return (void *)ptr_data;
}


GLuint
index_attribute_get(cgltf_attribute * attribute)
{
  switch(attribute->type){
    case cgltf_attribute_type_normal:
      return 1;
      break;
    case cgltf_attribute_type_texcoord:
      return 2;
      break;
    case cgltf_attribute_type_position:
      return 0;
      break;
    default:
      return 10;
      break;
  }
}

cgltf_size
num_elements_get(cgltf_attribute * attribute)
{
  switch(attribute->data->type) {
    case cgltf_type_vec3:
      return 3;
      break;
    case cgltf_type_vec2:
      return 2;
      break;
    case cgltf_type_scalar:
      return 1;
      break;
    default:
      return 0;
      break;
  }
}

void
render_text(
  GLuint program_shader,
  const char * text,
  vec2i position,
  GLfloat scale,
  vec3 color,
  mat4x4 projection)
{
  glUseProgram(program_shader);
  shader_set_v3(program_shader, "color_text", color);
  shader_set_int(program_shader, "text", 4);
  shader_set_m4(program_shader, "projection", projection);

  glActiveTexture(GL_TEXTURE4);
  glBindVertexArray(vao_text);

  for(const char * c = text; *c != '\0'; c++) {
    uint8_t ch_index = *c;
    if (ch_index >= NUM_CHARACTERS) {
      fprintf(stderr, "Character index %hu >= %d, aborting.\n",
        ch_index,
        NUM_CHARACTERS
      );
      goto cleanup;
    }
    struct character ch = characters[ch_index];
    GLfloat pos_x = position.x + ch.bearing.x * scale;
    GLfloat pos_y = position.y - (ch.size.y - ch.bearing.y) * scale;

    GLfloat w = ch.size.x * scale;
    GLfloat h = ch.size.y * scale;

    //GLfloat vertices[][4] = {
    //  { pos_x,     pos_y + h, 0.0, 0.0 },
    //  { pos_x,     pos_y,     0.0, 1.0 },
    //  { pos_x + w, pos_y,     1.0, 1.0 },

    //  { pos_x,     pos_y + h, 0.0, 0.0 },
    //  { pos_x + w, pos_y,     1.0, 1.0 },
    //  { pos_x + w, pos_y + h, 1.0, 0.0 },
    //};

    GLfloat vertices[][4] = {
      { -0.5, -0.5, 0.0, 0.0 },
      {  0.5, -0.5, 1.0, 0.0 },
      {  0.5,  0.5, 1.0, 1.0 },
    };
    UNUSED(h);
    UNUSED(w);
    UNUSED(pos_x);
    UNUSED(pos_y);

    glBindTexture(GL_TEXTURE_2D, ch.id_texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_text);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    position.x += (ch.advance >> 6) * scale;
    glUseProgram(0);
  }

cleanup:
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

int
init_utils(void)
{

  FT_Library ft = {0};
  if (FT_Init_FreeType(&ft)) {
    fprintf(stderr, "[ERROR:] Failed to initialize FreeType.\n");
    return -1;
  }

  FT_Face face = {0};
  if (FT_New_Face(ft, "res/fonts/OpenSans-Regular.ttf", 0, &face)) {
    fprintf(stderr, "[ERROR:] Failed to load OpenSans-Regular.ttf.\n");
    return -1;
  }

  FT_Set_Pixel_Sizes(face, 0, 48);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (GLubyte c=0; c<NUM_CHARACTERS; c++) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      fprintf(stderr, "[ERROR:] Failed to load glyph %c.\n", c);
      return -1;
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer
    );
    // Set texture options.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Store character for later use.
    characters[c].id_texture = texture;
    characters[c].size = (vec2i){{{
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows
    }}};
    characters[c].bearing = (vec2i){{{
      face->glyph->bitmap_left,
      face->glyph->bitmap_top
    }}};
    characters[c].advance = face->glyph->advance.x;
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  glGenVertexArrays(1, &vao_text);
  glGenBuffers(1, &vbo_text);
  glBindVertexArray(vao_text);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_text);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return 1;
}
