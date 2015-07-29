#include "mese_core.hpp"

namespace mese {

void Period::exec() {
    player_count = last.player_count;
    now_period = last.now_period + 1;

    for (int i = 0; i < player_count; ++i) {
        prod_rate[i] = decision.prod[i] / last.size[i];
        prod_cost_unit[i] = (
            setting.prod_cost_factor_rate
                * (prod_rate[i] > 0.8 ? 0.5 : 1) * pow(prod_rate[i] - 0.8, 2)
            + setting.prod_cost_factor_size
                / player_count / last.capital[i]
            + setting.prod_cost_factor_const
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
            (last.cash[i] > 0 && last.loan[i] == 0) ?
            - setting.interest_rate_cash * last.cash[i] :
            setting.interest_rate_loan * loan_early[i]
        );

        goods[i] = last.inventory[i] + decision.prod[i];
        goods_cost[i] = last.goods_cost_inventory[i] + prod_cost[i];
        goods_max_sales[i] = decision.price[i] * goods[i];

        history_mk[i] = last.history_mk[i] + decision.mk[i];
        history_rd[i] = last.history_rd[i] + decision.rd[i];
    }

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
        sqrt(sum_mk_compressed) / average_price_mixed
    ); // 5.3
    demand_effect_rd = setting.demand_rd * (
        sum_history_rd / (now_period + 1)
    ); // 2.66666
    orders_demand = setting.demand * (
        demand_effect_rd + demand_effect_mk
    );

    for (int i = 0; i < player_count; ++i) {
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

    for (int i = 0; i < player_count; ++i) {
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
        sold[i] = (
            (orders[i] > goods[i]) ?
            goods[i] :
            orders[i]
        );
        inventory[i] = goods[i] - sold[i];
        unfilled[i] = orders[i] - sold[i];

        // goods

        goods_cost_sold[i] = goods_cost[i] * div(sold[i], goods[i], 0);
        goods_cost_inventory[i] = goods_cost[i] - goods_cost_sold[i];

        // cash flow

        sales[i] = decision.price[i] * sold[i];

        inventory_charge[i] = (
            last.inventory[i] < inventory[i] ?
            last.inventory[i] :
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

    double sum_size = sum(size);
    double sum_sold = sum(sold);
    double sum_sales = sum(sales);
    double sum_last_sales = sum(last.sales);

    for (int i = 0; i < player_count; ++i) {
        mpi_a[i] = round(
            setting.mpi_factor_a * player_count * (
                retern[i] / (now_period + 1)
                / setting.mpi_retern_factor
            )
        );

        mpi_b[i] = round(
            setting.mpi_factor_b * player_count * (
                (history_rd[i] + history_mk[i])
                / (sum_history_rd + sum_history_mk)
            )
        );

        mpi_c[i] = round(
            setting.mpi_factor_c * player_count * (
                size[i] / sum_size
            )
        );

        mpi_d[i] = round(
            setting.mpi_factor_d * (
                1 - fabs(80 - prod_rate[i])
            )
        );

        mpi_e[i] = round(
            setting.mpi_factor_e * player_count * div(
                sold[i], sum_sold, 0
            )
        );

        mpi_f[i] = min(round(
            setting.mpi_factor_f * div(
                div(sales[i], last.sales[i], 0),
                div(sum_sales, sum_last_sales, 0),
                0
            )
        ), 20);

        mpi[i] = mpi_a[i] + mpi_b[i] + mpi_c[i] + mpi_d[i] + mpi_e[i] + mpi_f[i];
    }

    average_price = div(sum(sales), sum(sold), average_price_given);
}

}

// debug

#include <iostream>
int main() {
    using namespace mese;

    std::cout << sizeof(Period) << std::endl;
    std::cout << sizeof(Game) << std::endl;

    return 0;
}
