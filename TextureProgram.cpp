#include "TextureProgram.hpp"
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

Load<TextureProgram> texture_program(LoadTagEarly, []() -> TextureProgram const * {
    return new TextureProgram();
});

TextureProgram::TextureProgram() {
	// Shader programs partially adapted from https://learnopengl.com/In-Practice/Text-Rendering
    // Compile vertex and fragment shaders using the convenient 'gl_compile_program' helper function:
    program = gl_compile_program(
        // Vertex shader:
        "#version 330 core\n"
        "layout (location = 0) in vec4 vertex;\n" // <vec2 pos, vec2 tex>
        "out vec2 TexCoords;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
        "    TexCoords = vertex.zw;\n"
        "}\n"
    ,
        // Fragment shader:
        "#version 330 core\n"
        "in vec2 TexCoords;\n"
        "out vec4 color;\n"
        "uniform sampler2D text;\n"
        "uniform vec3 textColor;\n"
        "void main() {\n"
        "    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
        "    color = vec4(textColor, 1.0) * sampled;\n"
        "}\n"
    );

    // Look up the locations of vertex attributes:
    Position_vec4 = glGetAttribLocation(program, "vertex");

    // Look up the locations of uniforms:
    projection_mat4 = glGetUniformLocation(program, "projection");
    textColor_vec3 = glGetUniformLocation(program, "textColor");
    texture_sampler2D = glGetUniformLocation(program, "text");

    // Set default texture unit for text to 0 (GL_TEXTURE0)
    glUseProgram(program);
    glUniform1i(texture_sampler2D, 0); // Set text to sample from GL_TEXTURE0
    glUseProgram(0); // Unbind program
}


TextureProgram::~TextureProgram() {
    glDeleteProgram(program);
    program = 0;
}
