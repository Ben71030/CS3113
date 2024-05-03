#include "Menu.hpp"

//#define MENU_WIDTH 18
//#define MENU_HEIGHT 17
#define MENU_WIDTH 10
#define MENU_HEIGHT 10

unsigned int menu_data[] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};
//{
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
//    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,
//    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
//};

void Menu::initialize() {
    
    m_state.next_scene = -1;

    GLuint map_texture_id = Utility::load_texture("tileset.png");
    m_state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, map_texture_id, 1.0f, 4, 1);
    
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    
    m_state.player->set_position(glm::vec3(5.0, -5.0, 0));
    m_state.player->set_movement(glm::vec3(0));
    m_state.player->set_acceleration(glm::vec3(0, -9.81f, 0));
    m_state.player->set_speed(2.5f);
    m_state.player->m_texture_id = Utility::load_texture("george_0.png");
    
    m_state.player->m_walking[m_state.player->LEFT]  = new int[4] { 1, 5, 9,  13 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 3, 7, 11, 15 };
    m_state.player->m_walking[m_state.player->UP]    = new int[4] { 2, 6, 10, 14 };
    m_state.player->m_walking[m_state.player->DOWN]  = new int[4] { 0, 4, 8,  12 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index = 0;
    m_state.player->m_animation_time = 0;
    m_state.player->m_animation_cols = 4;
    m_state.player->m_animation_rows = 4;
    
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.35f);
    
    m_state.player->m_jumping_power = 5.0f;
}

void Menu::update(float delta_time) {
    m_state.player->update(delta_time, m_state.player, m_state.enemies, 0, m_state.map);
    
    if (m_state.player->get_position().x >= 15.85) {
        m_state.next_scene = 1;
    }
}

void Menu::render(ShaderProgram *program) {
    GLuint font_texture_id = Utility::load_texture("font1.png");
    
//    Utility::draw_text(program, font_texture_id, "Click Return", 0.4f, 0.1f, glm::vec3(2.0, -10, 0));
    Utility::draw_text(program, font_texture_id, "Welcome to the Maze", 0.3f, 0.001f, glm::vec3(2.0, -2.0, 0));
    Utility::draw_text(program, font_texture_id, "Try to survive for 2 mins", 0.3f, 0.001f, glm::vec3(1.0, -3.0, 0));
    Utility::draw_text(program, font_texture_id, "Click Return to Begin", 0.3f, 0.001f, glm::vec3(2.0, -5.0, 0));
    
    
//    m_state.map->render(program);
//    m_state.player->render(program);
}
