#include "mese.hpp"

namespace mese {

Period::Period(uint64_t count, Settings &&_settings):
    PeriodDataEarly {},
    PeriodData {},

    player_count {count},
    now_period {0},

    settings(std::move(_settings)), // move
    decisions {}
{
    for (uint64_t i = 0; i < player_count; ++i) {
        capital[i] = MESE_CASH(settings.initial_capital / player_count);
        size[i] = MESE_UNIT(capital[i] / settings.unit_fee);
        history_mk[i] = 0;
        history_rd[i] = 0;

        sold[i] = MESE_UNIT(size[i] * settings.prod_rate_balanced);
        inventory[i] = 0;
        goods_cost_inventory[i] = 0;

        sales[i] = MESE_CASH(settings.demand_ref_price * sold[i]);
        loan[i] = 0;
        cash[i] = MESE_CASH(settings.initial_cash / player_count); // C + 168000 = 182700 + L
        retern[i] = 0;
    }

    average_price = MESE_CASH(settings.demand_ref_price);
}

Period::Period(uint64_t count, Period &last, Settings &&_settings):
    PeriodDataEarly {},
    PeriodData {},

    player_count {count},
    now_period {last.now_period + 1},

    settings(std::move(_settings)), // move
    decisions {}
{
    // nothing
}

Period::Period(std::istream &stream):
    PeriodDataEarly {},
    PeriodData {},

    player_count {},
    now_period {},

