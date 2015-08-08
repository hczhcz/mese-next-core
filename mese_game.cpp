#include "mese.hpp"
#include "mese_print.hpp"

namespace mese {

Game::Game(size_t count, Settings &&_settings):
    player_count {count},
    now_period {1},
    status {0},
    period {{
        player_count,
        std::move(_settings)
    }}
{
    if (player_count > MAX_PLAYER) {
        throw 1; // TODO
    }

    Settings &settings {alloc()};

    for (size_t i = 0; i < player_count; ++i) {
        submit(
            i,
            settings.demand_ref_price,
            period[0].size[i] * settings.prod_rate_initial,
            settings.demand_ref_mk / player_count,
            period[0].capital[i] * settings.deprecation_rate,
            settings.demand_ref_rd / player_count
        );
    }

    if (!close()) {
        throw 1; // TODO: error
    }
}

Game::Game(std::istream &stream):
    player_count {},
    now_period {},
    status {},
    period {}
{
    size_t vsize;

    stream.read(reinterpret_cast<char *>(&player_count), sizeof(player_count));
    stream.read(reinterpret_cast<char *>(&now_period), sizeof(now_period));
    stream.read(reinterpret_cast<char *>(&status), sizeof(status));
    stream.read(reinterpret_cast<char *>(&vsize), sizeof(vsize));

    for (; vsize > 0; --vsize) {
        period.push_back({stream});
    }
}

Settings &Game::alloc(Settings &&_settings) {
    period.push_back({
        player_count,
        period.back(),
        std::move(_settings)
    });

    return period.back().settings;
}

Settings &Game::alloc() {
    Settings settings = period.back().settings; // copy

    return alloc(std::move(settings));
}

bool Game::submit(
    size_t i,
    double price, double prod, double mk, double ci, double rd
) {
    if (i >= player_count) {
        throw 1; // TODO
    }

    if (now_period >= period.size()) {
        throw 1; // TODO
    }

    if (period[now_period].submit(
        period[now_period - 1], i,
        price, prod, mk, ci, rd
    )) {
        set_status(i);

        return true;
    } else {
        return false;
    }
}

bool Game::close() {
    if (now_period >= period.size()) {
        throw 1; // TODO
    }

    if (ready()) {
        period[now_period].exec(period[now_period - 1]);
        ++now_period;
        status = 0;

        return true;
    } else {
        return false;
    }
}

void Game::print_full(std::ostream &stream) {
    print(stream, player_count, MESE_PRINT {
        val("player_count", player_count);
        val("now_period", now_period);
        val("status", status);

        for (size_t i = 1; i < period.size(); ++i) {
            // notice: period[0].settings == period[1].settings, see Game::Game
            period[i].print_full([&](auto callback) {
                doc("period_" + std::to_string(i), callback);
            });
        }
    });
}

void Game::print_player_early(std::ostream &stream, size_t i) {
    if (i >= player_count) {
        throw 1; // TODO
    }

    if (now_period >= period.size()) {
        throw 1; // TODO
    }

    print(stream, player_count, MESE_PRINT {
        val("status", status);

        period[now_period].print_player_early(i, [&](auto callback) {
            doc("data_early", callback);
        });
    });
}

void Game::print_player(std::ostream &stream, size_t i) {
    if (i >= player_count) {
        throw 1; // TODO
    }

    if (now_period - 1 >= period.size()) {
        throw 1; // TODO
    }

    print(stream, player_count, MESE_PRINT {
        val("player_count", player_count);
        val("now_period", now_period);
        val("status", status);

        if (now_period >= 3) {
            period[now_period - 2].print_player_early(i, [&](auto callback) {
                doc("last_data_early", callback);
            });
            period[now_period - 2].print_player(i, [&](auto callback) {
                doc("last_data_player", callback);
            });
            period[now_period - 2].print_public([&](auto callback) {
                doc("last_data_public", callback);
            });
        }

        period[now_period - 1].print_settings([&](auto callback) {
            doc("settings", callback);
        });
        period[now_period - 1].print_player_early(i, [&](auto callback) {
            doc("data_early", callback);
        });
        period[now_period - 1].print_player(i, [&](auto callback) {
            doc("data_player", callback);
        });
        period[now_period - 1].print_public([&](auto callback) {
            doc("data_public", callback);
        });

        if (now_period < period.size()) {
            period[now_period].print_settings([&](auto callback) {
                doc("next_settings", callback);
            });
        }
    });
}

void Game::print_public(std::ostream &stream) {
    if (now_period - 1 >= period.size()) {
        throw 1; // TODO
    }

    print(stream, player_count, MESE_PRINT {
        val("player_count", player_count);
        val("now_period", now_period);
        val("status", status);

        if (now_period >= 3) {
            period[now_period - 2].print_public([&](auto callback) {
                doc("last_data_public", callback);
            });
        }

        period[now_period - 1].print_settings([&](auto callback) {
            doc("settings", callback);
        });
        period[now_period - 1].print_public([&](auto callback) {
            doc("data_public", callback);
        });

        if (now_period < period.size()) {
            period[now_period].print_settings([&](auto callback) {
                doc("next_settings", callback);
            });
        }
    });
}

void Game::serialize(std::ostream &stream) {
    size_t vsize {period.size()};

    stream.write(reinterpret_cast<const char *>(&player_count), sizeof(player_count));
    stream.write(reinterpret_cast<const char *>(&now_period), sizeof(now_period));
    stream.write(reinterpret_cast<const char *>(&status), sizeof(status));
    stream.write(reinterpret_cast<const char *>(&vsize), sizeof(vsize));

    for (auto &i: period) {
        i.serialize(stream);
    }
}

}
