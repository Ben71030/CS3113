#define LOG(argument) std::cout << argument << '\n'
#include "First_Level.hpp"
#include "Entity.hpp"
#include "Utility.hpp"

#define FIRST_LEVEL_WIDTH 20
#define FIRST_LEVEL_HEIGHT 20
#define FIRST_LEVEL_ENEMY_COUNT 4

const float TIMER_DURATION = 120.0f;
float g_timer = TIMER_DURATION;


unsigned int first_level_data[] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

First_Level::First_Level(int the_lives) {
    m_state.player_lives = the_lives;
}

void First_Level::initialize() {
    
    m_state.next_scene = -10;

    GLuint map_texture_id = Utility::load_texture("bush.png");
    m_state.map = new Map(FIRST_LEVEL_WIDTH, FIRST_LEVEL_HEIGHT, first_level_data, map_texture_id, 1.0f, 4, 1);
    
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    
    m_state.player->set_position(glm::vec3(6, -1, 0));
    m_state.player->set_movement(glm::vec3(0));
    m_state.player->set_acceleration(glm::vec3(0, -9.81f, 0));
    m_state.player->set_speed(1.5f);
    m_state.player->m_texture_id = Utility::load_texture("Wizard1.png");

    
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.35f);
    
    m_state.player->m_jumping_power = 5.0f;
    
    m_state.enemies = new Entity[FIRST_LEVEL_ENEMY_COUNT];
    
    
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_position(glm::vec3(6, -
5, 0));
    m_state.enemies[0].set_acceleration(glm::vec3(0, -9.81f, 0));
    
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(WALKING);
    
    m_state.enemies[0].m_texture_id = Utility::load_texture("Hurt1.png");
    m_state.enemies[0].set_height(1.0f);
    m_state.enemies[0].set_width(0.5f);
    m_state.enemies[0].set_movement(glm::vec3(0.0f, 1.0f, 0.0f));
    m_state.enemies[0].set_speed(1.75);
    
    
    m_state.enemies[1].set_entity_type(ENEMY);
    m_state.enemies[1].set_position(glm::vec3(8, -
4, 0));
    m_state.enemies[1].set_acceleration(glm::vec3(0, -9.81f, 0));
    
    m_state.enemies[1].set_ai_type(GUARD);
    m_state.enemies[1].set_ai_state(WALKING);
    
    m_state.enemies[1].m_texture_id = Utility::load_texture("Hurt1.png");
    m_state.enemies[1].set_height(1.0f);
    m_state.enemies[1].set_width(0.5f);
    m_state.enemies[1].set_movement(glm::vec3(0.0f, -1.0f, 0.0f));
    m_state.enemies[1].set_speed(1.75);
    
    
    m_state.enemies[2].set_entity_type(ENEMY);
    m_state.enemies[2].set_position(glm::vec3(2, -
10, 0));
    m_state.enemies[2].set_acceleration(glm::vec3(0, -9.81f, 0));
    
    m_state.enemies[2].set_ai_type(GUARD);
    m_state.enemies[2].set_ai_state(WALKING);
    
    m_state.enemies[2].m_texture_id = Utility::load_texture("Hurt1.png");
    m_state.enemies[2].set_height(1.0f);
    m_state.enemies[2].set_width(0.5f);
    m_state.enemies[2].set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
    m_state.enemies[2].set_speed(1.75);


}

void First_Level::update(float delta_time) {
    
    std::cout << "Current 0: " << m_state.enemies[0].get_position().y << std::endl;
    if (m_state.enemies[0].get_position().y >= -1){
        m_state.enemies[0].set_movement(glm::vec3(0.0f, -1.0f, 0.0f));
    }
    if (m_state.enemies[0].get_position().y <= -8){
        m_state.enemies[0].set_movement(glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    std::cout << "Current 1: " << m_state.enemies[1].get_position().y << std::endl;
    if (m_state.enemies[1].get_position().y >= -3){
        m_state.enemies[1].set_movement(glm::vec3(0.0f, -1.0f, 0.0f));
    }
    if (m_state.enemies[1].get_position().y <= -7){
        m_state.enemies[1].set_movement(glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    std::cout << "Current 2: " << m_state.enemies[2].get_position().y << std::endl;
    if (m_state.enemies[2].get_position().x <= 2){
        m_state.enemies[2].set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (m_state.enemies[2].get_position().x >= 8){
        m_state.enemies[2].set_movement(glm::vec3(-1.0f, 1.0f, 0.0f));
    }
    
    
    m_state.player->update(delta_time, m_state.player, m_state.enemies, FIRST_LEVEL_ENEMY_COUNT, m_state.map);
    
    for (int i = 0; i < FIRST_LEVEL_ENEMY_COUNT; i++) {
        m_state.enemies[i].update(delta_time, m_state.player, m_state.enemies, FIRST_LEVEL_ENEMY_COUNT, m_state.map);
    }
    
    
    std::cout << "Lives: " << m_state.player_lives << "\n";
    
    for (int i = 0; i < FIRST_LEVEL_ENEMY_COUNT; i++) {
        std::cout << "Enemy: (" << m_state.enemies[i].get_position().x << ", ";
        std::cout << m_state.enemies[0].get_position().y << ") \n";
        
        if(m_state.player->check_collision(&m_state.enemies[i])) {
            if(m_state.enemies[i].m_collided_top) {
                std::cout << "yeaa\n";
                m_state.enemies[i].m_is_active = false;
            }
            else {
                std::cout << "nahh!\n";
                lose_a_life();
                m_state.next_scene = 1;
            }
        }
    }
    
    if (m_state.player->get_position().x >= 17.00) {
        m_state.next_scene = 2;
    }
}

void First_Level::render(ShaderProgram *program) {
    GLuint font_texture_id = Utility::load_texture("font1.png");

    int minutes = static_cast<int>(g_timer) / 60;
    int seconds = static_cast<int>(g_timer) % 60;
    std::string time_str = "Time: " + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);

    Utility::draw_text(program, font_texture_id, time_str, 0.2f, 0.1f, glm::vec3(6, 1.0, 0));
    
//    Utility::draw_text(program, font_texture_id, "Level 1", 0.8f, 0.1f, glm::vec3(2, -13, 0));
//
//    Utility::draw_text(program, font_texture_id, "Up!", 0.2f, 0.05f, glm::vec3(8.7, -10, 0));
//
//    Utility::draw_text(program, font_texture_id, "Climb!", 0.2f, 0.05f, glm::vec3(2.0, -4.8, 0));
//
//    Utility::draw_text(program, font_texture_id, "Finish..?", 0.2f, 0.1f, glm::vec3(13, 0.5, 0));
    
    std::string lives_str = std::to_string(m_state.player_lives);
    
    std::string rounded = lives_str.substr(0, lives_str.find(".")+0);
    
    std::string lives_left = "Lives: " + rounded;
    
    Utility::draw_text(program, font_texture_id, lives_left, 0.2f, 0.1f, glm::vec3(3, 1.0, 0));
    
    for (int i = 0; i < FIRST_LEVEL_ENEMY_COUNT; i++) {
        m_state.enemies[i].render(program);
    }
    
    m_state.map->render(program);
    m_state.player->render(program);
    
}
