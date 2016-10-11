#include "mese.hpp"
#include "mese_print.hpp"

namespace mese {

double ai_setsuna(Period &period, Period &last, uint64_t i) {
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
        + (0.2 - 0.4 * period.inventory[i] / period.size[i]) * period.capital[i]
        + 1.2 * (log(8) - log(period.now_period)) * period.history_rd[i];

    if (period.now_period >= 8) {
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

}
