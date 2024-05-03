#include "Lose.hpp"

#define WIDTH 18
#define HEIGHT 17

unsigned int lose_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

void Lose::initialize() {
    
    m_state.next_scene = -1;

    GLuint map_texture_id = Utility::load_texture("tileset.png");
    m_state.map = new Map(WIDTH, HEIGHT, lose_data, map_texture_id, 1.0f, 4, 1);
    
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    
    m_state.player->set_position(glm::vec3(5.0, 0, 0));
    m_state.player->set_movement(glm::vec3(0));
    m_state.player->set_acceleration(glm::vec3(0, -9.81f, 0));
    m_state.player->set_speed(2.5f);
    m_state.player->m_texture_id = Utility::load_texture("Wizard1.png");

    
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.35f);
    
    m_state.player->m_jumping_power = 5.0f;
}

void Lose::update(float delta_time) {
    m_state.player->update(delta_time, m_state.player, m_state.enemies, 0, m_state.map);
    
    if (m_state.player->get_position().x >= 15.85) {
        m_state.next_scene = 1;
    }
}

void Lose::render(ShaderProgram *program) {
    GLuint font_texture_id = Utility::load_texture("font1.png");
    Utility::draw_text(program, font_texture_id, "You Lost!!", 0.4f, 0.1f, glm::vec3(3.5, -2.5, 0));
        
    Utility::draw_text(program, font_texture_id, "Enter to Restart", 0.4f, 0.1f, glm::vec3(1.5, -3.5, 0));
        
    m_state.map->render(program);
    m_state.player->render(program);
}
