#include "mese.hpp"

namespace mese {

Game::Game(size_t count):
    player_count {count},
    now_period {1},
    period {Period {player_count}}
{
    Setting setting {alloc()};

    for (size_t i = 0; i < player_count; ++i) {
        submit(
            i,
            setting.demand_ref_price,
            period[0].size[i] * setting.prod_rate_balanced,
            setting.demand_ref_mk / player_count,
            period[0].capital[i] * setting.deprecation_rate,
            setting.demand_ref_rd / player_count
        );
    }

    if (!close()) {
        throw 1; // TODO: error
    }
}

Setting &Game::alloc() {
    period.push_back({player_count, period.size()});

    return period.back().setting;
}

bool Game::submit(
    size_t i,
    double price, double prod, double mk, double ci, double rd
) {
    return period[now_period].submit(
        period[now_period - 1], i,
        price, prod, mk, ci, rd
    );
}

bool Game::close() {
    if (period[now_period].ready()) {
        period[now_period].exec(period[now_period - 1]);
        ++now_period;

        return true;
    } else {
        return false;
    }
}

}
