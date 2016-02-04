#pragma once

#include <string>
#include <iostream>

namespace mese {

template <class T>
void print(std::ostream &stream, uint64_t arr_size, T callback) {
    stream.precision(2);
    stream.setf(std::ios::fixed);

    auto print_indent = [&](size_t indent) {
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

        for (uint64_t i = 1; i < arr_size; ++i) {
            stream << ", " << member[i];
        }

        stream << ']';
    };

    auto doc_handler = [&](auto self, size_t indent, auto callback) {
        stream << '{';

        auto m_val_handler = [&](
            const std::string &name,
            double value
        ) {
            print_indent(indent + 1);
            stream << name << ": ";
            val_handler(value);
            stream << ',';
        };

        auto m_arr_handler = [&](
            const std::string &name,
            double *member
        ) {
            print_indent(indent + 1);
            stream << name << ": ";
            arr_handler(member);
            stream << ',';
        };

        auto m_doc_handler = [&](
            const std::string &name,
            auto callback
        ) {
            print_indent(indent + 1);
            stream << name << ": ";
            self(self, indent + 1, callback);
            stream << ',';
        };

        callback(m_val_handler, m_arr_handler, m_doc_handler);

        print_indent(indent);
        stream << '}';
    };

    doc_handler(doc_handler, 0, callback);
}

}
