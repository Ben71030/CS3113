#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>

#include "Entity.hpp"
#include "Map.hpp"
#include "Utility.hpp"
#include "Scene.hpp"

#include "Menu.hpp"
#include "First_Level.hpp"
#include "Second_Level.hpp"
#include "Third_Level.hpp"

#include "Win.hpp"
#include "Lose.hpp"

const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED     = 124.0f,
            BG_BLUE    = 252.0f,
            BG_GREEN   = 0.0f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_model_matrix, g_projection_matrix;

Scene *g_current_scene;
Scene *g_scenes[6];

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;


void switch_to_scene(int the_next_scene, int num_of_lives = 3){
    if (the_next_scene == 1){
        g_current_scene = new First_Level(num_of_lives);
    }
    else if (the_next_scene == 2){
        g_current_scene = new Second_Level(num_of_lives);
    }
    else if (the_next_scene == 3){
        g_current_scene = new Third_Level(num_of_lives);
    }
    else{
        g_current_scene = g_scenes[the_next_scene];
    }
    g_current_scene -> initialize();
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Escape!",
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

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    g_scenes[0] = new Menu();
    g_scenes[1] = new First_Level(3);
    g_scenes[2] = new Second_Level(3);
    g_scenes[3] = new Third_Level(3);
    g_scenes[4] = new Win();
    g_scenes[5] = new Lose();
    switch_to_scene(0);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    g_current_scene->m_state.player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running  = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_q:
                    g_game_is_running  = false;
                    break;
                        
                case SDLK_LEFT:
                    break;
                
                case SDLK_RIGHT:
                    break;
                
                case SDLK_RETURN:
                    g_current_scene->m_state.player_lives = 3;
                    switch_to_scene(1);
                    break;

                case SDLK_SPACE:
                    if (g_current_scene->m_state.player->m_collided_bottom){
                        g_current_scene->m_state.player->m_is_jumping = true;
                    }
                    break;

                default:
                    break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_current_scene->m_state.player->move_left();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_current_scene->m_state.player->move_right();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->RIGHT];
    }
    else if (key_state[SDL_SCANCODE_RETURN]){
        g_current_scene->m_state.player_lives = 3;
        switch_to_scene(1);
    }

    if (glm::length(g_current_scene->m_state.player->get_movement()) > 1.0f)
    {
        g_current_scene->m_state.player->set_movement(glm::normalize(g_current_scene->m_state.player->get_movement()));
    }
}


void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    
    g_view_matrix = glm::mat4(1.0f);
    
    if (g_current_scene->m_state.player->get_position().y > -12.5) {
            if (g_current_scene->m_state.player->get_position().x > 5) {
                if (g_current_scene->m_state.player->get_position().x > 12) {
                    if (g_current_scene->m_state.player->get_position().y > -2) {
                        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-12, 2.1, 0));
                    }
                    else {
                        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-12, -g_current_scene->m_state.player->get_position().y, 0));
                    }
                }
                else {
                    if (g_current_scene->m_state.player->get_position().y > -2) {
                        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, 2.1, 0));
                    }
                    else {
                        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, -g_current_scene->m_state.player->get_position().y, 0));
                    }
                }
            }
            else {
                if (g_current_scene->m_state.player->get_position().y > -2) {
                    if (g_current_scene->m_state.player->get_position().x < 5) {
                        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 2.1, 0));
                    }
                    else {
                        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, 2.1, 0));
                    }
                }
                else {
                    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, -g_current_scene->m_state.player->get_position().y, 0));
                }
            }
        }
        else {
            if (g_current_scene->m_state.player->get_position().x < 5) {
                g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 12.75, 0));
            }
            else {
                g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, 12.75, 0));
            }
        }
        
        std::cout << "(" << g_current_scene->m_state.player->get_position().x
            << ", " << g_current_scene->m_state.player->get_position().y;
        std::cout << ")\n";}


void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    g_current_scene->render(&g_shader_program);
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    initialise();
    
    while (g_game_is_running)
    {
        process_input();
        update();
        
        if (g_current_scene->m_state.next_scene >= 0){
            if (g_current_scene->m_state.player_lives == 0){
                switch_to_scene(5);
            }
            else{
                switch_to_scene(g_current_scene->m_state.next_scene, g_current_scene->m_state.player_lives);
            }
        }
        
        render();
    }
    
    shutdown();
    return 0;
}
