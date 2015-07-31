#include "mese.hpp"

namespace mese {

Game::Game(size_t count, Setting &&_setting):
    player_count {count},
    now_period {1},
    period {{
        player_count,
        std::move(_setting)
    }}
{
    Setting setting {alloc()};

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
    Setting setting {period.back().setting};

    return alloc(std::move(setting));
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