    settings {},
    decisions {}
{
    stream.read(reinterpret_cast<char *>(this), sizeof(*this));
}

bool Period::submit(
    Period &last, uint64_t i,
    double price, double prod, double mk, double ci, double rd
) {
    decisions.price[i] = MESE_CASH(price);
    decisions.prod[i] = MESE_UNIT(prod);
    decisions.mk[i] = MESE_CASH(mk);
    decisions.ci[i] = MESE_CASH(ci);
    decisions.rd[i] = MESE_CASH(rd);

    prod_rate[i] = MESE_RATE(decisions.prod[i] / last.size[i]);
    prod_over[i] = MESE_RATE(prod_rate[i] - settings.prod_rate_balanced);

    double prod_cost_factor_rate = (
        prod_over[i] > 0 ?
        settings.prod_cost_factor_rate_over :
        settings.prod_cost_factor_rate_under
    );
    prod_cost_unit[i] = MESE_CASH(
        prod_cost_factor_rate * pow(prod_over[i], settings.prod_rate_pow)
        + settings.prod_cost_factor_size
            * settings.initial_capital / player_count / last.capital[i]
        + settings.prod_cost_factor_const
    );
    prod_cost_marginal[i] = MESE_CASH( // D(prod_cost(prod), prod)
        prod_cost_factor_rate
            * settings.prod_rate_pow
            * prod_rate[i] * pow(prod_over[i], settings.prod_rate_pow - 1)
        + prod_cost_unit[i]
    );
    prod_cost[i] = MESE_CASH(
        prod_cost_unit[i] * decisions.prod[i]
    );

    goods[i] = MESE_UNIT(last.inventory[i] + decisions.prod[i]);
    goods_cost[i] = MESE_CASH(last.goods_cost_inventory[i] + prod_cost[i]);
    goods_max_sales[i] = MESE_CASH(decisions.price[i] * goods[i]);

    deprecation[i] = MESE_CASH(last.capital[i] * settings.deprecation_rate);
    capital[i] = MESE_CASH(
        last.capital[i] + decisions.ci[i] - deprecation[i]
    );
    size[i] = MESE_UNIT(capital[i] / settings.unit_fee);

    spending[i] = MESE_CASH(
        prod_cost[i]
        + decisions.ci[i] - deprecation[i]
        + decisions.mk[i] + decisions.rd[i]
    );
    balance_early[i] = MESE_CASH(
        last.cash[i] - last.loan[i] - spending[i]
    );
    loan_early[i] = MESE_CASH(
        max(- balance_early[i], 0)
    );
    interest[i] = MESE_CASH(
        (
            balance_early[i] >= 0 ?
            settings.interest_rate_cash : settings.interest_rate_loan
        ) * balance_early[i]
    );

    history_mk[i] = MESE_CASH(last.history_mk[i] + decisions.mk[i]);
    history_rd[i] = MESE_CASH(last.history_rd[i] + decisions.rd[i]);

    return (
        decisions.price[i] >= settings.price_min
        && decisions.price[i] <= settings.price_max
        && decisions.prod[i] >= 0
        && decisions.prod[i] <= last.size[i]
        && decisions.mk[i] >= 0
        && decisions.mk[i] <= settings.mk_limit / player_count
        && decisions.ci[i] >= 0
        && decisions.ci[i] <= settings.ci_limit / player_count
        && decisions.rd[i] >= 0
        && decisions.rd[i] <= settings.rd_limit / player_count
        && loan_early[i] <= settings.loan_limit / player_count
    );
}

void Period::exec(Period &last) {
    double sum_mk = sum(decisions.mk);
    double sum_mk_compressed = min(
        (sum_mk - settings.mk_overload) * settings.mk_compression
        + settings.mk_overload,
        sum_mk
    );
    double sum_history_mk = sum(history_mk);
    double sum_history_rd = sum(history_rd);

    average_price_given = MESE_CASH(
        sum(decisions.price) / player_count
    );
    average_price_planned = MESE_CASH(
        div(sum(goods_max_sales), sum(goods), average_price_given)
    );
    average_price_mixed = MESE_CASH(
        settings.demand_price * average_price_planned
        + (1 - settings.demand_price) * last.average_price
    );

    demand_effect_mk = settings.demand_mk * pow(
        sum_mk_compressed / settings.demand_ref_mk,
        settings.demand_pow_mk
    ) / pow(
        average_price_mixed / settings.demand_ref_price,
        settings.demand_pow_price
    );
    demand_effect_rd = settings.demand_rd * pow(
        sum_history_rd / now_period / settings.demand_ref_rd,
        settings.demand_pow_rd
    );
    orders_demand = MESE_UNIT(
        settings.demand * (demand_effect_rd + demand_effect_mk)
    );

    for (uint64_t i = 0; i < player_count; ++i) {
        share_effect_price[i] = pow(
            average_price_mixed / decisions.price[i],
            settings.share_pow_price
        );
        share_effect_mk[i] = pow(
            decisions.mk[i] / decisions.price[i],
            settings.share_pow_mk
        );
        share_effect_rd[i] = pow(
            history_rd[i],
            settings.share_pow_rd
        );
    }

    double sum_share_effect_price = sum(share_effect_price);
    double sum_share_effect_mk = sum(share_effect_mk);
    double sum_share_effect_rd = sum(share_effect_rd);

    for (uint64_t i = 0; i < player_count; ++i) {
        // orders

        share[i] = MESE_RATE(
            settings.share_price * div(
                share_effect_price[i], sum_share_effect_price, 0
            )
            + settings.share_mk * div(
                share_effect_mk[i], sum_share_effect_mk, 0
            )
            + settings.share_rd * div(
                share_effect_rd[i], sum_share_effect_rd, 0
            )
        );

        share_compressed[i] = MESE_RATE(
            min(share[i] * settings.price_overload / decisions.price[i], share[i])
        );

        orders[i] = MESE_UNIT(orders_demand * share_compressed[i]);
        sold[i] = MESE_UNIT(min(orders[i], goods[i]));
        inventory[i] = MESE_UNIT(goods[i] - sold[i]);
        unfilled[i] = MESE_UNIT(orders[i] - sold[i]);

        // goods

        goods_cost_sold[i] = MESE_CASH(
            goods_cost[i] * div(sold[i], goods[i], 0)
        );
        goods_cost_inventory[i] = MESE_CASH(
            goods_cost[i] - goods_cost_sold[i]
        );

        // cash flow

        sales[i] = MESE_CASH(decisions.price[i] * sold[i]);

        inventory_charge[i] = MESE_CASH(
            min(
                last.inventory[i], inventory[i]
            ) * settings.inventory_fee
        );

        cost_before_tax[i] = MESE_CASH(
            goods_cost_sold[i]
            + deprecation[i]
            + decisions.mk[i] + decisions.rd[i]
            - interest[i] + inventory_charge[i]
        );
        profit_before_tax[i] = MESE_CASH(
            sales[i] - cost_before_tax[i]
        );
        tax_charge[i] = MESE_CASH(
            profit_before_tax[i] * settings.tax_rate
        );
        profit[i] = MESE_CASH(
            profit_before_tax[i] - tax_charge[i]
        );

        // balance[i] = MESE_CASH(
        //     last.cash[i] + loan_early[i] - last.loan[i]
        //     + sales[i]
        //     - spending[i] - deprecation[i]
        //     + interest[i] - inventory_charge[i] - tax_charge[i]
        // );
        balance[i] = MESE_CASH(
            last.cash[i] + loan_early[i] - last.loan[i]
            + profit[i]
            - decisions.ci[i] + deprecation[i]
            + goods_cost_sold[i] - prod_cost[i]
        );
        loan[i] = MESE_CASH(
            max(loan_early[i], loan_early[i] - balance[i])
        );
        cash[i] = MESE_CASH(
            max(balance[i], 0)
        );
        retern[i] = MESE_CASH(
            last.retern[i] + profit[i]
        );
    }

    average_price = MESE_CASH(
        div(sum(sales), sum(sold), average_price_given)
    );

    double sum_size = sum(size);
    double sum_sold = sum(sold);
    double sum_sales = sum(sales);
    double sum_last_sales = sum(last.sales);

    for (uint64_t i = 0; i < player_count; ++i) {
        mpi_a[i] = MESE_INDEX(
            settings.mpi_factor_a * player_count * (
                retern[i] / now_period
                / settings.mpi_retern_factor
            )
        );

        mpi_b[i] = MESE_INDEX(
            settings.mpi_factor_b * player_count * (
                (history_rd[i] + history_mk[i])
                / (sum_history_rd + sum_history_mk)
            )
        );

        mpi_c[i] = MESE_INDEX(
            settings.mpi_factor_c * player_count * (
                size[i] / sum_size
            )
        );

        mpi_d[i] = MESE_INDEX(
            settings.mpi_factor_d * (
                1 - abs(prod_over[i])
            )
        );

        mpi_e[i] = MESE_INDEX(
            settings.mpi_factor_e * player_count * div(
                sold[i], sum_sold, 0
            )
        );

        mpi_f[i] = MESE_INDEX(
            min(
                settings.mpi_factor_f * div(
                    div(sold[i], last.sold[i], 0),
                    div(sum_sales, sum_last_sales, 0),
                    0
                ),
                2 * settings.mpi_factor_f
            )
        );

        mpi[i] = MESE_INDEX(
            mpi_a[i] + mpi_b[i] + mpi_c[i] + mpi_d[i] + mpi_e[i] + mpi_f[i]
        );
    }
}

void Period::serialize(std::ostream &stream) {
    stream.write(reinterpret_cast<const char *>(this), sizeof(*this));
}

}
