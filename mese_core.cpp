#include "mese_core.hpp"

namespace mese {

Game::Game(size_t count):
    player_count {count},
    init {count}
{
    period.push_back({player_count, init});
    Period &current {period.back()};

    for (size_t i = 0; i < player_count; ++i) {
        if (!period.back().submit(
            i,
            current.setting.demand_ref_price,
            init.size[i] * current.setting.prod_rate_balanced,
            current.setting.demand_ref_mk / player_count,
            init.capital[i] * current.setting.deprecation_rate,
            current.setting.demand_ref_rd / player_count
        )) {
            throw 1; // TODO: initial decision should not break limits
        }
    }

    current.exec();
}

Period::Period(size_t count):
    PeriodDataEarly {},
    PeriodData {},

    last {*static_cast<Period *>(nullptr)},
    player_count {count},
    now_period {0},

    setting {},
    decision {}
{
    for (size_t i = 0; i < player_count; ++i) {
        capital[i] = setting.initial_capital / player_count;
        size[i] = capital[i] / setting.unit_fee;
        history_mk[i] = 0;
        history_rd[i] = 0;

        inventory[i] = 0;
        goods_cost_inventory[i] = 0;

        sales[i] = setting.demand_ref_price * size[i] * setting.prod_rate_balanced;
        loan[i] = 0;
        cash[i] = setting.initial_cash / player_count; // C + 168000 = 182700 + L
        retern[i] = 0;
    }

    average_price = setting.demand_ref_price;
}

Period::Period(size_t count, Period &_last):
    PeriodDataEarly {},
    PeriodData {},

    last {_last},
    player_count {count},
    now_period {_last.now_period + 1},

