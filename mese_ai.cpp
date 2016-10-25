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
            * max(1 - 1.5 * last.inventory[i] / last.sold[i], 0)
            * period.decisions.ci[i]
        + factor_rd
            * (1 - exp(-div(period.decisions.ci[i], period.decisions.rd[i], 1)))
            * (log(game.periods.size()) - log(game.now_period + 1))
            * period.decisions.rd[i]
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

double ec_play(
    Game &game, uint64_t i,
    double factor_ci, double factor_rd, double factor_inv,
    double factor_mpi
) {
    if (game.now_period == game.periods.size() - 1) {
        return e_setsuna(
            game, i,
            factor_ci, factor_rd, factor_inv
        ) + e_mpi(
            game, i,
            factor_mpi
        );
    } else {
        return e_setsuna(
            game, i,
            factor_ci, factor_rd, factor_inv
        );
    }
}

double ec_predict(
    Game &game, uint64_t i,
    double factor_ci, double factor_rd, double factor_inv,
    double factor_mpi
) {
    if (game.now_period == game.periods.size() - 1) {
        return e_setsuna(
            game, i,
            factor_ci, factor_rd, factor_inv
        ) + e_mpi(
            game, i,
            factor_mpi
        );
    } else {
        return e_setsuna(
            game, i,
            factor_ci, factor_rd, factor_inv
        ) + e_inertia(
            game, i,
            2.5, 1, 2
        );
    }
}

template <class T>
void find_best_global(
    Game &game, uint64_t i,
    std::multimap<double, std::array<double, 5>> &decisions,
    uint64_t limit,
    const double (&range_min)[5],
    const double (&range_max)[5],
    const double (&delta)[5],
    T evaluator
) {
    Period &period {game.periods[game.now_period]};
    Period &last {game.periods[game.now_period - 1]};

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
        double price = range_min[0] + 0.5 * delta[0];
        price < range_max[0];
        price += delta[0]
    ) {
        try_submit(price, 0, 0, 0, 0); // loan limit protection

        for (
            double prod = range_min[1] + 0.5 * delta[1];
            prod < range_max[1];
            prod += delta[1]
        ) {
            for (
                double mk = range_min[2] + 0.5 * delta[2];
                mk < range_max[2];
                mk += delta[2]
            ) {
                for (
                    double ci = range_min[3] + 0.5 * delta[3];
                    ci < range_max[3];
                    ci += delta[3]
                ) {
                    for (
                        double rd = range_min[4] + 0.5 * delta[4];
                        rd < range_max[4];
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

    for (auto &decision: old_decisions) {
        std::multimap<double, std::array<double, 5>>::iterator iter {
            decisions.insert({decision.first, decision.second})
        };

        std::array<double, 5> &d {iter->second};
        try_replace(iter, d[0] - delta[0], d[1], d[2], d[3], d[4]);
        try_replace(iter, d[0] + delta[0], d[1], d[2], d[3], d[4]);
        d = iter->second;
        try_replace(iter, d[0], d[1] - delta[1], d[2], d[3], d[4]);
        try_replace(iter, d[0], d[1] + delta[1], d[2], d[3], d[4]);
        d = iter->second;
        try_replace(iter, d[0], d[1], d[2] - delta[2], d[3], d[4]);
        try_replace(iter, d[0], d[1], d[2] + delta[2], d[3], d[4]);
        d = iter->second;
        try_replace(iter, d[0], d[1], d[2], d[3] - delta[3], d[4]);
        try_replace(iter, d[0], d[1], d[2], d[3] + delta[3], d[4]);
        d = iter->second;
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
    Period &period {game.periods[game.now_period]};
    Period &last {game.periods[game.now_period - 1]};

    std::multimap<double, std::array<double, 5>> decisions;

    double range_min[5] {
        period.settings.price_min,
        0,
        0,
        0,
        0
    };

    double range_max[5] {
        period.settings.price_max,
        last.size[i],
        period.settings.mk_limit / game.player_count,
        period.settings.ci_limit / game.player_count,
        period.settings.rd_limit / game.player_count
    };

    double fund = last.cash[i] - last.loan[i]
        + period.settings.loan_limit / game.player_count
        + period.settings.deprecation_rate * last.capital[i];
    double range_limit[5] {
        last.average_price * 3,
        last.size[i],
        0.5 * fund,
        0.5 * fund,
        0.5 * fund
    };

    double delta[5];
    for (uint64_t j = 0; j < 5; ++j) {
        delta[j] = min(
            range_max[j] - range_min[j],
            max(
                0.25 * (range_max[j] - range_min[j]),
                range_limit[j]
            )
        ) / steps[j];
    }

    find_best_global(
        game, i,
        decisions,
        limits[0], range_min, range_max, delta,
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
                return ec_play(game_copy, i, 0.1, factor_rd, 0, 1);
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
                    return ec_predict(game_copy, j, 0.1, 1, 4, 0.2);
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
                return ec_play(game_copy, i, 0.1, factor_rd, 4, 0.5);
            }
        )
    };

    game.submit(i, d[0], d[1], d[2], d[3], d[4]);
}

void ai_melody(Game &game, uint64_t i) {
    Game game_copy = game; // copy

    uint64_t start_period = game_copy.now_period;

    game_copy.close_force();

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
                        return ec_play(game_copy, j, 0.1, 1, 4, 0.2);
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
                        return ec_play(game_copy, i, 0.1, factor_rd, 0, 1);
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
                return ec_play(game_copy, i, 0.1, best_factor_rd, 0, 1);
            }
        )
    };

    game.submit(i, d[0], d[1], d[2], d[3], d[4]);
}

void ai_spica(Game &game, uint64_t i) {
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
                        if (game_copy.now_period > start_period) {
                            return ec_play(game_copy, j, 0.1, 1, 4, 0.2);
                        } else {
                            return ec_predict(game_copy, j, 0.1, 1, 4, 0.2);
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
                        return ec_play(game_copy, i, 0.1, factor_rd, 4, 0.5);
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
                return ec_play(game_copy, i, 0.1, best_factor_rd, 4, 0.5);
            }
        )
    };

    game.submit(i, d[0], d[1], d[2], d[3], d[4]);
}

}
