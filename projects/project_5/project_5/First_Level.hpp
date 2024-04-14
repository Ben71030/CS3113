#include "Scene.hpp"

class First_Level : public Scene {
    public:
        First_Level(int _lives);
        void initialize() override;
        void update(float delta_time) override;
        void render(ShaderProgram *program) override;
};
