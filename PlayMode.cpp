#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"
#include "TextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "hb-ftt.hpp"
#include "GameModel.hpp"	

#include <glm/gtc/type_ptr.hpp>

#include <random>

// Part of the base code. Did not remove due to potential dependencies.
Load< Scene > hexapod_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("hexapod.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
	});
});


void PlayMode::StepForward(int choice) {
	// 1 = left, 2 = right, 3 = up
	if(currentState->hasChoices) {
		if(choice == 1) {
			currentState = currentState->choiceA;
			onNewLine = true;
		} else if(choice == 2) {
			currentState = currentState->choiceB;
			onNewLine = true;
		}
		if(currentState == nullptr) {
			currentState = setupGame();
			choice_counter = 0;
		}
		if(currentState->isIncrement) {
			choice_counter++;
			if(choice_counter == 4) {
				currentState = setupTrueEnd();
				isTrueEnd = true;
			}
		}
	} else {
		if(choice == 3) {
			currentState = currentState->nextState;
			onNewLine = true;
			if(currentState == nullptr && isTrueEnd) {
				exit(0);
			} 
			if(currentState == nullptr) {
				currentState = setupGame();
				choice_counter = 0;
			}
		}
	}
}

PlayMode::PlayMode() : scene(*hexapod_scene) {

	currentState = setupGame();
	text_renderer = TextRenderer();
	onNewLine = true;

	choice_counter = 0;

	// Below is part of the base code. Did not remove due to potential dependencies.
	//get pointers to leg for convenience:
	for (auto &transform : scene.transforms) {
		if (transform.name == "Hip.FL") hip = &transform;
		else if (transform.name == "UpperLeg.FL") upper_leg = &transform;
		else if (transform.name == "LowerLeg.FL") lower_leg = &transform;
	}
	if (hip == nullptr) throw std::runtime_error("Hip not found.");
	if (upper_leg == nullptr) throw std::runtime_error("Upper leg not found.");
	if (lower_leg == nullptr) throw std::runtime_error("Lower leg not found.");

	hip_base_rotation = hip->rotation;
	upper_leg_base_rotation = upper_leg->rotation;
	lower_leg_base_rotation = lower_leg->rotation;

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			left.downs = 0;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			right.downs = 0;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			up.downs = 0;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			down.downs = 0;
			return true;
		}
	} 

	return false;
}

void PlayMode::update(float elapsed) {

	if(left.downs == 1) {
		//std::cout << "left" << std::endl;
		StepForward(1);
	} else if(right.downs == 1) {
		//std::cout << "right" << std::endl;
		StepForward(2);
	} else if(up.downs == 1) {
		//std::cout << "up" << std::endl;
		StepForward(3);
	}

	if(onNewLine){
		onNewLine = false;

	glyphs.clear();
	
	// get text data from current state
	const char *currtext = currentState->text;

	// Code logic below partially adapted from https://learnopengl.com/In-Practice/Text-Rendering
	GlyphData glyphData = text_renderer.shapeText(currtext);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
	 // Iterate over shaped glyphs in GlyphData
    for (unsigned int i = 0; i < glyphData.glyphCount; ++i) {
        hb_codepoint_t glyphIndex = glyphData.glyphInfo[i].codepoint;

        // Load the glyph into FreeType
        if (FT_Load_Glyph(text_renderer.ft_face, glyphIndex, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph for index: " << glyphIndex << std::endl;
            continue;
        }

        // Generate texture for the glyph
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,  // Single-channel (grayscale) texture
            text_renderer.ft_face->glyph->bitmap.width,
            text_renderer.ft_face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            text_renderer.ft_face->glyph->bitmap.buffer  // Glyph bitmap data from FreeType
        );

		        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Store glyph data for later use
        Glyph glyph = {
            texture, 
            glm::ivec2(text_renderer.ft_face->glyph->bitmap.width, text_renderer.ft_face->glyph->bitmap.rows),
            glm::ivec2(text_renderer.ft_face->glyph->bitmap_left, text_renderer.ft_face->glyph->bitmap_top),
            static_cast<unsigned int>(text_renderer.ft_face->glyph->advance.x / 64)  // Convert 1/64th pixel to pixel
        };

        // Insert the glyph into the map, using Harfbuzz glyph index as the key
        this->glyphs.push_back(glyph);
    }
	
	text_renderer.destroyGlyphData(glyphData);


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	// Bind and allocate buffer data (6 vertices for a quad, each vertex has 4 floats: 2 for position, 2 for texCoords)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	// Set up vertex attributes (position and texCoords)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	}
	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Drawing code below partially adapted from https://learnopengl.com/In-Practice/Text-Rendering

    // Enable blending (for transparent text rendering):
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Activate text shader program:
    glUseProgram(texture_program->program);

	// Set text color (white, for example):
    glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glUniform3f(texture_program->textColor_vec3, textColor.x, textColor.y, textColor.z);

    // Set up the projection matrix (adjust based on your orthographic setup):
    glm::mat4 projection = glm::ortho(0.0f, float(drawable_size.x), 0.0f, float(drawable_size.y));
    glUniformMatrix4fv(texture_program->projection_mat4, 1, GL_FALSE, &projection[0][0]);

    // Bind the vertex array object (VAO) for text rendering:
    glBindVertexArray(VAO);

    // Initialize starting position for the text:
    float x = 25.0f;  // Starting x position
    float y = 100.0f; // Starting y position
    float scale = 1.0f; // Scale for the text rendering

    // Iterate over all glyphs in the glyph map and render them:
    for (auto const &glyph_entry : glyphs) {
        Glyph ch = glyph_entry;

        // Calculate the position and size of the current glyph:
        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Update VBO for each character (quad vertices for the current glyph):
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };

        // Render glyph texture over quad:
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // Update the content of VBO memory with the quad vertices:
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render the quad:
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance the cursor for the next glyph (accounting for scaling):
        x += (ch.Advance) * scale; // Bitshift by 6 to convert from 1/64th pixels to pixels
    }
	isfirst = false;

    // Unbind the VAO and texture:
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Disable blending:
    glDisable(GL_BLEND);
	GL_ERRORS();


}

// Part of the base code. Did not remove due to potential dependencies.
glm::vec3 PlayMode::get_leg_tip_position() {
	//the vertex position here was read from the model in blender:
	return lower_leg->make_local_to_world() * glm::vec4(-1.26137f, -11.861f, 0.0f, 1.0f);
}