    setting {_last.setting},
    decision {}
{
    // nothing
}

bool Period::submit(
    size_t i,
    double price, double prod, double mk, double ci, double rd
) {
    decision.price[i] = price;
    decision.prod[i] = prod;
    decision.mk[i] = mk;
    decision.ci[i] = ci;
    decision.rd[i] = rd;

    prod_rate[i] = decision.prod[i] / last.size[i];
    prod_over[i] = prod_rate[i] - setting.prod_rate_balanced;

    double prod_cost_factor_rate = (
        prod_over[i] > 0 ?
        setting.prod_cost_factor_rate_over :
        setting.prod_cost_factor_rate_under
    );
    prod_cost_unit[i] = (
        prod_cost_factor_rate * pow(prod_over[i], setting.prod_rate_pow)
        + setting.prod_cost_factor_size
            * setting.initial_capital / player_count / last.capital[i]
        + setting.prod_cost_factor_const
    );
    prod_cost_marginal[i] = ( // D(prod_cost(prod), prod)
        prod_cost_factor_rate
            * setting.prod_rate_pow
            * prod_rate[i] * pow(prod_over[i], setting.prod_rate_pow - 1)
        + prod_cost_unit[i]
    );
    prod_cost[i] = prod_cost_unit[i] * decision.prod[i];

    deprecation[i] = last.capital[i] * setting.deprecation_rate;
    capital[i] = last.capital[i] + decision.ci[i] - deprecation[i];
    size[i] = capital[i] / setting.unit_fee;

    spending[i] = (
        prod_cost[i]
        + decision.ci[i] - deprecation[i]
        + decision.mk[i] + decision.rd[i]
    );
    balance_early[i] = last.cash[i] - last.loan[i] - spending[i];
    loan_early[i] = max(- balance_early[i], 0);
    interest[i] = (
        last.loan[i] == 0 ?
        - setting.interest_rate_cash * last.cash[i] :
        setting.interest_rate_loan * loan_early[i]
    );

    goods[i] = last.inventory[i] + decision.prod[i];
    goods_cost[i] = last.goods_cost_inventory[i] + prod_cost[i];
    goods_max_sales[i] = decision.price[i] * goods[i];

    history_mk[i] = last.history_mk[i] + decision.mk[i];
    history_rd[i] = last.history_rd[i] + decision.rd[i];

    return (
        decision.price[i] >= setting.price_min
        && decision.price[i] <= setting.price_max
        && decision.prod[i] >= 0
        && decision.prod[i] <= last.size[i]
        && decision.mk[i] >= 0
        && decision.mk[i] <= setting.mk_limit / player_count
        && decision.ci[i] >= 0
        && decision.ci[i] <= setting.ci_limit / player_count
        && decision.rd[i] >= 0
        && decision.rd[i] <= setting.rd_limit / player_count
        && loan_early[i] <= setting.loan_limit / player_count
    );
}

void Period::exec() {
    double sum_mk = sum(decision.mk);
    double sum_mk_compressed = (
        sum_mk > setting.mk_overload ? (
            (sum_mk - setting.mk_overload) * setting.mk_compression
            + setting.mk_overload
        ) : sum_mk
    );
    double sum_history_mk = sum(history_mk);
    double sum_history_rd = sum(history_rd);

    average_price_given = sum(decision.price) / player_count;
    average_price_planned = div(
        sum(goods_max_sales), sum(goods), average_price_given
    );
    average_price_mixed = (
        setting.demand_price * average_price_planned
        + (1 - setting.demand_price) * last.average_price
    );

    demand_effect_mk = setting.demand_mk * (
        pow(sum_mk_compressed, setting.demand_pow_mk)
        / pow(setting.demand_ref_mk, setting.demand_pow_mk)
    ) / (
        pow(average_price_mixed, setting.demand_pow_price)
        / pow(setting.demand_ref_price, setting.demand_pow_price)
    );
    demand_effect_rd = setting.demand_rd * (
        pow(sum_history_rd / (now_period + 1), setting.demand_pow_rd)
        / pow(setting.demand_ref_rd, setting.demand_pow_rd)
    );
    orders_demand = setting.demand * (
        demand_effect_rd + demand_effect_mk
    );

    for (size_t i = 0; i < player_count; ++i) {
        share_effect_price[i] = pow(
            average_price_mixed / decision.price[i],
            setting.share_pow_price
        );
        share_effect_mk[i] = pow(
            decision.mk[i] / decision.price[i],
            setting.share_pow_mk
        );
        share_effect_rd[i] = pow(
            history_rd[i],
            setting.share_pow_rd
        );
    }

    double sum_share_effect_price = sum(share_effect_price);
    double sum_share_effect_mk = sum(share_effect_mk);
    double sum_share_effect_rd = sum(share_effect_rd);

    for (size_t i = 0; i < player_count; ++i) {
        // orders

        share[i] = (
            setting.share_price * div(share_effect_price[i], sum_share_effect_price, 0)
            + setting.share_mk * div(share_effect_mk[i], sum_share_effect_mk, 0)
            + setting.share_rd * div(share_effect_rd[i], sum_share_effect_rd, 0)
        );

        share_compressed[i] =  (
            decision.price[i] > setting.price_overload ? (
                share[i] * setting.price_overload / decision.price[i]
            ) : share[i]
        );

        orders[i] = orders_demand * share[i];
        sold[i] = min(orders[i], goods[i]);
        inventory[i] = goods[i] - sold[i];
        unfilled[i] = orders[i] - sold[i];

        // goods

        goods_cost_sold[i] = goods_cost[i] * div(sold[i], goods[i], 0);
        goods_cost_inventory[i] = goods_cost[i] - goods_cost_sold[i];

        // cash flow

        sales[i] = decision.price[i] * sold[i];

        inventory_charge[i] = min(
            last.inventory[i],
            inventory[i]
        ) * setting.inventory_fee;

        cost_before_tax[i] = (
            prod_cost[i]
            + deprecation[i]
            + decision.mk[i] + decision.rd[i]
            + interest[i] + inventory_charge[i]
        );
        profit_before_tax[i] = sales[i] - cost_before_tax[i];
        tax_charge[i] = profit_before_tax[i] * setting.tax_rate;
        profit[i] = profit_before_tax[i] - tax_charge[i];

        balance[i] = (
            last.cash[i] + loan_early[i] - last.loan[i]
            + profit[i]
            - decision.ci[i] + deprecation[i]
            + goods_cost_sold[i] - prod_cost[i]
        );
        loan[i] = max(loan_early[i], loan_early[i] - balance[i]);
        cash[i] = max(balance[i], 0);
        retern[i] = last.retern[i] + profit[i];
    }

    average_price = div(sum(sales), sum(sold), average_price_given);

    double sum_size = sum(size);
    double sum_sold = sum(sold);
    double sum_sales = sum(sales);
    double sum_last_sales = sum(last.sales);

    for (size_t i = 0; i < player_count; ++i) {
        mpi_a[i] = (
            setting.mpi_factor_a * player_count * (
                retern[i] / (now_period + 1)
                / setting.mpi_retern_factor
            )
        );

        mpi_b[i] = (
            setting.mpi_factor_b * player_count * (
                (history_rd[i] + history_mk[i])
                / (sum_history_rd + sum_history_mk)
            )
        );

        mpi_c[i] = (
            setting.mpi_factor_c * player_count * (
                size[i] / sum_size
            )
        );

        mpi_d[i] = (
            setting.mpi_factor_d * (
                1 - abs(prod_over[i])
            )
        );

        mpi_e[i] = (
            setting.mpi_factor_e * player_count * div(
                sold[i], sum_sold, 0
            )
        );

        mpi_f[i] = min(
            setting.mpi_factor_f * div(
                div(sales[i], last.sales[i], 0),
                div(sum_sales, sum_last_sales, 0),
                0
            ),
            2 * setting.mpi_factor_f
        );

        mpi[i] = mpi_a[i] + mpi_b[i] + mpi_c[i] + mpi_d[i] + mpi_e[i] + mpi_f[i];
    }
}

}

// debug

#include <iostream>
int main() {
    using namespace mese;

    std::cout << sizeof(Period) << std::endl;
    std::cout << sizeof(Game) << std::endl;

    Game game {6};

    return 0;
}
