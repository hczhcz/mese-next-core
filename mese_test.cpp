#include "mese.hpp"
#include "mese_print.hpp"

int main() {
    using namespace mese;

    Game game {8, get_preset(PresetId::modern, 8)};

    game.print_full(std::cout);

    game.alloc();
    game.alloc();
    game.alloc().loan_limit = 50000 * 8;

    if (false) {
        // classic
        game.submit(0, 30, 393, 1050, 1050, 393);
        game.submit(1, 30, 393, 1050, 1050, 393);
        game.submit(2, 30, 393, 1050, 1050, 393);
        game.submit(3, 30, 393, 1050, 1050, 393);
        game.submit(4, 30, 393, 1050, 1050, 393);
        game.submit(5, 30, 393, 1050, 1050, 393);
        game.submit(6, 30, 393, 1050, 1050, 393);
        game.submit(7, 30, 393, 1050, 1050, 393);
        game.close();
    } else {
        // modern
        game.submit(0, 30, 420, 1050, 1050, 420);
        game.submit(1, 30, 420, 1050, 1050, 420);
        game.submit(2, 30, 420, 1050, 1050, 420);
        game.submit(3, 30, 420, 1050, 1050, 420);
        game.submit(4, 30, 420, 1050, 1050, 420);
        game.submit(5, 30, 420, 1050, 1050, 420);
        game.submit(6, 30, 420, 1050, 1050, 420);
        game.submit(7, 30, 420, 1050, 1050, 420);
        game.close();
    }

    game.submit(0, 65, 472,    0, 11900, 15000);
    game.submit(1, 84, 462, 3200, 15000,  9000);
    game.submit(2, 80, 462, 2200,  1000, 15000);
    game.submit(3, 80, 474,  500, 10050, 13000);
    game.submit(4, 70, 420, 5000, 11000, 10000);
    game.submit(5, 65, 447, 5000, 10000, 10000);
    game.submit(6, 70, 445, 4000, 13000, 10111);
    game.submit(7, 69, 472,    0, 15000, 11900);
    game.close();

    game.submit(0, 68, 756, 7200,  5000,  6000);
    game.submit(1, 62, 699, 4000,  9747,  2000);
    game.submit(2, 71, 462, 2500, 15000, 10000);
    game.submit(3, 55, 657,  500,  7500,  7300);
    game.submit(4, 77, 692, 3500,  8000,  4500);
    game.submit(5, 65, 600, 5000, 10000,  7000);
    game.submit(6, 70, 650, 8000, 10500,  3000);
    game.submit(7, 52, 654,    0, 10000,  6000);
    game.close();

    game.print_public(std::cout);

    // game.serialize(std::cout);

    // Game game2 {std::cin};
    // game2.print_public(std::cout);

    return 0;
}
