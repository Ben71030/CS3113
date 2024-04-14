#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.hpp"
#include "Entity.hpp"
#include "Map.hpp"


struct GameState
{
    Map *map;
    Entity *player;
    Entity *enemies;
        
    int next_scene;
    int player_lives;
};

class Scene {
public:

    GameState m_state;
    
    virtual void initialize() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    
    GameState const get_state()             const { return m_state;             }
    void lose_a_life();
};
