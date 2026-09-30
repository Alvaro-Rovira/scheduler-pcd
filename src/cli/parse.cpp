#include "cli/parse.h"

#include <stdexcept>

int parseInt(const std::string& text, const std::string& what, int minimum) {
    std::size_t used = 0;
    int value = 0;
    try {
        value = std::stoi(text, &used);
    } catch (const std::exception&) {
        throw std::runtime_error(what + ": '" + text + "' no es un numero");
    }
    if (used != text.size()) throw std::runtime_error(what + ": '" + text + "' no es un numero");
    if (value < minimum) throw std::runtime_error(what + ": tiene que ser >= " + std::to_string(minimum));
    return value;
}
