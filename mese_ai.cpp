#include "mese.hpp"
#include "mese_print.hpp"

namespace mese {

double e_setsuna(Game &game, uint64_t i) {
    Period &period {game.periods[game.now_period]};

    double value = period.retern[i]
        + (0.2 - 0.4 * period.inventory[i] / period.size[i])
            * period.capital[i]
        + 2 * (1 - exp(-div(period.decisions.ci[i], period.decisions.rd[i], 10000)))
            * (log(game.periods.size() - 1) - log(period.now_period))
            * period.history_rd[i];

    // notice: use ai after period data allocation
    if (period.now_period == game.periods.size() - 1) {
        double max_mpi = -INFINITY;
        for (uint64_t j = 0; j < period.player_count; ++j) {
            if (j != i && period.mpi[j] > max_mpi) {
                max_mpi = period.mpi[j];
            }
        }

        value *= period.mpi[i] - max_mpi;
    }

    return value;
}

double e_acute_predict(Game &game, uint64_t i) {
    Period &period {game.periods[game.now_period]};
    Period &last {game.periods[game.now_period - 1]};

    double value = period.retern[i]
        + (0.2 - 0.4 * period.inventory[i] / period.size[i])
            * period.capital[i]
        + 2 * (1 - exp(-div(period.decisions.ci[i], period.decisions.rd[i], 10000)))
            * (log(game.periods.size() - 1) - log(period.now_period))
            * period.history_rd[i]
        - 0.2 * abs(period.decisions.mk[i] - last.decisions.mk[i])
        - 0.1 * abs(period.decisions.ci[i] - last.decisions.ci[i])
        - 0.2 * abs(period.decisions.rd[i] - last.decisions.rd[i]);

    // notice: use ai after period data allocation
    if (period.now_period == game.periods.size() - 1) {
        double max_mpi = -INFINITY;
        for (uint64_t j = 0; j < period.player_count; ++j) {
            if (j != i && period.mpi[j] > max_mpi) {
                max_mpi = period.mpi[j];
            }
        }

        value *= period.mpi[i] - max_mpi;
    }

    return value;
}

double e_acute(Game &game, uint64_t i) {
    Period &period {game.periods[game.now_period]};

    double value = period.retern[i]
        + (0.2 - 0.4 * period.inventory[i] / period.size[i])
            * period.capital[i]
        + 2 * (1 - exp(-div(period.decisions.ci[i], period.decisions.rd[i], 10000)))
            * (log(game.periods.size() - 1) - log(period.now_period))
            * period.history_rd[i];

    // notice: use ai after period data allocation
    if (period.now_period == game.periods.size() - 1) {
        double max_mpi = -INFINITY;
        for (uint64_t j = 0; j < period.player_count; ++j) {
            if (j != i && period.mpi[j] > max_mpi) {
                max_mpi = period.mpi[j];
            }
        }

        value += 500 * (period.mpi[i] - max_mpi);
    }

    return value;
}

std::array<double, 5> ai_find_best(
    Game &game, uint64_t i,
    double (*evaluator)(Game &game, uint64_t i)
) {
    Period &period {game.periods[game.now_period]};
    Period &last {game.periods[game.now_period - 1]};

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

    double price_min = period.settings.price_min;
    double range[5] {
        period.settings.price_max - price_min,
        last.size[i],
        period.settings.mk_limit / period.player_count,
        period.settings.ci_limit / period.player_count,
        period.settings.rd_limit / period.player_count
    };
    double delta[5];
    for (size_t j = 0; j < 5; ++j) {
        delta[j] = range[j] / init_steps[j];
    }

    auto try_submit = [&](
        double price, double prod, double mk, double ci, double rd
    ) {
        if (game.submit(i, price, prod, mk, ci, rd)) {
            period.exec(last);

            double key = evaluator(game, i);

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
        if (game.submit(i, price, prod, mk, ci, rd)) {
            period.exec(last);

            double key = evaluator(game, i);

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
            delta[j] *= cooling;
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

    return decisions.rbegin()->second;
}

std::array<double, 5> ai_find_best_fast(
    Game &game, uint64_t i,
    double (*evaluator)(Game &game, uint64_t i)
) {
    Period &period {game.periods[game.now_period]};
    Period &last {game.periods[game.now_period - 1]};

    static const uint64_t limits[] {
        256,
        192, 144, 108, 81,
        54, 36, 24, 16,
        8, 4, 2, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
    static const uint64_t init_steps[5] {
        10, 10, 5, 3, 3
    };
    static const double cooling {0.8};

    std::multimap<double, std::array<double, 5>> decisions;

    double price_min = period.settings.price_min;
    double range[5] {
        period.settings.price_max - price_min,
        last.size[i],
        period.settings.mk_limit / period.player_count,
        period.settings.ci_limit / period.player_count,
        period.settings.rd_limit / period.player_count
    };
    double delta[5];
    for (size_t j = 0; j < 5; ++j) {
        delta[j] = range[j] / init_steps[j];
    }

    auto try_submit = [&](
        double price, double prod, double mk, double ci, double rd
    ) {
        if (game.submit(i, price, prod, mk, ci, rd)) {
            period.exec(last);

            double key = evaluator(game, i);

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
        if (game.submit(i, price, prod, mk, ci, rd)) {
            period.exec(last);

            double key = evaluator(game, i);

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
            delta[j] *= cooling;
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

    return decisions.rbegin()->second;
}

void ai_setsuna(Game &game, uint64_t i) {
    Game game_copy = game; // copy

    game_copy.close_force();
    --game_copy.now_period;

    std::array<double, 5> d {
        ai_find_best(game_copy, i, e_setsuna)
    };
    game.submit(i, d[0], d[1], d[2], d[3], d[4]);
}

void ai_acute(Game &game, uint64_t i) {
    Game game_copy = game; // copy

    Period &period {game_copy.periods[game_copy.now_period]};

    game_copy.close_force();
    --game_copy.now_period;

    for (uint64_t j = 0; j < period.player_count; ++j) {
        ai_find_best_fast(game_copy, j, e_acute_predict);
    }

    std::array<double, 5> d {
        ai_find_best(game_copy, i, e_acute)
    };
    game.submit(i, d[0], d[1], d[2], d[3], d[4]);
}

}
