#include "mese.hpp"

int main() {
    using namespace mese;

    Game game {8};

    game.period[1].debug(std::cout);

    return 0;
}
