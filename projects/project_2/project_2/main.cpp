/**
* Author: Anwar Benmallouk
* Assignment: Pong Clone
* Date due: 2024-03-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_GLEXT_PROTOTYPES 1
#define GL_SILENCE_DEPRECATION
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

#include <ctime>
#include "cmath"


const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED     = 255.f,
            BG_BLUE    = 255.f,
            BG_GREEN   = 255.f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X      = 0,
          VIEWPORT_Y      = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char  V_SHADER_PATH[]          = "shaders/vertex_textured.glsl",
            F_SHADER_PATH[]          = "shaders/fragment_textured.glsl",
            FONT_SPRITE_FILEPATH[]   = "font1.png",
            BALL_SPRITE_FILEPATH[] = "ball.png",
            PADDLE_SPRITE_FILEPATH[]   = "paddle.png";


const float TRIANGLE_RED     = 1.0,
            TRIANGLE_BLUE    = 0.4,
            TRIANGLE_GREEN   = 0.4,
            TRIANGLE_OPACITY = 1.0;

const float MILLISECONDS_IN_SECOND     = 1000.0;
const float MINIMUM_COLLISION_DISTANCE = 1.0f;

const int   NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL    = 0;
const GLint TEXTURE_BORDER     = 0;

SDL_Window* g_display_window;
bool  g_game_is_running = true;
float g_previous_ticks  = 0.0f;

ShaderProgram g_shader_program;
glm::mat4     g_view_matrix,
              g_left_paddle_model_matrix, g_right_paddle_model_matrix, g_ball_model_matrix,
              g_projection_matrix,
              g_other_model_matrix;

GLuint g_left_paddle_texture_id,
       g_right_paddle_texture_id,
       g_ball_texture_id,
       g_paddle_texture_id,
       g_text_texture_id;

// Global variables for left paddle
glm::vec3 g_left_paddle_position = glm::vec3(-4.0f, 0.0f, 0.0f);
glm::vec3 g_left_paddle_movement = glm::vec3(0.0f, 1.0f, 0.0f), g_recent_left_paddle_movement = glm::vec3(0.0f, 1.0f, 0.0f);
float g_left_paddle_speed = 3.0f;

// Global variables for right paddle
glm::vec3 g_right_paddle_position = glm::vec3(4.0f, 0.0f, 0.0f);
glm::vec3 g_right_paddle_movement = glm::vec3(0.0f, 0.0f, 0.0f);
float g_right_paddle_speed = 3.0f;

// Global variables for ball
glm::vec3 g_ball_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_movement = glm::vec3(0.3f, 0.3f, 0.0f);
float g_ball_speed = 10.0f;

bool g_left_paddle_player_controlled = false, g_left_paddle_won = false, g_right_paddle_won = false, g_game_over = false;


GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);

    return textureID;
}


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Pong 2.0!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_left_paddle_model_matrix = glm::mat4(1.0f);
    g_right_paddle_model_matrix = glm::mat4(1.0f);
    g_ball_model_matrix = glm::mat4(1.0f);

    g_view_matrix       = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);


    g_ball_texture_id = load_texture(BALL_SPRITE_FILEPATH);
    g_paddle_texture_id = load_texture(PADDLE_SPRITE_FILEPATH);
    g_text_texture_id = load_texture(FONT_SPRITE_FILEPATH);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    g_shader_program.set_colour(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);

    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

const int FONTBANK_SIZE = 16;

void draw_text(ShaderProgram *program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for each character
    // Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their position
        //    relative to the whole sentence)
        int spritesheet_index = (int) text[i];  // ascii value of character
        float offset = (screen_size + spacing) * i;
        
        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float) (spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float) (spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
        });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
        });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    
    g_shader_program.set_model_matrix(model_matrix);
    glUseProgram(g_shader_program.get_program_id());
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int) (text.size() * 6));
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
}


void process_input()
{
    if (g_left_paddle_player_controlled){
        g_left_paddle_movement = glm::vec3(0.0f);
    }
    g_right_paddle_movement = glm::vec3(0.0f);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                        g_game_is_running = false;
                        break;
                    case SDLK_t:
                        if (g_left_paddle_player_controlled){
                            g_left_paddle_player_controlled = false;
                        }else{
                            g_left_paddle_player_controlled = true;
                        }
                        break;
                    default:
                        break;
                }

            default:
                break;
        }
    }

    const Uint8 *key_state = SDL_GetKeyboardState(NULL);


    if (key_state[SDL_SCANCODE_UP] && g_right_paddle_position.y < 3.75)
    {
        g_right_paddle_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_DOWN] && g_right_paddle_position.y > -3.75)
    {
        g_right_paddle_movement.y = -1.0f;
    }
    if (key_state[SDL_SCANCODE_W] && g_left_paddle_position.y < 3.75 && g_left_paddle_player_controlled)
    {
        g_left_paddle_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_S] && g_left_paddle_position.y > -3.75 && g_left_paddle_player_controlled)
    {
        g_left_paddle_movement.y = -1.0f;
    }

    if (glm::length(g_left_paddle_movement) > 1.0f)
    {
        g_left_paddle_movement = glm::normalize(g_left_paddle_movement);
    }
    if (glm::length(g_right_paddle_movement) > 1.0f)
    {
        g_right_paddle_movement = glm::normalize(g_right_paddle_movement);
    }
}


void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    
    if (!g_left_paddle_player_controlled){
        g_left_paddle_movement = g_recent_left_paddle_movement;
        if (g_left_paddle_position.y < -3.75){
            g_left_paddle_movement.y = 1.0f;
            g_recent_left_paddle_movement = g_left_paddle_movement;
        }
        if (g_left_paddle_position.y > 3.75){
            g_left_paddle_movement.y = -1.0f;
            g_recent_left_paddle_movement = g_left_paddle_movement;
        }
    }
 
    // Update paddle positions
    g_left_paddle_position += g_left_paddle_movement * g_left_paddle_speed * delta_time;
    g_right_paddle_position += g_right_paddle_movement * g_right_paddle_speed * delta_time;

    // Translating the left and right paddles based on the new calculated position.
    g_left_paddle_model_matrix = glm::translate(glm::mat4(1.0f), g_left_paddle_position);
    g_right_paddle_model_matrix = glm::translate(glm::mat4(1.0f), g_right_paddle_position);

    // Scaling the square objects into rectangles about the y-axis
    g_left_paddle_model_matrix = glm::scale(g_left_paddle_model_matrix, glm::vec3(1.0f, 1.5f, 1.0f));
    g_right_paddle_model_matrix = glm::scale(g_right_paddle_model_matrix, glm::vec3(1.0f, 1.5f, 1.0f));

    // Update ball position
    g_ball_position += g_ball_movement * g_ball_speed * delta_time;
    g_ball_model_matrix = glm::translate(glm::mat4(1.0f), g_ball_position);

    // Right Paddle and Ball Collusion Handling
    float x_distance = fabs(g_right_paddle_position.x - g_ball_position.x) - ((1 + 1) / 2.0f);
    float y_distance = fabs(g_right_paddle_position.y - g_ball_position.y) - ((1 + 1.5) / 2.0f);

    if (x_distance < 0 && y_distance < 0){
        if (g_ball_movement == glm::vec3(0.3f, 0.3f, 0.0f)){
            g_ball_movement = glm::vec3(-0.3, 0.3f, 0.0f);
        }if (g_ball_movement == glm::vec3(0.3f, -0.3f, 0.0f)){
            g_ball_movement = glm::vec3(-0.3f, -0.3f, 0.0f);
        }
    }
    // Left Paddle and Ball Cullusion Handling
    x_distance = fabs(g_left_paddle_position.x - g_ball_position.x) - ((1 + 1) / 2.0f);
    y_distance = fabs(g_left_paddle_position.y - g_ball_position.y) - ((1 + 1.5) / 2.0f);
    
    if (x_distance < 0 && y_distance < 0){
        if (g_ball_movement == glm::vec3(-0.3f, -0.3f, 0.0f)){
            g_ball_movement = glm::vec3(0.3f, -0.3f, 0.0f);
        }if (g_ball_movement == glm::vec3(-0.3f, 0.3f, 0.0f)){
            g_ball_movement = glm::vec3(0.3f, 0.3f, 0.0f);
        }
    }
    
// ---- Below gives the ability for the ball to bounce off the top and bottom of the screen, by updating the movement direction of the ball
    if (g_ball_position.y > 3.75 && g_ball_movement == glm::vec3(-0.3f, 0.3f, 0.0f)){
        g_ball_movement = glm::vec3(-0.3f, -0.3f, 0.0f);
    }
    if (g_ball_position.y > 3.75 && g_ball_movement == glm::vec3(0.3f, 0.3f, 0.0f)){
        g_ball_movement = glm::vec3(0.3f, -0.3f, 0.0f);
    }
    if (g_ball_position.y < -3.75 && g_ball_movement == glm::vec3(0.3f, -0.3f, 0.0f)){
        g_ball_movement = glm::vec3(0.3f, 0.3f, 0.0f);
    }
    if (g_ball_position.y < -3.75 && g_ball_movement == glm::vec3(-0.3f, -0.3f, 0.0f)){
        g_ball_movement = glm::vec3(-0.3f, 0.3f, 0.0f);
    }
// ---------------------------------------------------------------------------------------------------
    if (g_ball_position.x > 5.00 || g_ball_position.x < -5.00){
//        g_left_paddle_won = true;
//        g_game_over = true;
        g_game_is_running = false;
    }
    
    
}


void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    // Left Paddle Object
//    g_shader_program.set_model_matrix(g_left_paddle_model_matrix);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
    draw_object(g_left_paddle_model_matrix, g_paddle_texture_id);
    // Right Paddle Object
//    g_shader_program.set_model_matrix(g_right_paddle_model_matrix);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
    draw_object(g_right_paddle_model_matrix, g_paddle_texture_id);
    // Ball Object
//    g_shader_program.set_model_matrix(g_ball_model_matrix);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
    draw_object(g_ball_model_matrix, g_ball_texture_id);
    
    
//    g_shader_program.set_model_matrix(g_model_matrix);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//
//    draw_object(g_model_matrix, g_player_texture_id);
//    draw_object(g_other_model_matrix, g_other_texture_id);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

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
        if (g_game_over){
            if (g_left_paddle_won){
                draw_text(&g_shader_program, g_text_texture_id, std::string("Game Over: Left Paddle WON!"), 0.25f, 0.0f, glm::vec3(-1.25f, 2.0f, 0.0f));
                render();
                
            }
        }
    }

    shutdown();
    return 0;
}
