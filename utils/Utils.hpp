#pragma once
#include <string>
#include "interpreter/Interpreter.hpp"
#include "types/types.hpp"

// ===============================================
//                Helper functions
// ===============================================

// Used to convert tokenized version in real bool
bool str_to_bool(const std::string& s);

std::string print_any_value(Octurn::AnyValue& value);

void printVariables(Interpreter& interp);

void printTrades();
