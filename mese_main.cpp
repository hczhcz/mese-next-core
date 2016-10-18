#include <cstring>

#include "mese.hpp"
#include "mese_print.hpp"

namespace mese {

void test() {
    Game game {10, get_preset("modern", 10)};

    game.alloc().loan_limit = 50000 * 10;
    game.alloc();
    game.alloc();
    game.alloc();
    game.alloc();
    game.alloc();
    game.alloc();

    ai_kokoro(game, 9, 1);
    game.submit(0, 75, 500, 5000, 12000, 8500);
    game.submit(1, 62, 446, 0, 12000, 10000);
    game.submit(2, 73, 525, 7070, 12000, 10111);
    game.submit(3, 75, 525, 8000, 12000, 8500);
    game.submit(4, 68, 447, 5364, 12000, 9834);
    game.submit(5, 70, 420, 8000, 12000, 0);
    game.submit(6, 62, 420, 2000, 9000, 12000);
    game.submit(7, 65, 447, 0, 15000, 12500);
    game.submit(8, 60, 525, 4000, 15000, 5000);
    game.close();

    ai_kokoro(game, 9, 1);
    game.submit(0, 68, 720, 9000, 4900, 0);
    game.submit(1, 60, 640, 7500, 7000, 0);
    game.submit(2, 65, 798, 5000, 9000, 7690);
    game.submit(3, 67, 719, 9000, 10000, 8000);
    game.submit(4, 68, 642, 3201, 10000, 5073);
    game.submit(5, 52, 680, 0, 12000, 7200);
    game.submit(6, 54, 723, 0, 12000, 0);
    game.submit(7, 57, 750, 8000, 6798, 0);
    game.submit(8, 62, 720, 6000, 15000, 0);
    game.close();

    ai_kokoro(game, 9, 1);
    game.submit(0, 60, 800, 12000, 13000, 13000);
    game.submit(1, 52, 747, 10000, 15000, 3500);
    game.submit(2, 51, 787, 2000, 8000, 12000);
    game.submit(3, 59, 908, 10500, 10000, 10000);
    game.submit(4, 50, 812, 812, 10000, 8932);
    game.submit(5, 55, 900, 8000, 3000, 10000);
    game.submit(6, 47, 890, 8000, 13000, 1000);
    game.submit(7, 51, 900, 5000, 11900, 12000);
    game.submit(8, 39, 964, 4000, 13000, 0);
    game.close();

    ai_kokoro(game, 9, 1);
    game.submit(0, 58, 1162, 10000, 15000, 15000);
    game.submit(1, 46, 1010, 13000, 15000, 0);
    game.submit(2, 37, 908, 1000, 12000, 1000);
    game.submit(3, 69, 1208, 15000, 15000, 15000);
    game.submit(4, 35, 971, 1442, 11000, 5000);
    game.submit(5, 36, 919, 0, 12000, 11900);
    game.submit(6, 42, 1011, 8000, 2526, 10000);
    game.submit(7, 47, 1000, 6000, 13595, 10000);
    game.submit(8, 35, 1350, 6000, 15000, 0);
    game.close();

    ai_kokoro(game, 9, 1);
    game.submit(0, 45, 1400, 13000, 14000, 15000);
    game.submit(1, 41, 1260, 15000, 15000, 0);
    game.submit(2, 27, 1102, 1000, 12000, 1000);
    game.submit(3, 50, 1219, 15000, 7000, 7000);
    game.submit(4, 31, 1144, 6864, 9000, 9696);
    game.submit(5, 47, 1194, 10000, 13600, 10000);
    game.submit(6, 42, 1263, 12000, 2526, 12000);
    game.submit(7, 45, 1369, 10000, 15000, 12000);
    game.submit(8, 30, 1650, 6000, 10000, 0);
    game.close();

    ai_kokoro(game, 9, 1);
    game.submit(0, 42, 1750, 15000, 15000, 0);
    game.submit(1, 38, 1500, 15000, 15000, 0);
    game.submit(2, 34, 1287, 12000, 12000, 1000);
    game.submit(3, 38, 1298, 15000, 3244, 0);
    game.submit(4, 33, 1347, 4041, 12000, 9000);
    game.submit(5, 54, 1500, 11100, 14000, 12000);
    game.submit(6, 34, 1263, 0, 2526, 0);
    game.submit(7, 43, 1688, 15000, 15000, 10000);
    game.submit(8, 28, 1932, 6000, 10000, 0);
    game.close();

    ai_kokoro(game, 9, 1);
    game.submit(0, 30, 1900, 15000, 15000, 0);
    game.submit(1, 30, 1722, 15000, 4304, 0);
    game.submit(2, 28, 1463, 12000, 12000, 1000);
    game.submit(3, 33, 1298, 15000, 0, 0);
    game.submit(4, 29, 1446, 6892, 12000, 9);
    game.submit(5, 35, 1700, 15000, 4000, 0);
    game.submit(6, 39, 1263, 12000, 0, 0);
    game.submit(7, 39, 1920, 15000, 13913, 0);
    game.submit(8, 22, 2086, 0, 5000, 0);
    game.close();

    game.print_player(std::cout, 9);

    // game.serialize(std::cout);
}

void print_info(bool info, bool help, bool list, bool cow) {
    static const char indent[] {"    "};
    #if defined(__linux__)
        static const char highlight0[] {"\x1b[1m"};
        static const char highlight1[] {"\x1b[1;7m"};
        static const char highlight2[] {"\x1b[1;34m"};
        static const char normal[] {"\x1b[0m"};
    #else
        static const char highlight0[] {""};
        static const char highlight1[] {""};
        static const char highlight2[] {""};
        static const char normal[] {""};
    #endif

    #define MESE_HL0(...) highlight0 << indent << __VA_ARGS__ << indent << normal
    #define MESE_HL1(...) highlight1 << __VA_ARGS__ << normal
    #define MESE_HL2(...) indent << highlight2 << __VA_ARGS__ << normal
    #define MESE_HL3(...) indent << indent << __VA_ARGS__

    std::cout << std::endl;
    std::cout << MESE_HL0(">>                                 <<") << std::endl;
    std::cout << MESE_HL0(">>            MESE-Next            <<") << std::endl;
    std::cout << MESE_HL0(">>    The modern remake of MESE    <<") << std::endl;
    std::cout << MESE_HL0(">>                                 <<") << std::endl;
    if (cow) {
        system(
            "echo '\n"
                " Throw out your pens,          \n"
                "     your pencils,             \n"
                "       your typewriters,       \n"
                "         your word processors, \n"
                "   and get your hands on...    \n"
                "                               \n"
                "          * ECHOPEN *          \n"
            "' | cowsay -n 2> /dev/null"
        );
    }
    std::cout << std::endl;

    if (info) {
        std::cout << MESE_HL1("  System Information  ") << std::endl;
        std::cout << std::endl;

        std::cout << MESE_HL2("version")
            << "  " << std::hex << BINARY_VER << std::dec
                << ", " << MAX_PLAYER << " players" << std::endl;
        std::cout << MESE_HL2("build date")
            << "  " << __DATE__ /* << " " << __TIME__ */ << std::endl;
        std::cout << MESE_HL2("build mode")
            << "  c++ " << __cplusplus << std::endl;
        std::cout << MESE_HL2("compiler")
            #if defined(__clang__)
                << "  clang " << __clang_major__
                    << "." << __clang_minor__
                    << "." << __clang_patchlevel__ << std::endl;
            #elif defined(__GNUC__)
                << "  gcc " << __GNUC__
                    << "." << __GNUC_MINOR__
                    << "." << __GNUC_PATCHLEVEL__ << std::endl;
            #else
                << "  unknown" << std::endl;
            #endif
        std::cout << MESE_HL2("target system")
            #if defined(__linux__)
                << "  linux"
            #elif defined(_WIN32) || defined(_WIN64)
                << "  windows"
            #else
                << "  unknown os"
            #endif
            #if defined(__amd64__)
                << " amd64" << std::endl;
            #elif defined(__i386__)
                << " i386" << std::endl;
            #else
                << " unknown arch" << std::endl;
            #endif
        std::cout << MESE_HL2("byte width")
            << "  u64: " << sizeof(uint64_t)
                << ", fp: " << sizeof(double)
                << ", total: " << sizeof(Period) << "n"
                    << " + " << 4 * sizeof(uint64_t) << std::endl;
        std::cout << std::endl;
    }

    if (help) {
        std::cout << MESE_HL1("  Command List  ") << std::endl;
        std::cout << std::endl;

        std::cout << MESE_HL2("init")
            << "  player_count preset [name value]..." << std::endl;
        std::cout << MESE_HL3("output:") << " binary data" << std::endl << std::endl;

        std::cout << MESE_HL2("alloc")
            << "  [name value]..." << std::endl;
        std::cout << MESE_HL3("input:") << " binary data" << std::endl
            << MESE_HL3("output:") << " binary data" << std::endl << std::endl;

        std::cout << MESE_HL2("submit")
            << "  player period price prod mk ci rd" << std::endl;
        std::cout << MESE_HL2("close")
            << std::endl;
        std::cout << MESE_HL2("close_force")
            << std::endl;
        std::cout << MESE_HL3("input:") << " binary data" << std::endl
            << MESE_HL3("output:") << " binary data" << std::endl
            << MESE_HL3("error number:") << " declined=1" << std::endl << std::endl;

        std::cout << MESE_HL2("print_full")
            << std::endl;
        std::cout << MESE_HL2("print_player_early")
            << "  player" << std::endl;
        std::cout << MESE_HL2("print_player")
            << "  player" << std::endl;
        std::cout << MESE_HL2("print_public")
            << std::endl;
        std::cout << MESE_HL3("input:") << " binary data" << std::endl
            << MESE_HL3("output:") << " report" << std::endl << std::endl;

        std::cout << MESE_HL2("help")
            << std::endl;
        std::cout << std::endl;
    }

    if (list) {
        std::cout << MESE_HL1("  Presets  ") << std::endl;
        std::cout << std::endl;

        for (const std::string &i: list_presets()) {
            std::cout << MESE_HL2(i) << std::endl;
        }
        std::cout << std::endl;

        std::cout << MESE_HL1("  Settings  ") << std::endl;
        std::cout << std::endl;

        for (const std::string &i: list_settings()) {
            std::cout << MESE_HL2(i) << std::endl;
        }
        std::cout << std::endl;
    }
}

int frontend(int argc, char *argv[]) {
    if (argc < 2) {
        print_info(true, true, false, false);

        return 0;
    } else {
        if (strcmp(argv[1], "init") == 0) {
            if (argc < 4) {
                throw 1; // TODO
            }

            uint64_t player_count = strtoul(argv[2], nullptr, 10);

            Settings settings {get_preset(argv[3], player_count)};
            for (int i = 4; i < argc - 1; i += 2) {
                change_setting(
                    settings, argv[i],
                    player_count, strtod(argv[i + 1], nullptr)
                );
            }

            Game game {player_count, std::move(settings)};

            game.serialize(std::cout);

            return 0;
        } else if (strcmp(argv[1], "alloc") == 0) {
            Game game {std::cin};

            Settings settings = game.periods.back().settings; // copy
            for (int i = 2; i < argc - 1; i += 2) {
                change_setting(
                    settings, argv[i],
                    game.player_count, strtod(argv[i + 1], nullptr)
                );
            }

            game.alloc(std::move(settings));

            game.serialize(std::cout);

            return 0;
        } else if (strcmp(argv[1], "submit") == 0) {
            Game game {std::cin};

            if (argc < 8) {
                throw 1; // TODO
            }

            if (
                (
                    strtod(argv[3], nullptr) == game.now_period
                    || strtod(argv[3], nullptr) == -1
                ) && game.submit(
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
                std::cerr << "ERROR: Decision declined" << std::endl;

                return 1;
            }
        } else if (strcmp(argv[1], "close") == 0) {
            Game game {std::cin};

            if (game.close()) {
                game.serialize(std::cout);

                return 0;
            } else {
                game.serialize(std::cout);
                std::cerr << "ERROR: Period not finished" << std::endl;

                return 1;
            }
        } else if (strcmp(argv[1], "close_force") == 0) {
            Game game {std::cin};

            game.close_force();

            game.serialize(std::cout);

            return 0;
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
        } else if (strcmp(argv[1], "help") == 0) {
            print_info(false, true, true, false);

            return 0;
        } else if (strcmp(argv[1], "ai") == 0) { // hidden
            Game game {std::cin};

            if (argc < 3) {
                throw 1; // TODO
            }

            if (strcmp(argv[3], "daybreak") == 0) {
                ai_setsuna(game, strtoul(argv[2], nullptr, 10), 2.1);
            } else if (strcmp(argv[3], "bouquet") == 0) {
                ai_setsuna(game, strtoul(argv[2], nullptr, 10), 1.5);
            } else if (strcmp(argv[3], "setsuna") == 0) {
                ai_setsuna(game, strtoul(argv[2], nullptr, 10), 1);
            } else if (strcmp(argv[3], "magnet") == 0) {
                ai_setsuna(game, strtoul(argv[2], nullptr, 10), 0.6);
            } else if (strcmp(argv[3], "innocence") == 0) {
                ai_kokoro(game, strtoul(argv[2], nullptr, 10), 2.1);
            } else if (strcmp(argv[3], "kokoro") == 0) {
                ai_kokoro(game, strtoul(argv[2], nullptr, 10), 1.5);
            } else if (strcmp(argv[3], "saika") == 0) {
                ai_kokoro(game, strtoul(argv[2], nullptr, 10), 1);
            } else if (strcmp(argv[3], "moon") == 0) {
                ai_kokoro(game, strtoul(argv[2], nullptr, 10), 0.6);
            } else {
                throw 1; // TODO
            }

            game.serialize(std::cout);

            return 0;
        } else if (strcmp(argv[1], "test") == 0) { // hidden
            test();

            return 0;
        } else if (strcmp(argv[1], "echopen") == 0) { // hidden
            print_info(true, false, false, true);

            return 0;
        } else {
            throw 1; // TODO
        }
    }
}

}

int main(int argc, char *argv[]) {
    try {
        return mese::frontend(argc, argv);
    } catch (...) {
        std::cerr << "ERROR: Internal error" << std::endl;

        return -1;
    }
}
