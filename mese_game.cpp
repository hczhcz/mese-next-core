#include "mese.hpp"

namespace mese {

Game::Game(size_t count):
    player_count {count}
{
    Period init {count};

    period.push_back({player_count, 1});
    Period &current {period.back()};

    for (size_t i = 0; i < player_count; ++i) {
        if (!period.back().submit(
            init, i,
            current.setting.demand_ref_price,
            init.size[i] * current.setting.prod_rate_balanced,
            current.setting.demand_ref_mk / player_count,
            init.capital[i] * current.setting.deprecation_rate,
            current.setting.demand_ref_rd / player_count
        )) {
            throw 1; // TODO: initial decision should not break limits
        }
    }

    current.exec(init);
}

}
