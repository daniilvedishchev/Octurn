#pragma once
#include <functional>
#include <variant>
#include <string>
#include <optional>
#include <unordered_map>
#include "types/types.hpp"

using Octurn::AnyValue;

enum class ValueType {Double,String};
enum class Slippage {optimistic,base,pessimistic};

struct SlippageParams {
    double maxParticipation;
    double impactCoef;
};

std::unordered_map<Slippage,std::unordered_map<std::string,double>> SlippageCfg;