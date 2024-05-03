#define LOG(argument) std::cout << argument << '\n'
#include "Third_Level.hpp"
#include "Entity.hpp"
#include "Utility.hpp"

#define THIRD_LEVEL_WIDTH 18
#define THIRD_LEVEL_HEIGHT 17

#define THIRD_LEVEL_ENEMY_COUNT 3

unsigned int third_level_data[] =
{
    3, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 3,
    3, 0, 0, 2, 0, 0, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 2, 3,
    3, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 3, 3,
    3, 0, 2, 3, 3, 3, 0, 0, 0, 3, 0, 0, 0, 3, 3, 3, 3, 3,
    3, 0, 0, 0, 0, 0, 0, 2, 2, 3, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 2, 2, 2, 3, 2, 0, 2, 0, 0, 2, 0, 3,
    3, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 3,
    3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 3, 3, 0, 3,
    3, 3, 2, 1, 0, 0, 1, 1, 0, 0, 1, 1, 3, 3, 3, 3, 0, 3,
    3, 0, 2, 2, 2, 2, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 3,
    3, 2, 2, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 3,
    3, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 2, 2, 3, 3, 3, 3,
    3, 0, 0, 3, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 3, 3, 3, 3,
    3, 3, 0, 0, 3, 3, 0, 0, 2, 3, 3, 0, 0, 3, 2, 3, 3, 3,
    3, 3, 3, 0, 0, 0, 0, 3, 2, 0, 0, 0, 2, 2, 2, 3, 3, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3
};

Third_Level::Third_Level(int the_lives) {
    m_state.player_lives = the_lives;
}

void Third_Level::initialize() {
    
    m_state.next_scene = -10;

    GLuint map_texture_id = Utility::load_texture("tileset.png");
    m_state.map = new Map(THIRD_LEVEL_WIDTH, THIRD_LEVEL_HEIGHT, third_level_data, map_texture_id, 1.0f, 4, 1);
    
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    
    m_state.player->set_position(glm::vec3(14.8, 0, 0));
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
    
    m_state.player->m_jumping_power = 7.0f;
    
    m_state.enemies = new Entity[THIRD_LEVEL_ENEMY_COUNT];
    
    GLuint enemy_texture_id = Utility::load_texture("zombie.jpeg");;

    for (int i = 0; i < THIRD_LEVEL_ENEMY_COUNT; i++) {
        m_state.enemies[i].set_entity_type(ENEMY);
        
        if (i == 0) {
            m_state.enemies[i].set_position(glm::vec3(11.0f, -7.0f, 0));
        }
        else if (i == 1) {
            m_state.enemies[i].set_position(glm::vec3(1.5f, -9.0f, 0));
        }
        
        m_state.enemies[i].set_acceleration(glm::vec3(0, -9.81f, 0));
        m_state.enemies[i].set_ai_type(GUARD);
        m_state.enemies[i].set_ai_state(IDLE);
        
        m_state.enemies[i].m_texture_id = enemy_texture_id;
        m_state.enemies[i].set_height(1.0f);
        m_state.enemies[i].set_width(0.5f);
        
        m_state.enemies[i].set_movement(glm::vec3(0));
        m_state.enemies[i].set_speed(1);
    }
}

void Third_Level::update(float delta_time) {
    m_state.player->update(delta_time, m_state.player, m_state.enemies, THIRD_LEVEL_ENEMY_COUNT, m_state.map);
    
    for (int i = 0; i < THIRD_LEVEL_ENEMY_COUNT; i++) {
        m_state.enemies[i].update(delta_time, m_state.player, m_state.enemies, THIRD_LEVEL_ENEMY_COUNT, m_state.map);
    }
    
    std::cout << "Lives: " << m_state.player_lives << "\n";
    
    for (int i = 0; i < THIRD_LEVEL_ENEMY_COUNT; i++) {
        std::cout << "Enemy: (" << m_state.enemies[i].get_position().x << ", ";
        std::cout << m_state.enemies[0].get_position().y << ") \n";
        
        if(m_state.player->check_collision(&m_state.enemies[i])) {
            if(m_state.enemies[i].m_collided_top) {
                std::cout << "yeaa\n";
                m_state.enemies[i].m_is_active = false;
            }
            else {
                std::cout << "nahhh!\n";
                lose_a_life();
                m_state.next_scene = 2;
            }
        }
    }
    
    if ((m_state.player->get_position().x >= 14.45) && (m_state.player->get_position().y >= 0.8)) {
        m_state.next_scene = 3;
    }
    else if (m_state.player->get_position().y < -15.5) {
        lose_a_life();
        if (m_state.player->m_lives == 0) {
            m_state.next_scene = 5;
        }
        else {
            m_state.next_scene = 2;
        }
    }
}

void Third_Level::render(ShaderProgram *program) {
    GLuint font_texture_id = Utility::load_texture("font1.png");
    Utility::draw_text(program, font_texture_id, "Level 3", 0.8f, 0.1f, glm::vec3(9.5, -10.5, 0));
    
    Utility::draw_text(program, font_texture_id, "Get back up", 0.2f, 0.05f, glm::vec3(9.4, -11.3, 0));
    Utility::draw_text(program, font_texture_id, "Finish!", 0.2f, 0.05f, glm::vec3(13.7, 1.0, 0));
    
    std::string lives_str = std::to_string(m_state.player_lives);
    
    std::string rounded = lives_str.substr(0, lives_str.find(".")+0);
    
    std::string lives_left = "Lives: " + rounded;
    
    Utility::draw_text(program, font_texture_id, lives_left, 0.2f, 0.1f, glm::vec3(3, 1.0, 0));
    
    for (int i = 0; i < THIRD_LEVEL_ENEMY_COUNT; i++) {
        m_state.enemies[i].render(program);
    }
    
    m_state.map->render(program);
    m_state.player->render(program);
    
}
