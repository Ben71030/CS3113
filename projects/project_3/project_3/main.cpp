#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define NUMBER_OF_ENEMIES 3
#define FIXED_TIMESTEP 0.0166666f
#define ACC_OF_GRAVITY -3.81f
#define PLATFORM_COUNT 3

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

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

struct GameState
{
    Entity* player;
    Entity* platforms;
};

const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const float BG_RED = 255.f,
            BG_BLUE = 255.f,
            BG_GREEN = 255.f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND  = 1000.0;
const char  SPRITESHEET_FILEPATH[]  = "george_0.png",
            PLATFORM_FILEPATH[]     = "platformPack_tile027.png",
            FONT_FILEPATH[] = "font1.png";

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL  = 0;
const GLint TEXTURE_BORDER   = 0;

GameState g_game_state;

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_time_accumulator = 0.0f;

GLuint load_texture(const char* filepath){
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
    }

void initalize(){
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Parachute Lander",
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
    
    
    // Creating the player
    
    g_game_state.player = new Entity();
    g_game_state.player -> set_position(glm::vec3(0.0f, 4.0f, 0.0f));
    g_game_state.player -> set_movement(glm::vec3(0.0f));
    g_game_state.player -> set_acceleration(glm::vec3(2.0f, ACC_OF_GRAVITY * 0.1, 0.0f));
    g_game_state.player -> set_speed(1.0f);
    g_game_state.player -> m_texture_id = load_texture("parachute.png");
    
    
    // Creating the platform
    g_game_state.platforms = new Entity[PLATFORM_COUNT];

    g_game_state.platforms[0].m_texture_id = load_texture(PLATFORM_FILEPATH);
    g_game_state.platforms[0].set_position(glm::vec3(-3.0f, -1.0f, 0.0f));
    g_game_state.platforms[0].update(0.0f, NULL, 0);
    
    g_game_state.platforms[1].m_texture_id = load_texture(PLATFORM_FILEPATH);
    g_game_state.platforms[1].set_position(glm::vec3(-0.5f, -3.0f, 0.0f));
    g_game_state.platforms[1].update(0.0f, NULL, 0);
    
    g_game_state.platforms[2].m_texture_id = load_texture(PLATFORM_FILEPATH);
    g_game_state.platforms[2].set_position(glm::vec3(3.0f, -1.0f, 0.0f));
    g_game_state.platforms[2].update(0.0f, NULL, 0);
    

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
    
    
void process_input(){
    g_game_state.player -> set_movement(glm::vec3(0.0f));
    
    
    SDL_Event event;
    while (SDL_PollEvent(&event)){
            switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_q:
                    g_game_is_running = false;
                    break;
                default:
                    break;
                }

            default:
                break;
            }
    }
    
    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    
    glm::vec3 current_acceleration = g_game_state.player->get_acceleration();
    if (key_state[SDL_SCANCODE_LEFT])
        {
            g_game_state.player->move_left();
            g_game_state.player->set_acceleration(glm::vec3(-1.0f, current_acceleration.y, current_acceleration.z));


        }
    if (key_state[SDL_SCANCODE_RIGHT])
        {
            g_game_state.player->move_right();
            g_game_state.player->set_acceleration(glm::vec3(1.0f, current_acceleration.y, current_acceleration.z));


        }else{
            glm::vec3 current_acceleration = g_game_state.player->get_acceleration();
            g_game_state.player->set_acceleration(glm::vec3(current_acceleration.x * 0.3, current_acceleration.y, current_acceleration.z));
            if (current_acceleration.x > 0.0f){
                g_game_state.player->move_right();
            }
            if (current_acceleration.x < 0.0f){
                g_game_state.player->move_left();
            }
        }

        if (glm::length(g_game_state.player->get_movement()) > 1.0f)
        {
            g_game_state.player->set_movement(glm::normalize(g_game_state.player->get_movement()));
        }
    
        
    
}
    
void update(){
    
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_time_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_time_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP)
    {
        g_game_state.player->update(FIXED_TIMESTEP, g_game_state.platforms, PLATFORM_COUNT);
        delta_time -= FIXED_TIMESTEP;
    }

    g_time_accumulator = delta_time;
}
    
void render(){
    
    glClear(GL_COLOR_BUFFER_BIT);

    g_game_state.player->render(&g_shader_program);

    for (int i = 0; i < PLATFORM_COUNT; i++) g_game_state.platforms[i].render(&g_shader_program);
    
    if (g_game_state.player->get_position().y < -3.75 || g_game_state.player->get_position().y > 3.75 || g_game_state.player->get_position().x > 5.00 || g_game_state.player->get_position().x < -5.00 ){

        GameState loss_message;
        loss_message.player = new Entity();
        loss_message.player -> set_position(glm::vec3(0.0f));
        loss_message.player -> m_texture_id = load_texture("game-over.png");
        loss_message.player -> render(&g_shader_program);
    }
    if (g_game_state.player->get_collided() == true){
        GameState win_message;
        win_message.player = new Entity();
        win_message.player -> set_position(glm::vec3(0.0f));
        win_message.player -> m_texture_id = load_texture("mission-accomplished.png");
        win_message.player -> render(&g_shader_program);
    }

    
    SDL_GL_SwapWindow(g_display_window);
}
    
void shutdown(){ SDL_Quit();}
    
int main(int argc, char* argv[]){
        
    initalize();
    
    while(g_game_is_running){
        
        process_input();
        update();
        render();
        
    }
    
    shutdown();
    return 0;
        
}

