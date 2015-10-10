#include "mese.hpp"

namespace mese {

#define MESE_SETTING(v_classic, v_imese, v_modern) \
    ( \
        id == PresetId::classic ? v_classic : ( \
            id == PresetId::imese ? v_imese : ( \
                id == PresetId::modern ? v_modern : ( \
                    NAN \
                ) \
            ) \
        ) \
    )

Settings get_preset(PresetId id, uint64_t player_count) {
    if (player_count > MAX_PLAYER) {
        throw 1; // TODO
    }

    Settings settings {};

    settings.price_max = 99;
    settings.price_min = MESE_SETTING(8, 18, 12);
    settings.mk_limit = MESE_SETTING(99999, 15000, 15000) * player_count;
    settings.ci_limit = MESE_SETTING(99999, 15000, 15000) * player_count;
    settings.rd_limit = MESE_SETTING(99999, 15000, 15000) * player_count;
    settings.loan_limit = MESE_SETTING(50000, 30000, 30000) * player_count;

    settings.prod_rate_initial = MESE_SETTING(0.75, 0.75, 0.8);
    settings.prod_rate_balanced = 0.8;
    settings.prod_rate_pow = 2;
    settings.prod_cost_factor_rate_over = MESE_SETTING(69, 69, 63);
    settings.prod_cost_factor_rate_under = MESE_SETTING(138, 138, 63);
    settings.prod_cost_factor_size = 15;
    settings.prod_cost_factor_const = 3;

    settings.unit_fee = 40;
    settings.deprecation_rate = 0.05;

    settings.initial_cash = MESE_SETTING(1837.5, 1837.5, 1750) * player_count;
    settings.initial_capital = 21000 * player_count;

    settings.interest_rate_cash = MESE_SETTING(0.0125, 0.0125, 0.025);
    settings.interest_rate_loan = MESE_SETTING(0.025, 0.025, 0.05);
    settings.inventory_fee = 1;
    settings.tax_rate = 0.25;

    settings.mk_overload = 2100 * player_count;
    settings.mk_compression = 0.25;

    settings.demand = MESE_SETTING(62.5, 62.5, 70) * player_count;
    settings.demand_price = 1;
    settings.demand_mk = MESE_SETTING(5.3, 5.3, 5);
    settings.demand_rd = 1;

    settings.demand_ref_price = 30;
    settings.demand_ref_mk = 1050 * player_count;
    settings.demand_ref_rd = MESE_SETTING(393.75, 393.75, 420) * player_count;
    settings.demand_pow_price = 1;
    settings.demand_pow_mk = 0.5;
    settings.demand_pow_rd = 1;

    settings.share_price = MESE_SETTING(0.7, 0.4, 0.4);
    settings.share_mk = MESE_SETTING(0.15, 0.3, 0.3);
    settings.share_rd = MESE_SETTING(0.15, 0.3, 0.3);
    settings.share_pow_price = 3;
    settings.share_pow_mk = 1.5;
    settings.share_pow_rd = 1;

    settings.price_overload = 40;

    settings.mpi_retern_factor = MESE_SETTING(1394.375, 1394.375, 1617) * player_count;
    settings.mpi_factor_a = 50;
    settings.mpi_factor_b = 10;
    settings.mpi_factor_c = 10;
    settings.mpi_factor_d = 10;
    settings.mpi_factor_e = 10;
    settings.mpi_factor_f = 10;

    return std::move(settings);
}

}
