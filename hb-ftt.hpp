#ifndef TEXT_RENDERER_HPP
#define TEXT_RENDERER_HPP

#include <string>
#include <vector>
#include <hb.h>
#include <hb-ft.h>
#include "GL.hpp"  

// A struct to hold glyph info and positions
struct GlyphData {
    hb_glyph_info_t* glyphInfo;
    hb_glyph_position_t* glyphPos;
    unsigned int glyphCount;
    hb_buffer_t* hb_buffer; 
};

// Class that handles the text rendering logic
class TextRenderer {
public:
    // Constructor: loads the font file and sets up FreeType and Harfbuzz
    TextRenderer();

    // Destructor: cleans up FreeType and Harfbuzz resources
    ~TextRenderer();

    // Process the text and generate OpenGL textures for each glyph
    GlyphData shapeText(const char *text);

    void destroyGlyphData(GlyphData& glyphData);

    void printBitmapAsBytes(const FT_Bitmap& bitmap);
    
    FT_Library ft_library;  // FreeType library
    FT_Face ft_face;  // FreeType font face
    hb_font_t* hb_font;  // Harfbuzz font
};

#endif
