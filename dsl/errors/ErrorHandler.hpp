#pragma once
#include <stdexcept>
#include <string>

#include "dsl/token/Token.hpp"

inline std::string makeErrorMessage(
    const Token& token,
    const std::string& field,
    const std::string& options){
    return "Invalid " + field + " type: " + token.lexeme + " at "
        + std::to_string(token.line) + " : " + std::to_string(token.col)
        + " available options: [" + options + "]";
}

struct SessionError : public std::runtime_error {
    SessionError(const Token& token)
        : std::runtime_error(makeErrorMessage(token, "session", "new-york, london, tokyo, all")) {}
};

struct ModeError : public std::runtime_error {
    ModeError(const Token& token)
        : std::runtime_error(makeErrorMessage(token, "mode", "live, backtest")) {}
};

struct SlippageError : public std::runtime_error {
    SlippageError(const Token& token)
        : std::runtime_error(makeErrorMessage(token, "slippage", "pessimistic, base, optimistic")) {}
};

struct FillError : public std::runtime_error {
    FillError(const Token& token)
        : std::runtime_error(makeErrorMessage(token, "fill", "fok, gtc")) {}
};

struct ExecutionError : public std::runtime_error {
    ExecutionError(const Token& token)
        : std::runtime_error(makeErrorMessage(token, "execution", "market, limit")) {}
};
