#include "mese.hpp"

namespace mese {

enum class PresetId {
    classic,
    imese,
    modern
};

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

const std::vector<std::string> &list_presets() {
    // notice: keep id_map updated
    static const std::vector<std::string> id_list {
        "classic", "imese", "modern"
    };

    return id_list;
}

Settings get_preset(const std::string &name, uint64_t player_count) {
    if (player_count > MAX_PLAYER) {
        throw 1; // TODO
    }

    // notice: keep id_list updated
    static const std::map<const std::string, PresetId> id_map {
        {"classic", PresetId::classic},
        {"imese", PresetId::imese},
        {"modern", PresetId::modern}
    };

    PresetId id {id_map.at(name)};

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

    return settings; // RVO
}

const std::vector<std::string> &list_settings() {
    // notice: keep name_map updated
    static const std::vector<std::string> name_list {
        "price_max", "price_min", "mk_limit", "ci_limit", "rd_limit", "loan_limit",

        "prod_rate_initial", "prod_rate_balanced", "prod_rate_pow",
        "prod_cost_factor_rate_over", "prod_cost_factor_rate_under",
        "prod_cost_factor_size", "prod_cost_factor_const",

        "unit_fee", "deprecation_rate",

        "initial_cash", "initial_capital",

        "interest_rate_cash", "interest_rate_loan", "inventory_fee", "tax_rate",

        "mk_overload", "mk_compression",

        "demand", "demand_price", "demand_mk", "demand_rd",

        "demand_ref_price", "demand_ref_mk", "demand_ref_rd",
        "demand_pow_price", "demand_pow_mk", "demand_pow_rd",

        "share_price", "share_mk", "share_rd",
        "share_pow_price", "share_pow_mk", "share_pow_rd",

        "price_overload",

        "mpi_retern_factor",
        "mpi_factor_a", "mpi_factor_b", "mpi_factor_c",
        "mpi_factor_d", "mpi_factor_e", "mpi_factor_f"
    };

    return name_list;
}

void change_setting(Settings &settings, const std::string &name, double value) {
    // notice: keep name_list updated
    static const std::map<const std::string, double Settings::*> name_map {
        {"price_max", &Settings::price_max},
        {"price_min", &Settings::price_min},
        {"mk_limit", &Settings::mk_limit},
        {"ci_limit", &Settings::ci_limit},
        {"rd_limit", &Settings::rd_limit},
        {"loan_limit", &Settings::loan_limit},

        {"prod_rate_initial", &Settings::prod_rate_initial},
        {"prod_rate_balanced", &Settings::prod_rate_balanced},
        {"prod_rate_pow", &Settings::prod_rate_pow},
        {"prod_cost_factor_rate_over", &Settings::prod_cost_factor_rate_over},
        {"prod_cost_factor_rate_under", &Settings::prod_cost_factor_rate_under},
        {"prod_cost_factor_size", &Settings::prod_cost_factor_size},
        {"prod_cost_factor_const", &Settings::prod_cost_factor_const},

        {"unit_fee", &Settings::unit_fee},
        {"deprecation_rate", &Settings::deprecation_rate},

        {"initial_cash", &Settings::initial_cash},
        {"initial_capital", &Settings::initial_capital},

        {"interest_rate_cash", &Settings::interest_rate_cash},
        {"interest_rate_loan", &Settings::interest_rate_loan},
        {"inventory_fee", &Settings::inventory_fee},
        {"tax_rate", &Settings::tax_rate},

        {"mk_overload", &Settings::mk_overload},
        {"mk_compression", &Settings::mk_compression},

        {"demand", &Settings::demand},
        {"demand_price", &Settings::demand_price},
        {"demand_mk", &Settings::demand_mk},
        {"demand_rd", &Settings::demand_rd},

        {"demand_ref_price", &Settings::demand_ref_price},
        {"demand_ref_mk", &Settings::demand_ref_mk},
        {"demand_ref_rd", &Settings::demand_ref_rd},
        {"demand_pow_price", &Settings::demand_pow_price},
        {"demand_pow_mk", &Settings::demand_pow_mk},
        {"demand_pow_rd", &Settings::demand_pow_rd},

        {"share_price", &Settings::share_price},
        {"share_mk", &Settings::share_mk},
        {"share_rd", &Settings::share_rd},
        {"share_pow_price", &Settings::share_pow_price},
        {"share_pow_mk", &Settings::share_pow_mk},
        {"share_pow_rd", &Settings::share_pow_rd},

        {"price_overload", &Settings::price_overload},

        {"mpi_retern_factor", &Settings::mpi_retern_factor},
        {"mpi_factor_a", &Settings::mpi_factor_a},
        {"mpi_factor_b", &Settings::mpi_factor_b},
        {"mpi_factor_c", &Settings::mpi_factor_c},
        {"mpi_factor_d", &Settings::mpi_factor_d},
        {"mpi_factor_e", &Settings::mpi_factor_e},
        {"mpi_factor_f", &Settings::mpi_factor_f}
    };

    settings.*(name_map.at(name)) = value;
}

}
