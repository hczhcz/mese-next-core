#include "mese.hpp"

namespace mese {

#define MESE_PRINT [&](auto val, auto arr, auto doc)

void Period::print_full(std::ostream &stream) {
    print(stream, MESE_PRINT {
        doc("settings", MESE_PRINT {
            doc("limits", MESE_PRINT {
                val("price_max", setting.price_max);
                val("price_min", setting.price_min);
                val("mk_limit", setting.mk_limit);
                val("ci_limit", setting.ci_limit);
                val("rd_limit", setting.rd_limit);
                val("loan_limit", setting.loan_limit);
            });

            doc("production", MESE_PRINT {
                val("prod_rate_initial", setting.prod_rate_initial);
                val("prod_rate_balanced", setting.prod_rate_balanced);
                val("prod_rate_pow", setting.prod_rate_pow);
                val("prod_cost_factor_rate_over", setting.prod_cost_factor_rate_over);
                val("prod_cost_factor_rate_under", setting.prod_cost_factor_rate_under);
                val("prod_cost_factor_size", setting.prod_cost_factor_size);
                val("prod_cost_factor_const", setting.prod_cost_factor_const);

                val("unit_fee", setting.unit_fee);
                val("deprecation_rate", setting.deprecation_rate);
            });

            doc("balance", MESE_PRINT {
                val("initial_cash", setting.initial_cash);
                val("initial_capital", setting.initial_capital);

                val("interest_rate_cash", setting.interest_rate_cash);
                val("interest_rate_loan", setting.interest_rate_loan);
                val("inventory_fee", setting.inventory_fee);
                val("tax_rate", setting.tax_rate);
            });

            doc("orders", MESE_PRINT {
                val("mk_overload", setting.mk_overload);
                val("mk_compression", setting.mk_compression);

                val("demand", setting.demand);
                val("demand_price", setting.demand_price);
                val("demand_mk", setting.demand_mk);
                val("demand_rd", setting.demand_rd);

                val("demand_ref_price", setting.demand_ref_price);
                val("demand_ref_mk", setting.demand_ref_mk);
                val("demand_ref_rd", setting.demand_ref_rd);
                val("demand_pow_price", setting.demand_pow_price);
                val("demand_pow_mk", setting.demand_pow_mk);
                val("demand_pow_rd", setting.demand_pow_rd);

                val("share_price", setting.share_price);
                val("share_mk", setting.share_mk);
                val("share_rd", setting.share_rd);
                val("share_pow_price", setting.share_pow_price);
                val("share_pow_mk", setting.share_pow_mk);
                val("share_pow_rd", setting.share_pow_rd);

                val("price_overload", setting.price_overload);
            });

            doc("mpi", MESE_PRINT {
                val("mpi_retern_factor", setting.mpi_retern_factor);
                val("mpi_factor_a", setting.mpi_factor_a);
                val("mpi_factor_b", setting.mpi_factor_b);
                val("mpi_factor_c", setting.mpi_factor_c);
                val("mpi_factor_d", setting.mpi_factor_d);
                val("mpi_factor_e", setting.mpi_factor_e);
                val("mpi_factor_f", setting.mpi_factor_f);
            });
        });

        doc("decisions", MESE_PRINT {
            arr("price", decision.price);
            arr("prod", decision.prod);
            arr("mk", decision.mk);
            arr("ci", decision.ci);
            arr("rd", decision.rd);
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

void Period::print_setting(std::ostream &stream) {
    print(stream, MESE_PRINT {
        doc("settings", MESE_PRINT {
            doc("limits", MESE_PRINT {
                val("price_max", setting.price_max);
                val("price_min", setting.price_min);
                val("mk_limit", setting.mk_limit);
                val("ci_limit", setting.ci_limit);
                val("rd_limit", setting.rd_limit);
                val("loan_limit", setting.loan_limit);
            });

            doc("production", MESE_PRINT {
                val("prod_rate_balanced", setting.prod_rate_balanced);
                val("prod_rate_pow", setting.prod_rate_pow);
                val("prod_cost_factor_rate_over", setting.prod_cost_factor_rate_over);
                val("prod_cost_factor_rate_under", setting.prod_cost_factor_rate_under);
                val("prod_cost_factor_size", setting.prod_cost_factor_size);
                val("prod_cost_factor_const", setting.prod_cost_factor_const);

                val("unit_fee", setting.unit_fee);
                val("deprecation_rate", setting.deprecation_rate);
            });

            doc("balance", MESE_PRINT {
                val("interest_rate_cash", setting.interest_rate_cash);
                val("interest_rate_loan", setting.interest_rate_loan);
                val("inventory_fee", setting.inventory_fee);
                val("tax_rate", setting.tax_rate);
            });
        });
    });
}

void Period::print_player_early(std::ostream &stream, size_t i) {
    print(stream, MESE_PRINT {
        doc("decisions", MESE_PRINT {
            val("price", decision.price[i]);
            val("prod", decision.prod[i]);
            val("mk", decision.mk[i]);
            val("ci", decision.ci[i]);
            val("rd", decision.rd[i]);
        });

        doc("data_early", MESE_PRINT {
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
    });
}

void Period::print_player(std::ostream &stream, size_t i) {
    print(stream, MESE_PRINT {
        doc("data", MESE_PRINT {
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
        });
    });
}

void Period::print_public(std::ostream &stream) {
    print(stream, MESE_PRINT {
        doc("decisions", MESE_PRINT {
            arr("price", decision.price);
        });

        doc("data", MESE_PRINT {
            doc("orders", MESE_PRINT {
                val("average_price_given", average_price_given);

                val("orders", sum(orders));
                arr("sold", sold);
                val("inventory", sum(inventory));
                val("unfilled", sum(unfilled));
            });

            doc("balance", MESE_PRINT {
                val("goods_cost_sold", sum(goods_cost_sold));

                arr("sales", sales);
                arr("profit", profit);

                arr("retern", retern);

                val("average_price", average_price);
            });

            doc("mpi", MESE_PRINT {
                arr("mpi", mpi);
            });
        });
    });
}

}
