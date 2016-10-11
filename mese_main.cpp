#include <cstring>

#include "mese.hpp"
#include "mese_print.hpp"

double evaluation(Period &period, Period &last, uint64_t i) {
    double average_capital = 0;
    double average_history_rd = 0;

    for (uint64_t j = 0; j < period.player_count; ++j) {
        if (j != i) {
            average_capital += period.capital[i];
            average_history_rd += period.history_rd[i];
        }
    }

    average_capital /= period.player_count - 1;
    average_history_rd /= period.player_count - 1;

    double value = period.retern[i]
        + 0.5
            * mese::pow(last.retern[i] / last.capital[i], 0.5)
            * (mese::pow(average_capital / period.capital[i], 0.5) - 0.5)
            * period.capital[i]
        + 1.2
            * mese::pow(log(9) - log(period.now_period), 0.5)
            * (mese::pow(average_history_rd / period.history_rd[i], 0.5) - 0.5)
            * period.history_rd[i];

    if (period.now_period >= 8) {
        double max_mpi = -INFINITY;
        for (uint64_t i = 0; i < period.player_count; ++i) {
            if (period.mpi[i] > max_mpi) {
                max_mpi = period.mpi[i];
            }
        }

        value += 100000 * (period.mpi[i] - max_mpi);
    }

    return value;
}

void test() {
    using namespace mese;

    Game game {8, get_preset("modern", 8)};

    // game.print_full(std::cout);

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
    // game.submit(7, 52, 654,    0, 10000,  6000);
    game.submit_best(7, evaluation);
    game.close();

    // game.print_public(std::cout);
    game.print_player(std::cout, 7);

    // game.serialize(std::cout);

    // Game game2 {std::cin};
    // game2.print_public(std::cout);
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
            << "  " << std::hex << mese::BINARY_VER << std::dec
                << ", " << mese::MAX_PLAYER << " players" << std::endl;
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
                << ", total: " << sizeof(mese::Period) << "n"
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

        for (const std::string &i: mese::list_presets()) {
            std::cout << MESE_HL2(i) << std::endl;
        }
        std::cout << std::endl;

        std::cout << MESE_HL1("  Settings  ") << std::endl;
        std::cout << std::endl;

        for (const std::string &i: mese::list_settings()) {
            std::cout << MESE_HL2(i) << std::endl;
        }
        std::cout << std::endl;
    }
}

int frontend(int argc, char *argv[]) {
    using namespace mese;

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
        } else if (strcmp(argv[1], "submit_best")) { // hidden
            Game game {std::cin};

            if (argc < 2) {
                throw 1; // TODO
            }

            game.submit_best(
                strtoul(argv[2], nullptr, 10),
                evaluation
            );

            game.serialize(std::cout);

            return 0;
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

int main(int argc, char *argv[]) {
    try {
        return frontend(argc, argv);
    } catch (...) {
        std::cerr << "ERROR: Internal error" << std::endl;

        return -1;
    }
}
