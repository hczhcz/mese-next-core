#pragma once

#include <cmath>
#include <string>

struct MESE {
    static const size_t BUFSIZE = 32;

    MESE &last;

    size_t player_count;
    size_t now_period;
    std::string company_name[BUFSIZE]; // TODO

    struct {
        // decision limits

        double price_limit;
        double loan_limit;
        double mk_limit;
        double ci_limit;
        double rd_limit;

        // costs related

        double prod_cost_factor_rate; // 138
        double prod_cost_factor_size; // 2520000 = 168000 * 15
        double prod_cost_factor_const; // 3

        double deprecation_rate; // 0.05

        double interest_rate_cash; // 0.005 - 0.0001 * setting
        double interest_rate_loan; // 0.0025 + 0.0001 * setting
        double inventory_fee; // 1
        double unit_fee; // 40
        double tax_rate; // 0.01 * setting

        // orders related

        double mk_overload; // 16800
        double mk_compression; // 0.25

        double demand; // 500 + setting
        double demand_price; // 1 + 0.01 * setting
        double demand_mk; // 159 / sqrt(8400) + 0.0053 * setting
        double demand_rd; // (1 + 0.01 * setting) / 3150

        double share_price; // 0.01 * setting
        double share_mk; // 0.01 * setting
        double share_rd; // 0.01 * setting

        double share_pow_price; // 3
        double share_pow_mk; // 1.5
        double share_pow_rd; // 1

        double price_overload; // 40

        // mpi related

        double mpi_retern_factor; // 11155
        double mpi_factor_a; // 50
        double mpi_factor_b; // 10
        double mpi_factor_c; // 10
        double mpi_factor_d; // 10
        double mpi_factor_e; // 10
        double mpi_factor_f; // 10
    } setting;

    struct {
        double price[BUFSIZE];
        double prod[BUFSIZE];
        double mk[BUFSIZE];
        double ci[BUFSIZE];
        double rd[BUFSIZE];
    } decision;

    double prod_rate[BUFSIZE];
    double prod_cost_unit[BUFSIZE];
    double prod_cost[BUFSIZE];

    double deprecation[BUFSIZE];
    double capital[BUFSIZE];
    double size[BUFSIZE];
    double spending[BUFSIZE];
    double balance_early[BUFSIZE];
    double loan_early[BUFSIZE];
    double interest[BUFSIZE];

    double goods[BUFSIZE];
    double goods_cost[BUFSIZE];
    double goods_max_sales[BUFSIZE];

    double history_mk[BUFSIZE];
    double history_rd[BUFSIZE];

    double average_price_given;
    double average_price_planned;
    double average_price_mixed;
    double demand_effect_mk;
    double demand_effect_rd;
    double orders_demand;

    double share_effect_price[BUFSIZE];
    double share_effect_mk[BUFSIZE];
    double share_effect_rd[BUFSIZE];
    double share[BUFSIZE];
    double share_compressed[BUFSIZE];

    double orders[BUFSIZE];
    double sold[BUFSIZE];
    double inventory[BUFSIZE];
    double unfilled[BUFSIZE];

    double goods_cost_sold[BUFSIZE];
    double goods_cost_inventory[BUFSIZE];

    double sales[BUFSIZE];
    double inventory_charge[BUFSIZE];
    double cost_before_tax[BUFSIZE];
    double profit_before_tax[BUFSIZE];
    double tax_charge[BUFSIZE];
    double profit[BUFSIZE];

    double balance[BUFSIZE];
    double loan[BUFSIZE];
    double cash[BUFSIZE];
    double retern[BUFSIZE];

    double mpi_a[BUFSIZE];
    double mpi_b[BUFSIZE];
    double mpi_c[BUFSIZE];
    double mpi_d[BUFSIZE];
    double mpi_e[BUFSIZE];
    double mpi_f[BUFSIZE];
    double mpi[BUFSIZE];

    double average_price;

    inline double sum(double (&member)[BUFSIZE]) {
        double result = 0;

        for (size_t i = 0; i < player_count; ++i) {
            result += member[i];
        }

        return result;
    }

    inline double div(double a, double b, double error) {
        return b == 0 ? error : a / b;
    }

    inline double min(double a, double b) {
        return a > b ? b : a;
    }

    inline double max(double a, double b) {
        return a > b ? a : b;
    }

    void exec();
};
