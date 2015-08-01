#include "mese.hpp"

namespace mese {

void Period::print_title(
    std::ostream &stream,
    std::string title
) {
    stream << title << std::endl;
}

void Period::print_group(
    std::ostream &stream,
    std::string name
) {
    stream << "    " << name << std::endl;
}

void Period::print_val(
    std::ostream &stream,
    std::string name,
    double value
) {
    stream << "        " << name << ": " << value << std::endl;
}

void Period::print_arr(
    std::ostream &stream,
    std::string name,
    double (&member)[MAX_PLAYER]
) {
    stream << "        " << name << ": " << member[0];

    for (size_t i = 1; i < player_count; ++i) {
        stream << ", " << member[i];
    }

    stream << std::endl;
}

void Period::debug(std::ostream &stream) {
    print_title(stream, "settings");

    print_group(stream, "decision limits");

    print_val(stream, "price_max", setting.price_max);
    print_val(stream, "price_min", setting.price_min);
    print_val(stream, "mk_limit", setting.mk_limit);
    print_val(stream, "ci_limit", setting.ci_limit);
    print_val(stream, "rd_limit", setting.rd_limit);
    print_val(stream, "loan_limit", setting.loan_limit);

    print_group(stream, "costs related");

    print_val(stream, "prod_rate_initial", setting.prod_rate_initial);
    print_val(stream, "prod_rate_balanced", setting.prod_rate_balanced);
    print_val(stream, "prod_rate_pow", setting.prod_rate_pow);
    print_val(stream, "prod_cost_factor_rate_over", setting.prod_cost_factor_rate_over);
    print_val(stream, "prod_cost_factor_rate_under", setting.prod_cost_factor_rate_under);
    print_val(stream, "prod_cost_factor_size", setting.prod_cost_factor_size);
    print_val(stream, "prod_cost_factor_const", setting.prod_cost_factor_const);

    print_val(stream, "initial_cash", setting.initial_cash);
    print_val(stream, "initial_capital", setting.initial_capital);
    print_val(stream, "deprecation_rate", setting.deprecation_rate);

    print_val(stream, "interest_rate_cash", setting.interest_rate_cash);
    print_val(stream, "interest_rate_loan", setting.interest_rate_loan);
    print_val(stream, "inventory_fee", setting.inventory_fee);
    print_val(stream, "unit_fee", setting.unit_fee);
    print_val(stream, "tax_rate", setting.tax_rate);

    print_group(stream, "orders related");

    print_val(stream, "mk_overload", setting.mk_overload);
    print_val(stream, "mk_compression", setting.mk_compression);

    print_val(stream, "demand", setting.demand);
    print_val(stream, "demand_price", setting.demand_price);
    print_val(stream, "demand_mk", setting.demand_mk);
    print_val(stream, "demand_rd", setting.demand_rd);

    print_val(stream, "demand_ref_price", setting.demand_ref_price);
    print_val(stream, "demand_ref_mk", setting.demand_ref_mk);
    print_val(stream, "demand_ref_rd", setting.demand_ref_rd);
    print_val(stream, "demand_pow_price", setting.demand_pow_price);
    print_val(stream, "demand_pow_mk", setting.demand_pow_mk);
    print_val(stream, "demand_pow_rd", setting.demand_pow_rd);

    print_val(stream, "share_price", setting.share_price);
    print_val(stream, "share_mk", setting.share_mk);
    print_val(stream, "share_rd", setting.share_rd);
    print_val(stream, "share_pow_price", setting.share_pow_price);
    print_val(stream, "share_pow_mk", setting.share_pow_mk);
    print_val(stream, "share_pow_rd", setting.share_pow_rd);

    print_val(stream, "price_overload", setting.price_overload);

    print_group(stream, "mpi related");

    print_val(stream, "mpi_retern_factor", setting.mpi_retern_factor);
    print_val(stream, "mpi_factor_a", setting.mpi_factor_a);
    print_val(stream, "mpi_factor_b", setting.mpi_factor_b);
    print_val(stream, "mpi_factor_c", setting.mpi_factor_c);
    print_val(stream, "mpi_factor_d", setting.mpi_factor_d);
    print_val(stream, "mpi_factor_e", setting.mpi_factor_e);
    print_val(stream, "mpi_factor_f", setting.mpi_factor_f);

    print_title(stream, "decisions");

    print_arr(stream, "price", decision.price);
    print_arr(stream, "prod", decision.prod);
    print_arr(stream, "mk", decision.mk);
    print_arr(stream, "ci", decision.ci);
    print_arr(stream, "rd", decision.rd);

    print_title(stream, "period data");

    print_group(stream, "early");

    print_arr(stream, "prod_rate", prod_rate);
    print_arr(stream, "prod_over", prod_over);
    print_arr(stream, "prod_cost_unit", prod_cost_unit);
    print_arr(stream, "prod_cost_marginal", prod_cost_marginal);
    print_arr(stream, "prod_cost", prod_cost);

    print_arr(stream, "deprecation", deprecation);
    print_arr(stream, "capital", capital);
    print_arr(stream, "size", size);
    print_arr(stream, "spending", spending);
    print_arr(stream, "balance_early", balance_early);
    print_arr(stream, "loan_early", loan_early);
    print_arr(stream, "interest", interest);

    print_arr(stream, "goods", goods);
    print_arr(stream, "goods_cost", goods_cost);
    print_arr(stream, "goods_max_sales", goods_max_sales);

    print_arr(stream, "history_mk", history_mk);
    print_arr(stream, "history_rd", history_rd);

    print_group(stream, "orders related");

    print_val(stream, "average_price_given", average_price_given);
    print_val(stream, "average_price_planned", average_price_planned);
    print_val(stream, "average_price_mixed", average_price_mixed);
    print_val(stream, "demand_effect_mk", demand_effect_mk);
    print_val(stream, "demand_effect_rd", demand_effect_rd);
    print_val(stream, "orders_demand", orders_demand);

    print_arr(stream, "share_effect_price", share_effect_price);
    print_arr(stream, "share_effect_mk", share_effect_mk);
    print_arr(stream, "share_effect_rd", share_effect_rd);
    print_arr(stream, "share", share);
    print_arr(stream, "share_compressed", share_compressed);

    print_arr(stream, "orders", orders);
    print_arr(stream, "sold", sold);
    print_arr(stream, "inventory", inventory);
    print_arr(stream, "unfilled", unfilled);

    print_group(stream, "balance related");

    print_arr(stream, "goods_cost_sold", goods_cost_sold);
    print_arr(stream, "goods_cost_inventory", goods_cost_inventory);

    print_arr(stream, "sales", sales);
    print_arr(stream, "inventory_charge", inventory_charge);
    print_arr(stream, "cost_before_tax", cost_before_tax);
    print_arr(stream, "profit_before_tax", profit_before_tax);
    print_arr(stream, "tax_charge", tax_charge);
    print_arr(stream, "profit", profit);

    print_arr(stream, "balance", balance);
    print_arr(stream, "loan", loan);
    print_arr(stream, "cash", cash);
    print_arr(stream, "retern", retern);

    print_val(stream, "average_price", average_price);

    print_group(stream, "mpi");

    print_arr(stream, "mpi_a", mpi_a);
    print_arr(stream, "mpi_b", mpi_b);
    print_arr(stream, "mpi_c", mpi_c);
    print_arr(stream, "mpi_d", mpi_d);
    print_arr(stream, "mpi_e", mpi_e);
    print_arr(stream, "mpi_f", mpi_f);
    print_arr(stream, "mpi", mpi);
}

}
