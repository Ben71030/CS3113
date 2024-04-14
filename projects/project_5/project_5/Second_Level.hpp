#include "Scene.hpp"

class Second_Level : public Scene {
    public:
        Second_Level(int _lives);
        void initialize() override;
        void update(float delta_time) override;
        void render(ShaderProgram *program) override;
};
