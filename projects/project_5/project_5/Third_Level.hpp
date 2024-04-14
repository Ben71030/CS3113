#include "Scene.hpp"

class Third_Level : public Scene {
    public:
        Third_Level(int _lives);
        void initialize() override;
        void update(float delta_time) override;
        void render(ShaderProgram *program) override;
};
