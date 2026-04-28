#pragma once

#include <cstddef>
#include <string>

#include "TokenTypes.hpp"

struct Token {
    tokenType type{tokenType::EndOfFile};
    std::string lexeme;
    size_t line{1};
    size_t col{1};
};