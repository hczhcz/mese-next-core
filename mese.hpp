#pragma once

#include <vector>

#include "util_math.hpp"
#include "util_print.hpp"

namespace mese {

const size_t MAX_PLAYER {32};

#define MESE_VAL(name) double name {NAN}
#define MESE_ARR(name) double name[MAX_PLAYER] { \
    NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, \
    NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, \
}

struct Settings {
    // decision limits

    MESE_VAL(price_max);
    MESE_VAL(price_min);
    MESE_VAL(mk_limit); // ref: on 8 player game
    MESE_VAL(ci_limit); // ref: on 8 player game
    MESE_VAL(rd_limit); // ref: on 8 player game
    MESE_VAL(loan_limit); // ref: on 8 player game

    // costs related

    MESE_VAL(prod_rate_initial);
    MESE_VAL(prod_rate_balanced);
    MESE_VAL(prod_rate_pow);
    MESE_VAL(prod_cost_factor_rate_over);
    MESE_VAL(prod_cost_factor_rate_under);
    MESE_VAL(prod_cost_factor_size);
    MESE_VAL(prod_cost_factor_const);

    MESE_VAL(unit_fee);
    MESE_VAL(deprecation_rate);

    MESE_VAL(initial_cash);
    MESE_VAL(initial_capital);

    MESE_VAL(interest_rate_cash); // const - 0.0025 * setting
    MESE_VAL(interest_rate_loan); // const + 0.0025 * setting
    MESE_VAL(inventory_fee);
    MESE_VAL(tax_rate); // 0.01 * setting

    // orders related

    MESE_VAL(mk_overload);
    MESE_VAL(mk_compression);

    MESE_VAL(demand); // const + setting
    MESE_VAL(demand_price); // 1 + 0.01 * setting
    MESE_VAL(demand_mk); // 159 / sqrt(8400) + 0.0053 * setting (?)
    MESE_VAL(demand_rd); // 1 + 0.01 * setting

    MESE_VAL(demand_ref_price);
    MESE_VAL(demand_ref_mk);
    MESE_VAL(demand_ref_rd);
    MESE_VAL(demand_pow_price);
    MESE_VAL(demand_pow_mk);
    MESE_VAL(demand_pow_rd);

    MESE_VAL(share_price); // 0.01 * setting
    MESE_VAL(share_mk); // 0.01 * setting
    MESE_VAL(share_rd); // 0.01 * setting
    MESE_VAL(share_pow_price);
    MESE_VAL(share_pow_mk);
    MESE_VAL(share_pow_rd);

    MESE_VAL(price_overload);

    // mpi related

    MESE_VAL(mpi_retern_factor);
    MESE_VAL(mpi_factor_a);
    MESE_VAL(mpi_factor_b);
    MESE_VAL(mpi_factor_c);
    MESE_VAL(mpi_factor_d);
    MESE_VAL(mpi_factor_e);
    MESE_VAL(mpi_factor_f);
};

enum class PresetId {
    classic,
    imese,
    modern
};

// player_count == 8 -> classic mode
// player_count == actual value -> 8p-feeling mode
Settings get_preset(PresetId id, size_t player_count);

struct Decisions {
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
    MESE_ARR(sold); // *
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
    inline double sum(double *member) {
        double result = 0;

        for (size_t i = 0; i < player_count; ++i) {
            result += member[i];
        }

        return result;
    }

public:
    size_t player_count;
    size_t now_period;

    Settings settings;
    Decisions decisions;

    // initial period
    Period(size_t count, Settings &&_settings);
    // normal period
    Period(size_t count, Period &last, Settings &&_settings);
    // unserialize
    Period(std::istream &stream);

    bool submit(
        Period &last, size_t i,
        double price, double prod, double mk, double ci, double rd
    );

    void exec(Period &last);

    template <class T>
    void print_full(T callback);
    template <class T>
    void print_settings(T callback);
    template <class T>
    void print_player_early(size_t i, T callback);
    template <class T>
    void print_player(size_t i, T callback);
    template <class T>
    void print_public(T callback);

    void serialize(std::ostream &stream);
};

class Game {
public:
    size_t player_count;
    size_t now_period;
    size_t status;

    // std::string company_name[MAX_PLAYER];

    std::vector<Period> period;

    // new game
    Game(size_t count, Settings &&_settings);
    // unserialize
    Game(std::istream &stream);

    inline bool get_status(size_t i) {
        return (status & (1 << i)) != 0;
    }

    inline void set_status(size_t i) {
        status |= 1 << i;
    }

    inline bool ready() {
        return status == (1 << player_count) - 1;
    }

    Settings &alloc(Settings &&_settings);
    Settings &alloc();

    bool submit(
        size_t i,
        double price, double prod, double mk, double ci, double rd
    );

    bool close();

    void print_full(std::ostream &stream);
    void print_player_early(std::ostream &stream, size_t i);
    void print_player(std::ostream &stream, size_t i);
    void print_public(std::ostream &stream);

    void serialize(std::ostream &stream);
};

}
