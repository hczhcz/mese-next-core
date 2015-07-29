#pragma once

#include <cmath>
#include <string>
#include <vector>

namespace mese {

#define MESE_VAL(name) double name {NAN}
#define MESE_ARR(name) double name[16] { \
    NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, \
    NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, \
}

const size_t MAX_PLAYER {16};

struct Setting {
    // decision limits

    double price_max {99};
    double price_min {12};
    double mk_limit {15000 * 8}; // ref: on 8 player game
    double ci_limit {15000 * 8}; // ref: on 8 player game
    double rd_limit {15000 * 8}; // ref: on 8 player game
    double loan_limit {30000 * 8}; // ref: on 8 player game

    // costs related

    double prod_rate_balanced {0.8};
    double prod_rate_pow {2};
    double prod_cost_factor_rate_over {69};
    double prod_cost_factor_rate_under {138};
    double prod_cost_factor_size {15};
    double prod_cost_factor_const {3};

    double initial_cash {14700};
    double initial_capital {168000};
    double deprecation_rate {0.05};

    double interest_rate_cash {0.0025}; // 0.005 - 0.0001 * setting
    double interest_rate_loan {0.005}; // 0.0025 + 0.0001 * setting
    double inventory_fee {1};
    double unit_fee {40};
    double tax_rate {0.1}; // 0.01 * setting

    // orders related

    double mk_overload {16800};
    double mk_compression {0.25};

    double demand {500}; // 500 + setting
    double demand_price {1}; // 1 + 0.01 * setting
    double demand_mk {5.3}; // 159 / sqrt(8400) + 0.0053 * setting
    double demand_rd {1}; // (1 + 0.01 * setting) / 3150

    double demand_ref_price {30};
    double demand_ref_mk {8400};
    double demand_ref_rd {3150};
    double demand_pow_price {1};
    double demand_pow_mk {0.5};
    double demand_pow_rd {1};

    double share_price {0.4}; // 0.01 * setting
    double share_mk {0.3}; // 0.01 * setting
    double share_rd {0.3}; // 0.01 * setting
    double share_pow_price {3};
    double share_pow_mk {1.5};
    double share_pow_rd {1};

    double price_overload {40};

    // mpi related

    double mpi_retern_factor {11155};
    double mpi_factor_a {50};
    double mpi_factor_b {10};
    double mpi_factor_c {10};
    double mpi_factor_d {10};
    double mpi_factor_e {10};
    double mpi_factor_f {10};
};

struct Decision {
    MESE_ARR(price);
    MESE_ARR(prod);
    MESE_ARR(mk);
    MESE_ARR(ci);
    MESE_ARR(rd);
};

struct PeriodDataEarly {
    MESE_ARR(prod_rate);
    MESE_ARR(prod_over);
    MESE_ARR(prod_cost_unit);
    MESE_ARR(prod_cost_marginal);
    MESE_ARR(prod_cost);

    MESE_ARR(deprecation);
    MESE_ARR(capital); // *
    MESE_ARR(size); // *
    MESE_ARR(spending);
    MESE_ARR(balance_early);
    MESE_ARR(loan_early);
    MESE_ARR(interest);

    MESE_ARR(goods);
    MESE_ARR(goods_cost);
    MESE_ARR(goods_max_sales);

    MESE_ARR(history_mk); // *
    MESE_ARR(history_rd); // *
};

struct PeriodData {
    MESE_VAL(average_price_given);
    MESE_VAL(average_price_planned);
    MESE_VAL(average_price_mixed);
    MESE_VAL(demand_effect_mk);
    MESE_VAL(demand_effect_rd);
    MESE_VAL(orders_demand);

    MESE_ARR(share_effect_price);
    MESE_ARR(share_effect_mk);
    MESE_ARR(share_effect_rd);
    MESE_ARR(share);
    MESE_ARR(share_compressed);

    MESE_ARR(orders);
    MESE_ARR(sold);
    MESE_ARR(inventory); // *
    MESE_ARR(unfilled);

    MESE_ARR(goods_cost_sold);
    MESE_ARR(goods_cost_inventory); // *

    MESE_ARR(sales); // *
    MESE_ARR(inventory_charge);
    MESE_ARR(cost_before_tax);
    MESE_ARR(profit_before_tax);
    MESE_ARR(tax_charge);
    MESE_ARR(profit);

    MESE_ARR(balance);
    MESE_ARR(loan); // *
    MESE_ARR(cash); // *
    MESE_ARR(retern); // *

    MESE_VAL(average_price); // *

    MESE_ARR(mpi_a);
    MESE_ARR(mpi_b);
    MESE_ARR(mpi_c);
    MESE_ARR(mpi_d);
    MESE_ARR(mpi_e);
    MESE_ARR(mpi_f);
    MESE_ARR(mpi);
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
