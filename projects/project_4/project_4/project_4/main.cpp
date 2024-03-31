/**
* Author: Anwar Benmallouk
* Assignment: Rise of the AI
* Date due: 2024-03-30, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 3
#define PLATFORM_COUNT 25
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 5

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

//#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.hpp"
#include "Map.hpp"

// ————— GAME STATE ————— //
struct GameState
{
    Entity* player;
    Entity* enemies;
    Entity* platforms;

    Map* map;

};

// ————— CONSTANTS ————— //
const int   WINDOW_WIDTH = 640,
            WINDOW_HEIGHT = 480;

const float BG_RED = 0.1922f,
            BG_BLUE = 0.549f,
            BG_GREEN = 0.9059f,
            BG_OPACITY = 1.0f;

const int   VIEWPORT_X = 0,
            VIEWPORT_Y = 0,
            VIEWPORT_WIDTH = WINDOW_WIDTH,
            VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char GAME_WINDOW_NAME[] = "PacMan";

const char  V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
            F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;


const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2,
    0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// ————— VARIABLES ————— //
GameState g_game_state;

SDL_Window* g_display_window;
bool g_game_is_running  = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix, g_model_matrix;

float   g_previous_ticks = 0.0f,
        g_accumulator = 0.0f;

// ————— GENERAL FUNCTIONS ————— //
GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint texture_id;
    glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return texture_id;
}

void initialise()
{
    // ————— GENERAL ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow(GAME_WINDOW_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // ————— VIDEO SETUP ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_model_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);



    // --------------------------- PLAYER ------------------------------
    g_game_state.player = new Entity();
    g_game_state.player->set_entity_type(PLAYER);

    g_game_state.player->m_texture_id = load_texture("pacman.png");
    g_game_state.player->set_height(1.0f);
    g_game_state.player->set_width(1.0f);

    g_game_state.player->set_position(glm::vec3(-4, -1, 0));
    g_game_state.player->set_movement(glm::vec3(0));
    g_game_state.player->set_acceleration(glm::vec3(0, -2.81f, 0));
    g_game_state.player->set_speed(2.85f);
    g_game_state.player->set_jumping_power(3.2f);

    g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->RIGHT];
    g_game_state.player->m_animation_frames = 4;
    g_game_state.player->m_animation_index = 0;
    g_game_state.player->m_animation_time = 0.0f;
    g_game_state.player->m_animation_cols = 4;
    g_game_state.player->m_animation_rows = 4;
    g_game_state.player->set_height(0.8f);
    g_game_state.player->set_width(0.8f);

    // Jumping
    g_game_state.player->m_jumping_power = 5.0f;

    
    // --------------------------- ENEMIES -----------------------------
    g_game_state.enemies = new Entity[ENEMY_COUNT];
    
    GLuint enemy_texture_id;
    for (int i = 0; i < ENEMY_COUNT; i ++) {
        g_game_state.enemies[i].set_entity_type(ENEMY);
        
        if (i == 0) {
            g_game_state.enemies[i].set_position(glm::vec3(1.0f, 0.5f, 0.0f));
            g_game_state.enemies[i].set_acceleration(glm::vec3(0, -9.81f, 0));
            g_game_state.enemies[i].set_ai_type(GUARD);
            g_game_state.enemies[i].set_ai_state(IDLE);
            enemy_texture_id = load_texture("pacman_enemy.png");
        }
        if (i == 1) {
            g_game_state.enemies[i].set_position(glm::vec3(0.0f, 1.0f, 0.0f));
            g_game_state.enemies[i].set_acceleration(glm::vec3(0, -9.81f, 0));
            g_game_state.enemies[i].set_ai_type(GUARD);
            g_game_state.enemies[i].set_ai_state(IDLE);
            enemy_texture_id = load_texture("pacman_enemy.png");
        }
        if (i == 2) {
            g_game_state.enemies[i].set_position(glm::vec3(-1.0f, 2.0f, 0.0f));
            g_game_state.enemies[i].set_acceleration(glm::vec3(0, -9.81f, 0));
            g_game_state.enemies[i].set_ai_type(GUARD);
            g_game_state.enemies[i].set_ai_state(IDLE);
            enemy_texture_id = load_texture("pacman_enemy.png");
        }
        
        g_game_state.enemies[i].m_texture_id = enemy_texture_id;
        g_game_state.enemies[i].set_height(1.0f);
        g_game_state.enemies[i].set_width(1.0f);
        
        g_game_state.enemies[i].set_movement(glm::vec3(0));
        g_game_state.enemies[i].set_speed(1);
    }
// -------------------------- PLATFORMS ----------------------------

    g_game_state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platform_texture_id = load_texture("vegetation_grass_card_03.png");
    
    int x = 0;
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        g_game_state.platforms[i].set_entity_type(PLATFORM);
        g_game_state.platforms[i].m_texture_id = platform_texture_id;
        
        if (i < 10) {
            g_game_state.platforms[i].set_position(glm::vec3(-4.5 + i, -3.25, 0));
        }
        else if (i == 14) {
            x = i - 4;
            g_game_state.platforms[i].set_position(glm::vec3(-4.5 + x, -2.25, 0));
        }
        else if (i == 17) {
            x = i - 7;
            g_game_state.platforms[i].set_position(glm::vec3(-4.5 + x, -1.25, 0));
        }
        else if (i == 19) {
            x = i - 9;
            g_game_state.platforms[i].set_position(glm::vec3(-4.5 + x, -0.25, 0));
        }
        else {
            x = i - 19;
            g_game_state.platforms[i].set_position(glm::vec3(-4.5 + x, 0.75, 0));
        }
        
        g_game_state.platforms[i].update(0, NULL, NULL, 0, NULL, NULL);
    }
    


    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void draw_text(ShaderProgram *program, GLuint font_texture_id, std::string text, float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    
    std::vector<float> vertices;
    std::vector<float> tex_coords;
    
    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        
        tex_coords.insert(tex_coords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
        
    }


    glm::mat4 font_model_matrix = glm::mat4(1.0f);
    g_model_matrix = glm::translate(font_model_matrix, position);
    program->set_model_matrix(font_model_matrix);

    glBindTexture(GL_TEXTURE_2D, font_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
void process_input()
{
    g_game_state.player->set_movement(glm::vec3(0.0f));

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
                // Quit the game with a keystroke
                g_game_is_running  = false;
                break;

            case SDLK_SPACE:
                // Jump
                if (g_game_state.player->m_collided_bottom)
                {
                    g_game_state.player->m_is_jumping = true;
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
        g_game_state.player->move_left();
        g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_game_state.player->move_right();
        g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->RIGHT];
    }

    if (glm::length(g_game_state.player->get_movement()) > 1.0f)
    {
        g_game_state.player->set_movement(glm::normalize(g_game_state.player->get_movement()));
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

    while (delta_time >= FIXED_TIMESTEP)
    {
        g_game_state.player->update(FIXED_TIMESTEP, g_game_state.player, g_game_state.platforms, PLATFORM_COUNT, g_game_state.enemies, ENEMY_COUNT);
        
        for (int i = 0; i < ENEMY_COUNT; i++) {
            g_game_state.enemies[i].update(FIXED_TIMESTEP, g_game_state.player, g_game_state.platforms, PLATFORM_COUNT, g_game_state.enemies, ENEMY_COUNT);
        }
        
        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_game_state.player->get_position().x, 0.0f, 0.0f));
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        g_game_state.platforms[i].render(&g_shader_program);
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        g_game_state.enemies[i].render(&g_shader_program);
    }
    
    g_game_state.player->render(&g_shader_program);
    
    int defeat_count = 0;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (g_game_state.enemies[i].was_defeated) {
            defeat_count += 1;
        }
    }
    std::cout << "Count of Defeated: " << defeat_count << "\n";
    if (defeat_count == ENEMY_COUNT) {
        g_game_state.player->defeated_enemies = true;
    }
    
    if (g_game_state.player->defeated_enemies == true) {
        draw_text(&g_shader_program, load_texture("font1.png"), "W", 0.5f, -0.25f, glm::vec3(1.0, 0, 0));
        std::cout << "win\n";
        g_game_state.player->m_is_active = false;
    }
    else if (g_game_state.player->was_defeated == true) {
        draw_text(&g_shader_program, load_texture("font1.png"), "L", 0.5f, -0.25f, glm::vec3(1.0, 0, 0));
        std::cout << "lose\n";
        g_game_state.player->m_is_active = false;
    }
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    delete[] g_game_state.enemies;
    delete    g_game_state.player;
    delete    g_game_state.map;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running )
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
