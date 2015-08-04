#include "mese.hpp"
#include "mese_print.hpp"

namespace mese {

Game::Game(size_t count, Setting &&_setting):
    player_count {count},
    now_period {1},
    status {0},
    period {{
        player_count,
        std::move(_setting)
    }}
{
    Setting &setting {alloc()};

    for (size_t i = 0; i < player_count; ++i) {
        submit(
            i,
            setting.demand_ref_price,
            period[0].size[i] * setting.prod_rate_initial,
            setting.demand_ref_mk / player_count,
            period[0].capital[i] * setting.deprecation_rate,
            setting.demand_ref_rd / player_count
        );
    }

    if (!close()) {
        throw 1; // TODO: error
    }
}

Setting &Game::alloc(Setting &&_setting) {
    period.push_back({
        player_count,
        period.back(),
        std::move(_setting)
    });

    return period.back().setting;
}

Setting &Game::alloc() {
    Setting setting = period.back().setting; // copy

    return alloc(std::move(setting));
}

bool Game::submit(
    size_t i,
    double price, double prod, double mk, double ci, double rd
) {
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
            // notice: period[0].setting == period[1].setting, see Game::Game
            period[i].print_full([&](auto callback) {
                doc("period_" + std::to_string(i), callback);
            });
        }
    });
}

void Game::print_player_early(std::ostream &stream, size_t i) {
    print(stream, player_count, MESE_PRINT {
        period[now_period].print_player_early(i, [&](auto callback) {
            doc("data_early", callback);
        });
    });
}

void Game::print_player(std::ostream &stream, size_t i) {
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

        period[now_period - 1].print_setting([&](auto callback) {
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

        period[now_period].print_setting([&](auto callback) {
            doc("next_settings", callback);
        });
    });
}

void Game::print_public(std::ostream &stream) {
    print(stream, player_count, MESE_PRINT {
        val("player_count", player_count);
        val("now_period", now_period);
        val("status", status);

        if (now_period >= 3) {
            period[now_period - 2].print_public([&](auto callback) {
                doc("last_data_public", callback);
            });
        }

        period[now_period - 1].print_setting([&](auto callback) {
            doc("settings", callback);
        });
        period[now_period - 1].print_public([&](auto callback) {
            doc("data_public", callback);
        });

        period[now_period].print_setting([&](auto callback) {
            doc("next_settings", callback);
        });
    });
}


}
