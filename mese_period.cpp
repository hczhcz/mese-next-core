#include "mese.hpp"

namespace mese {

Period::Period(size_t count, Settings &&_settings):
    PeriodDataEarly {},
    PeriodData {},

    player_count {count},
    now_period {0},

    settings(std::move(_settings)), // move
    decisions {}
{
    for (size_t i = 0; i < player_count; ++i) {
        capital[i] = settings.initial_capital / player_count;
        size[i] = capital[i] / settings.unit_fee;
        history_mk[i] = 0;
        history_rd[i] = 0;

        sold[i] = size[i] * settings.prod_rate_balanced;
        inventory[i] = 0;
        goods_cost_inventory[i] = 0;

        sales[i] = settings.demand_ref_price * sold[i];
        loan[i] = 0;
        cash[i] = settings.initial_cash / player_count; // C + 168000 = 182700 + L
        retern[i] = 0;
    }

    average_price = settings.demand_ref_price;
}

Period::Period(size_t count, Period &last, Settings &&_settings):
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
    Period &last, size_t i,
    double price, double prod, double mk, double ci, double rd
) {
    decisions.price[i] = price;
    decisions.prod[i] = prod;
    decisions.mk[i] = mk;
    decisions.ci[i] = ci;
    decisions.rd[i] = rd;

    prod_rate[i] = decisions.prod[i] / last.size[i];
    prod_over[i] = prod_rate[i] - settings.prod_rate_balanced;

    double prod_cost_factor_rate = (
        prod_over[i] > 0 ?
        settings.prod_cost_factor_rate_over :
        settings.prod_cost_factor_rate_under
    );
    prod_cost_unit[i] = (
        prod_cost_factor_rate * pow(prod_over[i], settings.prod_rate_pow)
        + settings.prod_cost_factor_size
            * settings.initial_capital / player_count / last.capital[i]
        + settings.prod_cost_factor_const
    );
    prod_cost_marginal[i] = ( // D(prod_cost(prod), prod)
        prod_cost_factor_rate
            * settings.prod_rate_pow
            * prod_rate[i] * pow(prod_over[i], settings.prod_rate_pow - 1)
        + prod_cost_unit[i]
    );
    prod_cost[i] = prod_cost_unit[i] * decisions.prod[i];

    deprecation[i] = last.capital[i] * settings.deprecation_rate;
    capital[i] = last.capital[i] + decisions.ci[i] - deprecation[i];
    size[i] = capital[i] / settings.unit_fee;

    spending[i] = (
        prod_cost[i]
        + decisions.ci[i] - deprecation[i]
        + decisions.mk[i] + decisions.rd[i]
    );
    balance_early[i] = last.cash[i] - last.loan[i] - spending[i];
    loan_early[i] = max(- balance_early[i], 0);
    interest[i] = (
        loan_early[i] == 0 ?
        - settings.interest_rate_cash * last.cash[i] :
        settings.interest_rate_loan * loan_early[i]
    );

    goods[i] = last.inventory[i] + decisions.prod[i];
    goods_cost[i] = last.goods_cost_inventory[i] + prod_cost[i];
    goods_max_sales[i] = decisions.price[i] * goods[i];

    history_mk[i] = last.history_mk[i] + decisions.mk[i];
    history_rd[i] = last.history_rd[i] + decisions.rd[i];

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
    double sum_mk_compressed = (
        sum_mk > settings.mk_overload ? (
            (sum_mk - settings.mk_overload) * settings.mk_compression
            + settings.mk_overload
        ) : sum_mk
    );
    double sum_history_mk = sum(history_mk);
    double sum_history_rd = sum(history_rd);

    average_price_given = sum(decisions.price) / player_count;
    average_price_planned = div(
        sum(goods_max_sales), sum(goods), average_price_given
    );
    average_price_mixed = (
        settings.demand_price * average_price_planned
        + (1 - settings.demand_price) * last.average_price
    );

    demand_effect_mk = settings.demand_mk * (
        pow(sum_mk_compressed, settings.demand_pow_mk)
        / pow(settings.demand_ref_mk, settings.demand_pow_mk)
    ) / (
        pow(average_price_mixed, settings.demand_pow_price)
        / pow(settings.demand_ref_price, settings.demand_pow_price)
    );
    demand_effect_rd = settings.demand_rd * (
        pow(sum_history_rd / now_period, settings.demand_pow_rd)
        / pow(settings.demand_ref_rd, settings.demand_pow_rd)
    );
    orders_demand = settings.demand * (
        demand_effect_rd + demand_effect_mk
    );

    for (size_t i = 0; i < player_count; ++i) {
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

    for (size_t i = 0; i < player_count; ++i) {
        // orders

        share[i] = (
            settings.share_price * div(share_effect_price[i], sum_share_effect_price, 0)
            + settings.share_mk * div(share_effect_mk[i], sum_share_effect_mk, 0)
            + settings.share_rd * div(share_effect_rd[i], sum_share_effect_rd, 0)
        );

        share_compressed[i] =  (
            decisions.price[i] > settings.price_overload ? (
                share[i] * settings.price_overload / decisions.price[i]
            ) : share[i]
        );

        orders[i] = orders_demand * share_compressed[i];
        sold[i] = min(orders[i], goods[i]);
        inventory[i] = goods[i] - sold[i];
        unfilled[i] = orders[i] - sold[i];

        // goods

        goods_cost_sold[i] = goods_cost[i] * div(sold[i], goods[i], 0);
        goods_cost_inventory[i] = goods_cost[i] - goods_cost_sold[i];

        // cash flow

        sales[i] = decisions.price[i] * sold[i];

        inventory_charge[i] = min(
            last.inventory[i],
            inventory[i]
        ) * settings.inventory_fee;

        cost_before_tax[i] = (
            goods_cost_sold[i]
            + deprecation[i]
            + decisions.mk[i] + decisions.rd[i]
            + interest[i] + inventory_charge[i]
        );
        profit_before_tax[i] = sales[i] - cost_before_tax[i];
        tax_charge[i] = profit_before_tax[i] * settings.tax_rate;
        profit[i] = profit_before_tax[i] - tax_charge[i];

        // balance[i] = (
        //     last.cash[i] + loan_early[i] - last.loan[i]
        //     + sales[i]
        //     - spending[i] - deprecation[i]
        //     - interest[i] - inventory_charge[i] - tax_charge[i]
        // );
        balance[i] = (
            last.cash[i] + loan_early[i] - last.loan[i]
            + profit[i]
            - decisions.ci[i] + deprecation[i]
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
            settings.mpi_factor_a * player_count * (
                retern[i] / now_period
                / settings.mpi_retern_factor
            )
        );

        mpi_b[i] = (
            settings.mpi_factor_b * player_count * (
                (history_rd[i] + history_mk[i])
                / (sum_history_rd + sum_history_mk)
            )
        );

        mpi_c[i] = (
            settings.mpi_factor_c * player_count * (
                size[i] / sum_size
            )
        );

        mpi_d[i] = (
            settings.mpi_factor_d * (
                1 - abs(prod_over[i])
            )
        );

        mpi_e[i] = (
            settings.mpi_factor_e * player_count * div(
                sold[i], sum_sold, 0
            )
        );

        mpi_f[i] = min(
            settings.mpi_factor_f * div(
                div(sold[i], last.sold[i], 0),
                div(sum_sales, sum_last_sales, 0),
                0
            ),
            2 * settings.mpi_factor_f
        );

        mpi[i] = mpi_a[i] + mpi_b[i] + mpi_c[i] + mpi_d[i] + mpi_e[i] + mpi_f[i];
    }
}

void Period::serialize(std::ostream &stream) {
    stream.write(reinterpret_cast<const char *>(this), sizeof(*this));
}

}
