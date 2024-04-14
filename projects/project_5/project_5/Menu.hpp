#include "Scene.hpp"

class Menu : public Scene {
    public:
        void initialize() override;
        void update(float delta_time) override;
        void render(ShaderProgram *program) override;
};
