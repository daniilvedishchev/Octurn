#pragma once

#include <string>
#include <vector>

using VecD  = std::vector<double>;
using Vec2D = std::vector<VecD>;

Vec2D get_ohlc(int id);

void polygon_data_ready(const int callbackId,const Vec2D& ohlc);

bool ohlc_ready(int id);

int request_ohlc(const std::string& ticker,
    int multiplier,
    const std::string& timespan,
    const std::string& from,
    const std::string& to);
