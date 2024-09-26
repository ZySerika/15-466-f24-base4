#pragma once

#include "GL.hpp"
#include "Load.hpp"

// Shader program that draws textured vertices:
struct TextureProgram {
    TextureProgram();
    ~TextureProgram();

    GLuint program = 0;

    // Attribute (per-vertex variable) locations:
    GLuint Position_vec4 = -1U;

    // Uniform (per-invocation variable) locations:
    GLuint projection_mat4 = -1U;
    GLuint textColor_vec3 = -1U;

    // Texture Sampler:
    GLuint texture_sampler2D = -1U;

    // TEXTURE0 - texture that is accessed by TexCoords
};

extern Load<TextureProgram> texture_program;
