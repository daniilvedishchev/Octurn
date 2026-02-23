#pragma once
#include "types/types.hpp"

using Octurn::multiValue;
using Octurn::AnyValue;

std::vector<double> MA(const multiValue& args, std::unordered_map<std::string,AnyValue>&variables_);
std::vector<double> RSI(const multiValue& args, std::unordered_map<std::string,AnyValue>& data, std::unordered_map<std::string,AnyValue>&variables_);
