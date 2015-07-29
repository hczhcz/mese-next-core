#pragma once

#include <cmath>
#include <string>
#include <vector>

namespace mese {

const size_t MAX_PLAYER = 16;

struct Setting {
    // decision limits

    double price_max{99};
    double price_min{12};
    double loan_limit{30000};
    double mk_limit{15000};
    double ci_limit{15000};
    double rd_limit{15000};

    // costs related

    double prod_rate_balanced{0.8};
    double prod_rate_pow{2};
    double prod_cost_factor_rate_over{69};
    double prod_cost_factor_rate_under{138};
    double prod_cost_factor_size{168000 * 15};
    double prod_cost_factor_const{3};

    double deprecation_rate{0.05};

    double interest_rate_cash{0.0025}; // 0.005 - 0.0001 * setting
    double interest_rate_loan{0.005}; // 0.0025 + 0.0001 * setting
    double inventory_fee{1};
    double unit_fee{40};
    double tax_rate{0.1}; // 0.01 * setting

    // orders related

    double mk_overload{16800};
    double mk_compression{0.25};

    double demand{500}; // 500 + setting
    double demand_price{1}; // 1 + 0.01 * setting
    double demand_mk{5.3}; // 159 / sqrt(8400) + 0.0053 * setting
    double demand_rd{1}; // (1 + 0.01 * setting) / 3150

    double demand_ref_price{30};
    double demand_ref_mk{8400};
    double demand_ref_rd{3150};
    double demand_pow_price{1};
    double demand_pow_mk{0.5};
    double demand_pow_rd{1};

    double share_price{0.4}; // 0.01 * setting
    double share_mk{0.3}; // 0.01 * setting
    double share_rd{0.3}; // 0.01 * setting
    double share_pow_price{3};
    double share_pow_mk{1.5};
    double share_pow_rd{1};

    double price_overload{40};

    // mpi related

    double mpi_retern_factor{11155};
    double mpi_factor_a{50};
    double mpi_factor_b{10};
    double mpi_factor_c{10};
    double mpi_factor_d{10};
    double mpi_factor_e{10};
    double mpi_factor_f{10};
};

struct Decision {
    double price[MAX_PLAYER];
    double prod[MAX_PLAYER];
    double mk[MAX_PLAYER];
    double ci[MAX_PLAYER];
    double rd[MAX_PLAYER];
};

struct PeriodDataEarly {
    double prod_rate[MAX_PLAYER];
    double prod_over[MAX_PLAYER];
    double prod_cost_unit[MAX_PLAYER];
    double prod_cost_marginal[MAX_PLAYER];
    double prod_cost[MAX_PLAYER];

    double deprecation[MAX_PLAYER];
    double capital[MAX_PLAYER]; // *
    double size[MAX_PLAYER]; // *
    double spending[MAX_PLAYER];
    double balance_early[MAX_PLAYER];
    double loan_early[MAX_PLAYER];
    double interest[MAX_PLAYER];

    double goods[MAX_PLAYER];
    double goods_cost[MAX_PLAYER];
    double goods_max_sales[MAX_PLAYER];

    double history_mk[MAX_PLAYER]; // *
    double history_rd[MAX_PLAYER]; // *
};

struct PeriodData {
    double average_price_given;
    double average_price_planned;
    double average_price_mixed;
    double demand_effect_mk;
    double demand_effect_rd;
    double orders_demand;

    double share_effect_price[MAX_PLAYER];
    double share_effect_mk[MAX_PLAYER];
    double share_effect_rd[MAX_PLAYER];
    double share[MAX_PLAYER];
    double share_compressed[MAX_PLAYER];

    double orders[MAX_PLAYER];
    double sold[MAX_PLAYER];
    double inventory[MAX_PLAYER]; // *
    double unfilled[MAX_PLAYER];

    double goods_cost_sold[MAX_PLAYER];
    double goods_cost_inventory[MAX_PLAYER]; // *

    double sales[MAX_PLAYER]; // *
    double inventory_charge[MAX_PLAYER];
    double cost_before_tax[MAX_PLAYER];
    double profit_before_tax[MAX_PLAYER];
    double tax_charge[MAX_PLAYER];
    double profit[MAX_PLAYER];

    double balance[MAX_PLAYER];
    double loan[MAX_PLAYER]; // *
    double cash[MAX_PLAYER]; // *
    double retern[MAX_PLAYER]; // *

    double average_price; // *

    double mpi_a[MAX_PLAYER];
    double mpi_b[MAX_PLAYER];
    double mpi_c[MAX_PLAYER];
    double mpi_d[MAX_PLAYER];
    double mpi_e[MAX_PLAYER];
    double mpi_f[MAX_PLAYER];
    double mpi[MAX_PLAYER];
};

class Game;

class Period: public PeriodDataEarly, public PeriodData {
private:
    inline double sum(double (&member)[MAX_PLAYER]) {
        double result = 0;

        for (size_t i = 0; i < player_count; ++i) {
            result += member[i];
        }

        return result;
    }

    inline double div(double a, double b, double error) {
        return b == 0 ? error : a / b;
    }

    inline double pow(double a, double e) {
        return ::pow(a, e);
    }

    inline double abs(double a) {
        return a > 0 ? a : - a;
    }

    inline double min(double a, double b) {
        return a > b ? b : a;
    }

    inline double max(double a, double b) {
        return a > b ? a : b;
    }

public:
    Period &last;

    size_t player_count;
    size_t now_period;

    Setting setting;
    Decision decision;

    Period(size_t count);
    Period(size_t count, Period &_last);

    bool submit(
        size_t i,
        double price, double prod, double mk, double ci, double rd
    );

    void exec();
};

class Game {
public:
    size_t player_count;

    std::string company_name[MAX_PLAYER];
    std::vector<Period> period;
};

}
