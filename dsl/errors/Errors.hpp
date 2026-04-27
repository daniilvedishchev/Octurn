#pragma once
#include <stdexcept>
#include <string>

#include "dsl/token/Token.hpp"

struct SessionError : public std::runtime_error {
    SessionError(Token& token) : std::runtime_error("Invalid session type: " + token.lexeme + " at "
    + std::to_string(token.line) + " : " + std::to_string(token.col)+ "available sessions: [new-york, london, tokyo, all]\n"){}
};

struct ModeError : public std::runtime_error {
    ModeError(Token& token) : std::runtime_error("Invalid mode type: " + token.lexeme + " at "
    + std::to_string(token.line) + " : " + std::to_string(token.col)){}
};