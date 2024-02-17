/**
* Author: Anwar Benmallouk
* Assignment: Simple 2D Scene
* Date due: 2024-02-17, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

const int WINDOW_WIDTH  = 640*2,
          WINDOW_HEIGHT = 480*2;

const float BG_RED     = 255.0f,
            BG_BLUE    = 255.0f,
            BG_GREEN   = 255.0f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X      = 0,
          VIEWPORT_Y      = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl",
           FLAG_PATH[] = "flag.png",
           SOPHIE_PATH[] = "soph.png";

const int TRIANGLE_RED     = 1.0,
          TRIANGLE_BLUE    = 0.4,
          TRIANGLE_GREEN   = 0.4,
          TRIANGLE_OPACITY = 1.0;

const float MILLISECONDS_IN_SECOND = 1000.0;
const float DEGREES_PER_SECOND     = 90.0f;

const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL  = 0; // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER   = 0; // this value MUST be zero

SDL_Window* g_display_window;

bool g_game_is_running = true;
bool g_is_growing      = true;

ShaderProgram g_program;
glm::mat4 g_view_matrix,
          g_model_matrix, g_model_matrix_2,
          g_projection_matrix,
          g_trans_matrix;

float g_triangle_x      = 0.0f, g_triangle_x2 = 0.0f;
float g_triangle_rotate = 0.0f;
float g_previous_ticks  = 0.0f;


GLuint g_sophie_texture_id;
GLuint g_flag_texture_id; //NEW

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        LOG(filepath);
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint texture_id;
    glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return texture_id;
}


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Project 1: Simple 2D Scene",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT,
                                        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_sophie_texture_id = load_texture(SOPHIE_PATH);
    g_flag_texture_id = load_texture(FLAG_PATH); //NEW
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    g_view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.
    g_trans_matrix = g_model_matrix;
    
    g_program.set_projection_matrix(g_projection_matrix);
    g_program.set_view_matrix(g_view_matrix);
    // Notice we haven't set our model matrix yet!

    g_program.set_colour(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);

    glUseProgram(g_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_game_is_running = false;
        }
    }
}

void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
    g_previous_ticks = ticks;

    g_triangle_x += 1.0f * delta_time;
    g_triangle_x2 += 1.0f * delta_time; //NEW
    g_triangle_rotate += DEGREES_PER_SECOND * delta_time; // 90-degrees per second
    g_model_matrix = glm::mat4(1.0f);
    g_model_matrix_2 = glm::mat4(1.0f);


//  Object 1
    g_model_matrix = glm::translate(g_model_matrix, glm::vec3(g_triangle_x/10, 0.0f, 0.0f));
    g_model_matrix = glm::rotate(g_model_matrix, glm::radians(g_triangle_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
// Object 2
    g_model_matrix_2 = glm::translate(g_model_matrix, glm::vec3(0.0f, g_triangle_x2/10, 0.0f));
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
// Object 1:
    float vertices[] =
    {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };
    
    glVertexAttribPointer(g_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_program.get_position_attribute());

    // Textures
    float texture_coordinates[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };
    
    glVertexAttribPointer(g_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_program.get_tex_coordinate_attribute());
// Object 1:
    g_program.set_model_matrix(g_model_matrix);
    
    glBindTexture(GL_TEXTURE_2D, g_sophie_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
// Object 2:
    g_program.set_model_matrix(g_model_matrix_2);
    
    glBindTexture(GL_TEXTURE_2D, g_flag_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    

    glDisableVertexAttribArray(g_program.get_position_attribute());
    glDisableVertexAttribArray(g_program.get_tex_coordinate_attribute());
    
//// Object 2:
//
//    glVertexAttribPointer(g_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
//    glEnableVertexAttribArray(g_program.get_position_attribute());
//
//    glVertexAttribPointer(g_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
//    glEnableVertexAttribArray(g_program.get_tex_coordinate_attribute());
//
//    g_program.set_model_matrix(g_model_matrix_2);
//
//    glBindTexture(GL_TEXTURE_2D, g_flag_texture_id);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//
//
//    glDisableVertexAttribArray(g_program.get_position_attribute());
//    glDisableVertexAttribArray(g_program.get_tex_coordinate_attribute());

    
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
