#include "mese.hpp"
#include "mese_print.hpp"

namespace mese {

const uint64_t limits_slow[] {
    256, 224, 192, 160,
    128, 112, 96, 80,
    64, 56, 48, 40,
    32, 28, 24, 20,
    16, 14, 12, 10,
    8, 7, 6, 5,
    4, 4, 3, 3,
    2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
const uint64_t limits_fast[] {
    16, 14, 12, 10,
    8, 7, 6, 5,
    4, 4, 3, 3,
    2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
const uint64_t steps_slow[5] {
    15, 10, 5, 5, 5
};
const uint64_t steps_fast[5] {
    8, 5, 3, 3, 3
};
const double cooling_default {0.9};

double e_setsuna(
    Game &game, uint64_t i,
    double factor_ci, double factor_rd, double factor_inv
) {
    Period &period {game.periods[game.now_period]};
    Period &last {game.periods[game.now_period - 1]};

    return period.retern[i]
        + factor_ci
            * (1 - 2 * last.inventory[i] / last.size[i])
            * period.capital[i]
        + factor_rd
            * min(div(period.decisions.ci[i], period.decisions.rd[i], 1), 1)
            * (log(game.periods.size()) - log(game.now_period + 1))
            * period.history_rd[i]
        + factor_inv
            * period.inventory[i];
}

double e_inertia(
    Game &game, uint64_t i,
    double factor_mk, double factor_ci, double factor_rd
) {
    Period &period {game.periods[game.now_period]};
    Period &last {game.periods[game.now_period - 1]};

    return -div(
        factor_mk
            * (period.decisions.mk[i] - last.decisions.mk[i])
            * (period.decisions.mk[i] - last.decisions.mk[i])
        + factor_ci
            * (period.decisions.ci[i] - last.decisions.ci[i])
            * (period.decisions.ci[i] - last.decisions.ci[i])
        + factor_rd
            * (period.decisions.rd[i] - last.decisions.rd[i])
            * (period.decisions.rd[i] - last.decisions.rd[i]),
        last.cash[i],
        0
    );
}

double e_mpi(
    Game &game, uint64_t i,
    double factor_mpi
) {
    Period &period {game.periods[game.now_period]};

    double max_mpi = game.player_count > 1 ? -INFINITY : 0;

    for (uint64_t j = 0; j < game.player_count; ++j) {
        if (j != i && period.mpi[j] > max_mpi) {
            max_mpi = period.mpi[j];
        }
    }

    return factor_mpi
        * period.settings.mpi_retern_factor / game.player_count
        * (period.mpi[i] - max_mpi);
}

template <class T>
void find_best_global(
    Game &game, uint64_t i,
    std::multimap<double, std::array<double, 5>> &decisions,
    uint64_t limit,
    const uint64_t (&steps)[5],
    double (&delta)[5],
    T evaluator
) {
    Period &period {game.periods[game.now_period]};
    Period &last {game.periods[game.now_period - 1]};

    double range[5] {
        period.settings.price_max - period.settings.price_min,
        last.size[i],
        period.settings.mk_limit / game.player_count,
        period.settings.ci_limit / game.player_count,
        period.settings.rd_limit / game.player_count
    };

    for (uint64_t j = 0; j < 5; ++j) {
        delta[j] = range[j] / steps[j];
    }

    auto try_submit = [&](
        double price, double prod, double mk, double ci, double rd
    ) {
        if (game.submit(i, price, prod, mk, ci, rd)) {
            period.exec(last);

            double key = evaluator();

            if (decisions.size() == limit) {
                decisions.erase(decisions.begin());
            }

            decisions.insert({
                key, {{price, prod, mk, ci, rd}}
            });
        }
    };

    for (
        double price = period.settings.price_min + 0.5 * delta[0];
        price < range[0] + period.settings.price_min;
        price += delta[0]
    ) {
        try_submit(price, 0, 0, 0, 0); // loan limit protection

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
}

template <class T>
void find_best_local(
    Game &game, uint64_t i,
    std::multimap<double, std::array<double, 5>> &decisions,
    const double (&delta)[5],
    T evaluator
) {
    Period &period {game.periods[game.now_period]};
    Period &last {game.periods[game.now_period - 1]};

    auto try_replace = [&](
        std::multimap<double, std::array<double, 5>>::iterator &iter,
        double price, double prod, double mk, double ci, double rd
    ) {
        if (game.submit(i, price, prod, mk, ci, rd)) {
            period.exec(last);

            double key = evaluator();

            if (key > iter->first) {
                decisions.erase(iter);

                iter = decisions.insert({
                    key, {{price, prod, mk, ci, rd}}
                });
            }
        }
    };

    std::multimap<double, std::array<double, 5>> old_decisions;
    old_decisions.swap(decisions);

    for (auto &j: old_decisions) {
        std::array<double, 5> &d {j.second};

        std::multimap<double, std::array<double, 5>>::iterator iter {
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

template <uint64_t iter_count, class T>
std::array<double, 5> find_best(
    Game &game, uint64_t i,
    const uint64_t (&limits)[iter_count],
    const uint64_t (&steps)[5],
    double cooling,
    T evaluator
) {
    std::multimap<double, std::array<double, 5>> decisions;

    double delta[5];

    find_best_global(
        game, i,
        decisions,
        limits[0], steps, delta,
        evaluator
    );

    for (uint64_t limit: limits) {
        while (decisions.size() > limit) {
            decisions.erase(decisions.begin());
        }

        for (uint64_t j = 0; j < 5; ++j) {
            delta[j] *= cooling;
        }

        find_best_local(
            game, i,
            decisions,
            delta,
            evaluator
        );
    }

    if (decisions.size() > 0) {
        return decisions.rbegin()->second; // copy
    } else {
        return {{-1, 0, 0, 0, 0}}; // error
    }
}

void ai_setsuna(Game &game, uint64_t i, double factor_rd) {
    Game game_copy = game; // copy

    game_copy.close_force();
    --game_copy.now_period;

    std::array<double, 5> d {
        find_best(
            game_copy, i,
            limits_slow, steps_slow, cooling_default,
            [&]() {
                if (game_copy.now_period == game_copy.periods.size() - 1) {
                    return e_setsuna(
                        game_copy, i,
                        0.2, factor_rd, 0
                    ) + e_mpi(
                        game_copy, i,
                        1
                    );
                } else {
                    return e_setsuna(
                        game_copy, i,
                        0.2, factor_rd, 0
                    );
                }
            }
        )
    };

    game.submit(i, d[0], d[1], d[2], d[3], d[4]);
}

void ai_kokoro(Game &game, uint64_t i, double factor_rd) {
    Game game_copy = game; // copy

    game_copy.status = 0;
    game_copy.close_force();
    --game_copy.now_period;

    for (uint64_t j = 0; j < game_copy.player_count; ++j) {
        std::array<double, 5> d {
            find_best(
                game_copy, j,
                limits_fast, steps_fast, cooling_default,
                [&]() {
                    if (game_copy.now_period == game_copy.periods.size() - 1) {
                        return e_setsuna(
                            game_copy, j,
                            0.2, 1, 4
                        ) + e_mpi(
                            game_copy, j,
                            0.2
                        );
                    } else {
                        return e_setsuna(
                            game_copy, j,
                            0.2, 1, 4
                        ) + e_inertia(
                            game_copy, j,
                            2.5, 1, 2
                        );
                    }
                }
            )
        };

        game_copy.submit(j, d[0], d[1], d[2], d[3], d[4]);
    }

    std::array<double, 5> d {
        find_best(
            game_copy, i,
            limits_slow, steps_slow, cooling_default,
            [&]() {
                if (game_copy.now_period == game_copy.periods.size() - 1) {
                    return e_setsuna(
                        game_copy, i,
                        0.2, factor_rd, 4
                    ) + e_mpi(
                        game_copy, i,
                        0.5
                    );
                } else {
                    return e_setsuna(
                        game_copy, i,
                        0.2, factor_rd, 4
                    );
                }
            }
        )
    };

    game.submit(i, d[0], d[1], d[2], d[3], d[4]);
}

void ai_spica(Game &game, uint64_t i, double /* factor_rd */) {
    Game game_copy = game; // copy

    uint64_t start_period = game_copy.now_period;

    while (game_copy.now_period < game_copy.periods.size()) {
        game_copy.status = 0;
        game_copy.close_force();
        --game_copy.now_period;

        for (uint64_t j = 0; j < game_copy.player_count; ++j) {
            std::array<double, 5> d {
                find_best(
                    game_copy, j,
                    limits_fast, steps_fast, cooling_default,
                    [&]() {
                        if (game_copy.now_period == game_copy.periods.size() - 1) {
                            return e_setsuna(
                                game_copy, j,
                                0.2, 1, 4
                            ) + e_mpi(
                                game_copy, j,
                                0.2
                            );
                        } else if (game_copy.now_period > start_period) {
                            return e_setsuna(
                                game_copy, j,
                                0.2, 1, 4
                            );
                        } else {
                            return e_setsuna(
                                game_copy, j,
                                0.2, 1, 4
                            ) + e_inertia(
                                game_copy, j,
                                2.5, 1, 2
                            );
                        }
                    }
                )
            };

            game_copy.submit(j, d[0], d[1], d[2], d[3], d[4]);
        }

        game_copy.close_force();
    }

    game_copy.now_period = start_period;

    double best_evaluation;
    double best_factor_rd;

    for (double factor_rd = 0; factor_rd < 3; factor_rd += 0.25) {
        while (game_copy.now_period < game_copy.periods.size()) {
            Period &period {game_copy.periods[game_copy.now_period]};
            Period &last {game_copy.periods[game_copy.now_period - 1]};

            for (uint64_t j = 0; j < game_copy.player_count; ++j) {
                game_copy.submit(
                    j,
                    period.decisions.price[j],
                    period.decisions.prod[j],
                    period.decisions.mk[j],
                    period.decisions.ci[j],
                    period.decisions.rd[j]
                );
            }

            std::array<double, 5> d {
                find_best(
                    game_copy, i,
                    limits_fast, steps_fast, cooling_default,
                    [&]() {
                        if (game_copy.now_period == game_copy.periods.size() - 1) {
                            return e_setsuna(
                                game_copy, i,
                                0.2, factor_rd, 4
                            ) + e_mpi(
                                game_copy, i,
                                0.5
                            );
                        } else {
                            return e_setsuna(
                                game_copy, i,
                                0.2, factor_rd, 4
                            );
                        }
                    }
                )
            };

            game_copy.submit(i, d[0], d[1], d[2], d[3], d[4]);

            period.exec(last);
            ++game_copy.now_period;
        }

        game_copy.now_period = game_copy.periods.size() - 1;

        double evaluation = e_mpi(
            game_copy, i,
            1
        );

        if (evaluation > best_evaluation) {
            best_evaluation = evaluation;
            best_factor_rd = factor_rd;
        }

        game_copy.now_period = start_period;
    }

    std::array<double, 5> d {
        find_best(
            game_copy, i,
            limits_slow, steps_slow, cooling_default,
            [&]() {
                if (game_copy.now_period == game_copy.periods.size() - 1) {
                    return e_setsuna(
                        game_copy, i,
                        0.2, best_factor_rd, 4
                    ) + e_mpi(
                        game_copy, i,
                        0.5
                    );
                } else {
                    return e_setsuna(
                        game_copy, i,
                        0.2, best_factor_rd, 4
                    );
                }
            }
        )
    };

    game.submit(i, d[0], d[1], d[2], d[3], d[4]);
}

}
