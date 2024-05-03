#include "Scene.hpp"

class Level : public Scene {
    public:
        Level(int _lives);
        void initialize() override;
        void update(float delta_time) override;
        void render(ShaderProgram *program) override;
};
