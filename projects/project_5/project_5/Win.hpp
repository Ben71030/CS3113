#include "Scene.hpp"

class Win : public Scene {
    public:
        void initialize() override;
        void update(float delta_time) override;
        void render(ShaderProgram *program) override;
};
