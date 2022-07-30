#include "point_manager.hpp"

namespace game {
    class GameStateController {
        public:
            GameStateController();
            ~GameStateController();
            // Updates the display with the current state of the simulation
            void update();
        private:
            PointManager* state;
    };
}