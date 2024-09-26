#include "hb-ftt.hpp"
#include <iostream>

#define FONT_SIZE 36
#define MARGIN (FONT_SIZE * .5)


// Printing code Queried from ChatGPT
void printBitmap(const FT_Bitmap& bitmap) {
    std::cout << "Glyph Bitmap (" << bitmap.width << "x" << bitmap.rows << "):" << std::endl;
    for (unsigned int y = 0; y < bitmap.rows; y++) {
        for (unsigned int x = 0; x < bitmap.width; x++) {
            std::cout << (bitmap.buffer[y * bitmap.pitch + x] ? '#' : ' ');
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Printing code Queried from ChatGPT
void TextRenderer::printBitmapAsBytes(const FT_Bitmap& bitmap) {
    std::cout << "Glyph Bitmap Bytes (" << bitmap.width << "x" << bitmap.rows << "):" << std::endl;
    for (unsigned int y = 0; y < bitmap.rows; y++) {
        for (unsigned int x = 0; x < bitmap.width; x++) {
            // Print the raw byte at position (y * bitmap.pitch + x)
            // You can print this as hex or decimal based on your preference.
            unsigned char byte = bitmap.buffer[y * bitmap.pitch + x];
            // Print as hexadecimal (two digits)
            std::cout << std::hex << (int)byte << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::dec;  // Switch back to decimal format for other output
}

// Constructor: Initializes FreeType and Harfbuzz
TextRenderer::TextRenderer() {
}

// Destructor: Cleans up FreeType and Harfbuzz resources
TextRenderer::~TextRenderer() {
}

// Renders text and generates glyph textures
GlyphData TextRenderer::shapeText(const char *text) {
    // adapted from https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c

    if (FT_Init_FreeType(&ft_library)) {
        std::cerr << "Failed to initialize FreeType!" << std::endl;
        throw std::runtime_error("FreeType initialization error");
    }

    if (FT_New_Face(ft_library, "OpenSans.ttf", 0, &ft_face)) {
        std::cerr << "Failed to load font: " << std::endl;
        throw std::runtime_error("Font loading error");
    }

    if (FT_Set_Char_Size(ft_face, FONT_SIZE * 64, FONT_SIZE * 64, 0, 0)) {
        std::cerr << "Failed to set font size!" << std::endl;
        throw std::runtime_error("Font size error");
    }

    hb_font_t *hb_font = hb_ft_font_create(ft_face, NULL);

    if (!hb_font) {
    std::cerr << "Failed to create Harfbuzz font!" << std::endl;
    throw std::runtime_error("Harfbuzz font creation error");
    }

    // Create Harfbuzz buffer and shape the text
    hb_buffer_t* hb_buffer = hb_buffer_create();
    if (!hb_buffer_allocation_successful (hb_buffer)) {
    std::cerr << "Failed to create Harfbuzz buffer!" << std::endl;
    throw std::runtime_error("Harfbuzz buffer creation error");
    }
    hb_buffer_add_utf8(hb_buffer, text, -1, 0, -1);
    hb_buffer_guess_segment_properties(hb_buffer);

    hb_shape(hb_font, hb_buffer, NULL, 0);

    // Get glyph information and positions
    unsigned int glyphCount;
    hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(hb_buffer, &glyphCount);
    hb_glyph_position_t* glyphPos = hb_buffer_get_glyph_positions(hb_buffer, &glyphCount);

    // Store the information in a GlyphData struct
    GlyphData glyphData;
    glyphData.glyphInfo = glyphInfo;
    glyphData.glyphPos = glyphPos;
    glyphData.glyphCount = glyphCount;
    glyphData.hb_buffer = hb_buffer;  // Store the buffer

    return glyphData;

}

void TextRenderer::destroyGlyphData(GlyphData& glyphData) {
    hb_buffer_destroy(glyphData.hb_buffer);
    hb_font_destroy(hb_font);
    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft_library);
}

