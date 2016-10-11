#include "mese.hpp"
#include "mese_print.hpp"

namespace mese {

Game::Game(uint64_t count, Settings &&_settings):
    player_count {count},
    now_period {1},
    status {0},
    periods {{
        player_count,
        std::move(_settings)
    }}
{
    if (player_count > MAX_PLAYER) {
        throw 1; // TODO
    }

    Settings &settings {alloc()};

    for (uint64_t i = 0; i < player_count; ++i) {
        submit(
            i,
            settings.demand_ref_price,
            periods[0].size[i] * settings.prod_rate_initial,
            settings.demand_ref_mk / player_count,
            periods[0].capital[i] * settings.deprecation_rate,
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
    periods {}
{
    uint64_t vtag;
    uint64_t vsize;

    stream.read(
        reinterpret_cast<char *>(&vtag), sizeof(vtag)
    );
    stream.read(
        reinterpret_cast<char *>(&player_count), sizeof(player_count)
    );
    stream.read(
        reinterpret_cast<char *>(&now_period), sizeof(now_period)
    );
    stream.read(
        reinterpret_cast<char *>(&status), sizeof(status)
    );
    stream.read(
        reinterpret_cast<char *>(&vsize), sizeof(vsize)
    );

    for (; vsize > 0; --vsize) {
        periods.push_back({stream});
    }

    if (vtag != BINARY_VER) {
        throw 1; // TODO
    }
}

Settings &Game::alloc(Settings &&_settings) {
    periods.push_back({
        player_count,
        periods.back(),
        std::move(_settings)
    });

    return periods.back().settings;
}

Settings &Game::alloc() {
    Settings settings = periods.back().settings; // copy

    return alloc(std::move(settings));
}

bool Game::submit(
    uint64_t i,
    double price, double prod, double mk, double ci, double rd
) {
    if (i >= player_count) {
        throw 1; // TODO
    }

    if (now_period >= periods.size()) {
        throw 1; // TODO
    }

    if (periods[now_period].submit(
        periods[now_period - 1], i,
        price, prod, mk, ci, rd
    )) {
        set_status(i);

        return true;
    } else {
        unset_status(i);

        return false;
    }
}

void Game::submit_best(
    uint64_t i,
    double (*evaluation)(Period &period, uint64_t i)
) {
    uint64_t old_status = status;

    close_force();
    --now_period;
    status = old_status;

    static const uint64_t limits[] {
        1024,
        768, 576, 432, 324, 243,
        162, 108, 72, 48, 32,
        16, 8, 4, 2, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
    static const uint64_t init_steps[5] {
        15, 10, 5, 5, 5
    };
    static const double cooling {0.8};

    std::multimap<double, std::array<double, 5>> decisions;

    double price_min = periods[now_period].settings.price_min;
    double range[5] {
        periods[now_period].settings.price_max - price_min,
        periods[now_period - 1].size[i],
        periods[now_period].settings.mk_limit / player_count,
        periods[now_period].settings.ci_limit / player_count,
        periods[now_period].settings.rd_limit / player_count
    };
    double delta[5];
    for (size_t j = 0; j < 5; ++j) {
        delta[j] = range[j] / init_steps[j];
    }

    auto try_submit = [&](
        double price, double prod, double mk, double ci, double rd
    ) {
        if (submit(i, price, prod, mk, ci, rd)) {
            periods[now_period].exec(periods[now_period - 1]);

            double key = evaluation(periods[now_period], i);

            if (decisions.size() == limits[0]) {
                decisions.erase(decisions.begin());
            }

            decisions.insert({
                key, {{price, prod, mk, ci, rd}}
            });
        }
    };

    auto try_replace = [&](
        decltype(decisions)::iterator &iter,
        double price, double prod, double mk, double ci, double rd
    ) {
        if (submit(i, price, prod, mk, ci, rd)) {
            periods[now_period].exec(periods[now_period - 1]);

            double key = evaluation(periods[now_period], i);

            if (key > iter->first) {
                decisions.erase(iter);

                iter = decisions.insert({
                    key, {{price, prod, mk, ci, rd}}
                });
            }
        }
    };

    for (
        double price = price_min + 0.5 * delta[0];
        price < range[0] + price_min;
        price += delta[0]
    ) {
        for (
            double prod = 0.5 * delta[1];
            prod < range[1];
            prod += delta[1]
        ) {
            for (
                double mk = 0.5 * delta[2];
                mk < range[2];
                mk += delta[2]
            ) {
                for (
                    double ci = 0.5 * delta[3];
                    ci < range[3];
                    ci += delta[3]
                ) {
                    for (
                        double rd = 0.5 * delta[4];
                        rd < range[4];
                        rd += delta[4]
                    ) {
                        try_submit(price, prod, mk, ci, rd);
                    }
                }
            }
        }
    }

    for (uint64_t limit: limits) {
        while (decisions.size() > limit) {
            decisions.erase(decisions.begin());
        }


        for (size_t j = 0; j < 5; ++j) {
            delta[j] = cooling * delta[j];
        }

        std::multimap<double, std::array<double, 5>> old_decisions;
        old_decisions.swap(decisions);

        for (auto &j: old_decisions) {
            std::array<double, 5> &d {j.second};

            decltype(decisions)::iterator iter {
                decisions.insert({j.first, j.second})
            };

            try_replace(iter, d[0] - delta[0], d[1], d[2], d[3], d[4]);
            try_replace(iter, d[0] + delta[0], d[1], d[2], d[3], d[4]);
            try_replace(iter, d[0], d[1] - delta[1], d[2], d[3], d[4]);
            try_replace(iter, d[0], d[1] + delta[1], d[2], d[3], d[4]);
            try_replace(iter, d[0], d[1], d[2] - delta[2], d[3], d[4]);
            try_replace(iter, d[0], d[1], d[2] + delta[2], d[3], d[4]);
            try_replace(iter, d[0], d[1], d[2], d[3] - delta[3], d[4]);
            try_replace(iter, d[0], d[1], d[2], d[3] + delta[3], d[4]);
            try_replace(iter, d[0], d[1], d[2], d[3], d[4] - delta[4]);
            try_replace(iter, d[0], d[1], d[2], d[3], d[4] + delta[4]);
        }
    }

    std::array<double, 5> &d {decisions.rbegin()->second};
    submit(i, d[0], d[1], d[2], d[3], d[4]);
}

bool Game::close() {
    if (now_period >= periods.size()) {
        throw 1; // TODO
    }

    if (ready()) {
        periods[now_period].exec(periods[now_period - 1]);
        ++now_period;
        status = 0;

        return true;
    } else {
        return false;
    }
}

void Game::close_force() {
    if (now_period >= periods.size()) {
        throw 1; // TODO
    }

    for (uint64_t i = 0; i < player_count; ++i) {
        if (!get_status(i)) {
            double last_price = max(
                min(
                    periods[now_period - 1].decisions.price[i],
                    periods[now_period].settings.price_max
                ),
                periods[now_period].settings.price_min
            );
            double last_prod = max(
                periods[now_period - 1].prod_rate[i],
                periods[now_period].settings.prod_rate_balanced
            ) * periods[now_period - 1].size[i];
            double last_mk = min(
                periods[now_period - 1].decisions.mk[i],
                periods[now_period].settings.mk_limit / player_count
            );
            double last_ci = min(
                periods[now_period - 1].decisions.ci[i],
                periods[now_period].settings.ci_limit / player_count
            );
            double deprecation = periods[now_period].settings.deprecation_rate
                * periods[now_period - 1].capital[i];
            double last_rd = min(
                periods[now_period - 1].decisions.rd[i],
                periods[now_period].settings.rd_limit / player_count
            );

            submit(
                i, last_price, last_prod, last_mk, last_ci, last_rd
            ) || submit(
                i, last_price, last_prod, last_mk, last_ci, 0
            ) || submit(
                i, last_price, last_prod, last_mk, deprecation, 0
            ) || submit(
                i, last_price, last_prod, 0, deprecation, 0
            ) || submit(
                i, last_price, 0, 0, 0, 0
            );
        }
    }

    periods[now_period].exec(periods[now_period - 1]);
    ++now_period;
    status = 0;
}

void Game::print_full(std::ostream &stream) {
    print(stream, player_count, MESE_PRINT {
        val("player_count", player_count);
        val("now_period", now_period);
        val("status", status);

        for (uint64_t i = 1; i < periods.size(); ++i) {
            // notice: periods[0].settings == periods[1].settings, see Game::Game
            periods[i].print_full([&](auto callback) {
                doc("period_" + std::to_string(i), callback);
            });
        }
    });
}

void Game::print_player_early(std::ostream &stream, uint64_t i) {
    if (i >= player_count) {
        throw 1; // TODO
    }

    if (now_period >= periods.size()) {
        throw 1; // TODO
    }

    print(stream, player_count, MESE_PRINT {
        val("status", status);

        periods[now_period].print_decisions(i, [&](auto callback) {
            doc("decisions", callback);
        });
        periods[now_period].print_player_early(i, [&](auto callback) {
            doc("data_early", callback);
        });
    });
}

void Game::print_player(std::ostream &stream, uint64_t i) {
    if (i >= player_count) {
        throw 1; // TODO
    }

    if (now_period - 1 >= periods.size()) {
        throw 1; // TODO
    }

    print(stream, player_count, MESE_PRINT {
        val("player_count", player_count);
        val("now_period", now_period);
        val("status", status);

        if (now_period >= 3) {
            // periods[now_period - 2].print_decisions(i, [&](auto callback) {
            //     doc("last_decisions", callback);
            // });
            // periods[now_period - 2].print_player_early(i, [&](auto callback) {
            //     doc("last_data_early", callback);
            // });
            periods[now_period - 2].print_player(i, [&](auto callback) {
                doc("last_data", callback);
            });
            periods[now_period - 2].print_public([&](auto callback) {
                doc("last_data_public", callback);
            });
        }

        periods[now_period - 1].print_settings([&](auto callback) {
            doc("settings", callback);
        });
        periods[now_period - 1].print_decisions(i, [&](auto callback) {
            doc("decisions", callback);
        });
        periods[now_period - 1].print_player_early(i, [&](auto callback) {
            doc("data_early", callback);
        });
        periods[now_period - 1].print_player(i, [&](auto callback) {
            doc("data", callback);
        });
        periods[now_period - 1].print_public([&](auto callback) {
            doc("data_public", callback);
        });

        if (now_period < periods.size()) {
            periods[now_period].print_settings([&](auto callback) {
                doc("next_settings", callback);
            });
        }
    });
}

void Game::print_public(std::ostream &stream) {
    if (now_period - 1 >= periods.size()) {
        throw 1; // TODO
    }

    print(stream, player_count, MESE_PRINT {
        val("player_count", player_count);
        val("now_period", now_period);
        val("status", status);

        if (now_period >= 3) {
            periods[now_period - 2].print_public([&](auto callback) {
                doc("last_data_public", callback);
            });
        }

        periods[now_period - 1].print_settings([&](auto callback) {
            doc("settings", callback);
        });
        periods[now_period - 1].print_public([&](auto callback) {
            doc("data_public", callback);
        });

        if (now_period < periods.size()) {
            periods[now_period].print_settings([&](auto callback) {
                doc("next_settings", callback);
            });
        }
    });
}

void Game::serialize(std::ostream &stream) {
    static_assert(sizeof(uint64_t) == 8, "");
    static_assert(sizeof(double) == 8, "");
    static_assert(sizeof(Period) == 13248, "");

    uint64_t vtag {BINARY_VER};
    uint64_t vsize {periods.size()};

    stream.write(
        reinterpret_cast<const char *>(&vtag), sizeof(vtag)
    );
    stream.write(
        reinterpret_cast<const char *>(&player_count), sizeof(player_count)
    );
    stream.write(
        reinterpret_cast<const char *>(&now_period), sizeof(now_period)
    );
    stream.write(
        reinterpret_cast<const char *>(&status), sizeof(status)
    );
    stream.write(
        reinterpret_cast<const char *>(&vsize), sizeof(vsize)
    );

    for (Period &period: periods) {
        period.serialize(stream);
    }
}

}
