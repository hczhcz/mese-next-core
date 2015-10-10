#include <cstring>

#include "mese.hpp"
#include "mese_print.hpp"

void test() {
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
}

int frontend(int argc, char *argv[]) {
    using namespace mese;

    std::cout.precision(2);
    std::cout.setf(std::ios::fixed);

    if (argc < 2) {
        std::cout << highlight1 << "  Command List  " << normal << std::endl;
        std::cout << indent
            << highlight2 << "test" << normal
            << std::endl;
        std::cout << indent
            << highlight2 << "init" << normal
            << "  player_count preset [name value]..." << std::endl;
        std::cout << indent
            << highlight2 << "alloc" << normal
            << "  [name value]..." << std::endl;
        std::cout << indent
            << highlight2 << "submit" << normal
            << "  player period price prod mk ci rd -> bool" << std::endl;
        std::cout << indent
            << highlight2 << "close" << normal
            << "  -> bool" << std::endl;
        std::cout << indent
            << highlight2 << "print_full" << normal
            << std::endl;
        std::cout << indent
            << highlight2 << "print_player_early" << normal
            << "  player" << std::endl;
        std::cout << indent
            << highlight2 << "print_player" << normal
            << "  player" << std::endl;
        std::cout << indent
            << highlight2 << "print_public" << normal
            << std::endl;
        std::cout << std::endl;

        return 0;
    } else {
        if (strcmp(argv[1], "test") == 0) {
            test();

            return 0;
        } else if (strcmp(argv[1], "init") == 0) {
            if (argc < 4) {
                throw 1; // TODO
            }

            uint64_t player_count {strtoul(argv[2], nullptr, 10)};

            PresetId id;
            if (strcmp(argv[3], "classic") == 0) {
                id = PresetId::classic;
            } else if (strcmp(argv[3], "imese") == 0) {
                id = PresetId::imese;
            } else if (strcmp(argv[3], "modern") == 0) {
                id = PresetId::modern;
            } else {
                throw 1;
            }

            Settings settings = get_preset(id, player_count);

            // TODO: set values?

            Game game {player_count, std::move(settings)};

            game.serialize(std::cout);

            return 0;
        } else if (strcmp(argv[1], "alloc") == 0) {
            Game game {std::cin};

            Settings settings = game.period.back().settings; // copy

            // TODO: set values?

            game.alloc(std::move(settings));

            game.serialize(std::cout);

            return 0;
        } else if (strcmp(argv[1], "submit") == 0) {
            Game game {std::cin};

            if (argc < 8) {
                throw 1; // TODO
            }

            if (
                strtod(argv[3], nullptr) == game.now_period
                && game.submit(
                    strtoul(argv[2], nullptr, 10),
                    strtod(argv[4], nullptr),
                    strtod(argv[5], nullptr),
                    strtod(argv[6], nullptr),
                    strtod(argv[7], nullptr),
                    strtod(argv[8], nullptr)
                )
            ) {
                game.serialize(std::cout);

                return 0;
            } else {
                game.serialize(std::cout);

                return 1;
            }
        } else if (strcmp(argv[1], "close") == 0) {
            Game game {std::cin};

            if (game.close()) {
                game.serialize(std::cout);

                return 0;
            } else {
                game.serialize(std::cout);

                return 1;
            }
        } else if (strcmp(argv[1], "print_full") == 0) {
            Game game {std::cin};

            game.print_full(std::cout);

            return 0;
        } else if (strcmp(argv[1], "print_player_early") == 0) {
            Game game {std::cin};

            if (argc < 3) {
                throw 1; // TODO
            }

            game.print_player_early(std::cout, strtoul(argv[2], nullptr, 10));

            return 0;
        } else if (strcmp(argv[1], "print_player") == 0) {
            Game game {std::cin};

            if (argc < 3) {
                throw 1; // TODO
            }

            game.print_player(std::cout, strtoul(argv[2], nullptr, 10));

            return 0;
        } else if (strcmp(argv[1], "print_public") == 0) {
            Game game {std::cin};

            game.print_public(std::cout);

            return 0;
        } else {
            throw 1; // TODO: unknown command
        }
    }
}

int main(int argc, char *argv[]) {
    try {
        return frontend(argc, argv);
    } catch (...) {
        return -1;
    }
}
