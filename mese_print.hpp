#pragma once

#include "mese.hpp"

namespace mese {

#define MESE_PRINT [&](auto val, auto arr, auto doc)

template <class T>
void Period::print_full(T callback) {
    callback(MESE_PRINT {
        val("player_count", player_count);
        val("now_period", now_period);

        doc("settings", MESE_PRINT {
            doc("limits", MESE_PRINT {
                val("price_max", settings.price_max);
                val("price_min", settings.price_min);
                val("mk_limit", settings.mk_limit);
                val("ci_limit", settings.ci_limit);
                val("rd_limit", settings.rd_limit);
                val("loan_limit", settings.loan_limit);
            });

            doc("production", MESE_PRINT {
                val("prod_rate_initial", settings.prod_rate_initial);
                val("prod_rate_balanced", settings.prod_rate_balanced);
                val("prod_rate_pow", settings.prod_rate_pow);
                val("prod_cost_factor_rate_over", settings.prod_cost_factor_rate_over);
                val("prod_cost_factor_rate_under", settings.prod_cost_factor_rate_under);
                val("prod_cost_factor_size", settings.prod_cost_factor_size);
                val("prod_cost_factor_const", settings.prod_cost_factor_const);

                val("unit_fee", settings.unit_fee);
                val("deprecation_rate", settings.deprecation_rate);
            });

            doc("balance", MESE_PRINT {
                val("initial_cash", settings.initial_cash);
                val("initial_capital", settings.initial_capital);

                val("interest_rate_cash", settings.interest_rate_cash);
                val("interest_rate_loan", settings.interest_rate_loan);
                val("inventory_fee", settings.inventory_fee);
                val("tax_rate", settings.tax_rate);
            });

            doc("orders", MESE_PRINT {
                val("mk_overload", settings.mk_overload);
                val("mk_compression", settings.mk_compression);

                val("demand", settings.demand);
                val("demand_price", settings.demand_price);
                val("demand_mk", settings.demand_mk);
                val("demand_rd", settings.demand_rd);

                val("demand_ref_price", settings.demand_ref_price);
                val("demand_ref_mk", settings.demand_ref_mk);
                val("demand_ref_rd", settings.demand_ref_rd);
                val("demand_pow_price", settings.demand_pow_price);
                val("demand_pow_mk", settings.demand_pow_mk);
                val("demand_pow_rd", settings.demand_pow_rd);

                val("share_price", settings.share_price);
                val("share_mk", settings.share_mk);
                val("share_rd", settings.share_rd);
                val("share_pow_price", settings.share_pow_price);
                val("share_pow_mk", settings.share_pow_mk);
                val("share_pow_rd", settings.share_pow_rd);

                val("price_overload", settings.price_overload);
            });

            doc("mpi", MESE_PRINT {
                val("mpi_retern_factor", settings.mpi_retern_factor);
                val("mpi_factor_a", settings.mpi_factor_a);
                val("mpi_factor_b", settings.mpi_factor_b);
                val("mpi_factor_c", settings.mpi_factor_c);
                val("mpi_factor_d", settings.mpi_factor_d);
                val("mpi_factor_e", settings.mpi_factor_e);
                val("mpi_factor_f", settings.mpi_factor_f);
            });
        });

        doc("decisions", MESE_PRINT {
            arr("price", decisions.price);
            arr("prod", decisions.prod);
            arr("mk", decisions.mk);
            arr("ci", decisions.ci);
            arr("rd", decisions.rd);
        });

        doc("data_early", MESE_PRINT {
            doc("production", MESE_PRINT {
                arr("prod_rate", prod_rate);
                arr("prod_over", prod_over);
                arr("prod_cost_unit", prod_cost_unit);
                arr("prod_cost_marginal", prod_cost_marginal);
                arr("prod_cost", prod_cost);
            });

            doc("balance", MESE_PRINT {
                arr("deprecation", deprecation);
                arr("capital", capital);
                arr("size", size);
                arr("spending", spending);
                arr("balance_early", balance_early);
                arr("loan_early", loan_early);
                arr("interest", interest);

                arr("goods", goods);
                arr("goods_cost", goods_cost);
                arr("goods_max_sales", goods_max_sales);
            });

            doc("history", MESE_PRINT {
                arr("history_mk", history_mk);
                arr("history_rd", history_rd);
            });
        });

        doc("data", MESE_PRINT {
            doc("orders", MESE_PRINT {
                val("average_price_given", average_price_given);
                val("average_price_planned", average_price_planned);
                val("average_price_mixed", average_price_mixed);
                val("demand_effect_mk", demand_effect_mk);
                val("demand_effect_rd", demand_effect_rd);
                val("orders_demand", orders_demand);

                arr("share_effect_price", share_effect_price);
                arr("share_effect_mk", share_effect_mk);
                arr("share_effect_rd", share_effect_rd);
                arr("share", share);
                arr("share_compressed", share_compressed);

                arr("orders", orders);
                arr("sold", sold);
                arr("inventory", inventory);
                arr("unfilled", unfilled);
            });

            doc("balance", MESE_PRINT {
                arr("goods_cost_sold", goods_cost_sold);
                arr("goods_cost_inventory", goods_cost_inventory);

                arr("sales", sales);
                arr("inventory_charge", inventory_charge);
                arr("cost_before_tax", cost_before_tax);
                arr("profit_before_tax", profit_before_tax);
                arr("tax_charge", tax_charge);
                arr("profit", profit);

                arr("balance", balance);
                arr("loan", loan);
                arr("cash", cash);
                arr("retern", retern);

                val("average_price", average_price);
            });

            doc("mpi", MESE_PRINT {
                arr("mpi_a", mpi_a);
                arr("mpi_b", mpi_b);
                arr("mpi_c", mpi_c);
                arr("mpi_d", mpi_d);
                arr("mpi_e", mpi_e);
                arr("mpi_f", mpi_f);
                arr("mpi", mpi);
            });
        });
    });
}

template <class T>
void Period::print_settings(T callback) {
    callback(MESE_PRINT {
        doc("limits", MESE_PRINT {
            val("price_max", settings.price_max);
            val("price_min", settings.price_min);
            val("mk_limit", settings.mk_limit);
            val("ci_limit", settings.ci_limit);
            val("rd_limit", settings.rd_limit);
            val("loan_limit", settings.loan_limit);
        });

        doc("production", MESE_PRINT {
            val("prod_rate_balanced", settings.prod_rate_balanced);
            val("prod_rate_pow", settings.prod_rate_pow);
            val("prod_cost_factor_rate_over", settings.prod_cost_factor_rate_over);
            val("prod_cost_factor_rate_under", settings.prod_cost_factor_rate_under);
            val("prod_cost_factor_size", settings.prod_cost_factor_size);
            val("prod_cost_factor_const", settings.prod_cost_factor_const);

            val("unit_fee", settings.unit_fee);
            val("deprecation_rate", settings.deprecation_rate);
        });

        doc("balance", MESE_PRINT {
            val("interest_rate_cash", settings.interest_rate_cash);
            val("interest_rate_loan", settings.interest_rate_loan);
            val("inventory_fee", settings.inventory_fee);
            val("tax_rate", settings.tax_rate);
        });
    });
}

template <class T>
void Period::print_decisions(size_t i, T callback) {
    callback(MESE_PRINT {
        val("price", decisions.price[i]);
        val("prod", decisions.prod[i]);
        val("mk", decisions.mk[i]);
        val("ci", decisions.ci[i]);
        val("rd", decisions.rd[i]);
    });
}

template <class T>
void Period::print_player_early(size_t i, T callback) {
    callback(MESE_PRINT {
        doc("production", MESE_PRINT {
            val("prod_rate", prod_rate[i]);
            val("prod_over", prod_over[i]);
            val("prod_cost_unit", prod_cost_unit[i]);
            val("prod_cost_marginal", prod_cost_marginal[i]);
            val("prod_cost", prod_cost[i]);
        });

        doc("balance", MESE_PRINT {
            val("deprecation", deprecation[i]);
            val("capital", capital[i]);
            val("size", size[i]);
            val("spending", spending[i]);
            val("balance_early", balance_early[i]);
            val("loan_early", loan_early[i]);
            val("interest", interest[i]);

            val("goods", goods[i]);
            val("goods_cost", goods_cost[i]);
            val("goods_max_sales", goods_max_sales[i]);
        });

        doc("history", MESE_PRINT {
            val("history_mk", history_mk[i]);
            val("history_rd", history_rd[i]);
        });
    });
}

template <class T>
void Period::print_player(size_t i, T callback) {
    callback(MESE_PRINT {
        doc("orders", MESE_PRINT {
            val("orders", orders[i]);
            val("sold", sold[i]);
            val("inventory", inventory[i]);
            val("unfilled", unfilled[i]);
        });

        doc("balance", MESE_PRINT {
            val("goods_cost_sold", goods_cost_sold[i]);
            val("goods_cost_inventory", goods_cost_inventory[i]);

            val("sales", sales[i]);
            val("inventory_charge", inventory_charge[i]);
            val("cost_before_tax", cost_before_tax[i]);
            val("profit_before_tax", profit_before_tax[i]);
            val("tax_charge", tax_charge[i]);
            val("profit", profit[i]);

            val("balance", balance[i]);
            val("loan", loan[i]);
            val("cash", cash[i]);
            val("retern", retern[i]);
        });

        doc("mpi", MESE_PRINT {
            val("mpi", mpi[i]);
        });
    });
}

template <class T>
void Period::print_public(T callback) {
    callback(MESE_PRINT {
        // val("player_count", player_count);
        // val("now_period", now_period);

        doc("decisions", MESE_PRINT {
            arr("price", decisions.price);
            val("average_prod", sum(decisions.prod) / player_count);
        });

        doc("data_early", MESE_PRINT {
            doc("production", MESE_PRINT {
                val("average_prod_cost_unit", sum(prod_cost_unit) / player_count);
                val("average_prod_cost", sum(prod_cost) / player_count);
            });

            doc("balance", MESE_PRINT {
                val("average_capital", sum(capital) / player_count);
                val("average_size", sum(size) / player_count);

                val("average_goods", sum(goods) / player_count);
            });
        });

        doc("data", MESE_PRINT {
            doc("orders", MESE_PRINT {
                val("average_price_given", average_price_given);

                val("average_orders", sum(orders) / player_count);
                arr("sold", sold);
                val("average_sold", sum(sold) / player_count);
                val("average_inventory", sum(inventory) / player_count);
                val("average_unfilled", sum(unfilled) / player_count);
            });

            doc("balance", MESE_PRINT {
                val("average_goods_cost_sold", sum(goods_cost_sold) / player_count);

                arr("sales", sales);
                val("average_sales", sum(sales) / player_count);
                arr("cost_before_tax", cost_before_tax);
                val("average_cost_before_tax", sum(cost_before_tax) / player_count);
                arr("profit_before_tax", profit_before_tax);
                val("average_profit_before_tax", sum(profit_before_tax) / player_count);
                arr("tax_charge", tax_charge);
                val("average_tax_charge", sum(tax_charge) / player_count);
                arr("profit", profit);
                val("average_profit", sum(profit) / player_count);

                arr("retern", retern);
                val("average_retern", sum(retern) / player_count);

                val("average_price", average_price);
            });

            doc("mpi", MESE_PRINT {
                arr("mpi", mpi);
            });
        });
    });
}

}
