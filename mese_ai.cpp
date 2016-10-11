#include "mese.hpp"
#include "mese_print.hpp"

namespace mese {

double ai_setsuna(Period &period, uint64_t i) {
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
