#pragma once
#include <unordered_map>
#include "config/configTypes.hpp"

std::unordered_map<Slippage,std::unordered_map<std::string,double>> slippageTable;