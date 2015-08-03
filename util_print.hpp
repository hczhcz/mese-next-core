#pragma once

#include <string>
#include <iostream>

namespace mese {

template <class T>
void print(std::ostream &stream, size_t arr_size, T callback) {
    size_t indent {0};

    auto print_indent = [&]() {
        stream << std::endl;
        for (size_t i = 0; i < indent; ++i) {
            stream << "    ";
        }
    };

    auto val_handler = [&](double value) {
        stream << value;
    };

    auto arr_handler = [&](double *member) {
        stream << '[' << member[0];

        for (size_t i = 1; i < arr_size; ++i) {
            stream << ", " << member[i];
        }

        stream << ']';
    };

    auto doc_handler = [&](auto self, auto callback) {
        stream << '{';
        ++indent;

        auto m_val_handler = [&](
            const std::string &name,
            double value
        ) {
            print_indent();
            stream << name << ": ";
            val_handler(value);
            stream << ',';
        };

        auto m_arr_handler = [&](
            const std::string &name,
            double *member
        ) {
            print_indent();
            stream << name << ": ";
            arr_handler(member);
            stream << ',';
        };

        auto m_doc_handler = [&](
            const std::string &name,
            auto callback
        ) {
            print_indent();
            stream << name << ": ";
            self(self, callback);
            stream << ',';
        };

        callback(m_val_handler, m_arr_handler, m_doc_handler);

        --indent;
        print_indent();
        stream << '}';
    };

    doc_handler(doc_handler, callback);
}

}
