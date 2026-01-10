#pragma once
#include <iostream>
#include <vector>
#include <numeric>
#include <stdexcept>
#include <cmath>
#include <limits>
#include <utility>
#include <algorithm>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <Eigen/Dense>
#include <pybind11/eigen.h>

using Eigen::ArrayXd;
namespace py = pybind11;


class TechnicalIndicators {
public:
    // Constructor: initializes from std::vector<double>
    TechnicalIndicators(const py::array_t<double>& close_data)
        : close(Eigen::Map<const Eigen::ArrayXd>(close_data.data(), close_data.size())),
          n(close_data.size()) {}

    // Simple Moving Average (SMA)
    Eigen::ArrayXd MA(int period) const {
        if (n < period || period <= 0)
            throw std::invalid_argument("Invalid period or not enough data.");

        ArrayXd result = ArrayXd::Constant(n, std::numeric_limits<double>::quiet_NaN());
        double sum = close.head(period).sum();
        result(period - 1) = sum / period;

        for (int i = period; i < n; ++i) {
            sum += close(i) - close(i - period);
            result(i) = sum / period;
        }

        return result;
    }

    // Exponential Moving Average (EMA)
    ArrayXd EMA(const ArrayXd& input, int period) const {
        if (input.size() < period || period <= 0)
            throw std::invalid_argument("Invalid period or not enough data.");

        ArrayXd result = ArrayXd::Constant(input.size(), std::numeric_limits<double>::quiet_NaN());
        double alpha = 2.0 / (period + 1);
        double sma = input.head(period).mean();
        result(period - 1) = sma;

        for (int i = period; i < input.size(); ++i) {
            result(i) = alpha * input(i) + (1 - alpha) * result(i - 1);
        }

        return result;
    }

    // Average True Range (ATR)
    ArrayXd ATR(const ArrayXd& open,
                const ArrayXd& high,
                const ArrayXd& low,
                const ArrayXd& close_prices,
                int period) const {
        int size = close_prices.size();
        if (period <= 0 || high.size() != size || low.size() != size || open.size() != size)
            throw std::invalid_argument("Invalid input sizes or period.");

        ArrayXd tr = ArrayXd::Constant(size, std::numeric_limits<double>::quiet_NaN());

        for (int i = 1; i < size; ++i) {
            tr(i) = std::max({
                high(i) - low(i),
                std::abs(high(i) - close_prices(i - 1)),
                std::abs(low(i) - close_prices(i - 1))
            });
        }

        ArrayXd atr = ArrayXd::Constant(size, std::numeric_limits<double>::quiet_NaN());
        double initial_atr = tr.segment(1, period).mean();
        atr(period) = initial_atr;

        for (int i = period + 1; i < size; ++i) {
            atr(i) = (atr(i - 1) * (period - 1) + tr(i)) / period;
        }

        return atr;
    }

    // MACD Indicator HAS AN ERROR IN IT
    std::pair<ArrayXd, ArrayXd> MACD(int short_period, int long_period, int signal_period) const {
        ArrayXd ema_short = EMA(close, short_period);
        ArrayXd ema_long  = EMA(close, long_period);

        ArrayXd macd_line = ArrayXd::Constant(n, std::numeric_limits<double>::quiet_NaN());
        for (int i = 0; i < n; ++i) {
            if (!std::isnan(ema_short(i)) && !std::isnan(ema_long(i))) {
                macd_line(i) = ema_short(i) - ema_long(i);
            }
        }

        ArrayXd signal_line = EMA(macd_line, signal_period);

        return {macd_line, signal_line};
    }

    ArrayXd RSI(int period){
        ArrayXd price_changes = ArrayXd::Constant(n, std::numeric_limits<double>::quiet_NaN());
        ArrayXd RS = ArrayXd::Constant(n, std::numeric_limits<double>::quiet_NaN());
        price_changes.tail(n-1) = close.tail(n-1)-close.head(n-1);
        
        ArrayXd gains = price_changes.unaryExpr([](double x){return std::isnan(x) ? 0.0 : std::max(x,0.0);}).eval();
        ArrayXd losses = price_changes.unaryExpr([](double x){return std::isnan(x) ? 0.0 : -std::min(x,0.0);}).eval();

        for (size_t i = period;i<n;i++){
            RS(i) = price_changes.segment(i,i+period).mean();
        }

        ArrayXd RSI = RS.unaryExpr([](double x){return std::isnan(x) ? 0.0 : 100-100/(1+x);}).eval();

        return RSI;
    }

private:
    ArrayXd close;
    int n;
};

PYBIND11_MODULE(technicalindicators, m) {
    py::class_<TechnicalIndicators>(m, "TechnicalIndicators")
        .def(py::init<py::array_t<double>>())
        .def("MA", &TechnicalIndicators::MA,py::arg("period"))
        .def("EMA", &TechnicalIndicators::EMA,py::arg("input"),py::arg("period"))
        .def("ATR", &TechnicalIndicators::ATR,py::arg("open"),py::arg("high"),py::arg("low"),py::arg("close"),py::arg("period"))
        .def("MACD", &TechnicalIndicators::MACD,py::arg("short_period"),py::arg("long_period"),py::arg("signal_period"))
        .def("RSI", &TechnicalIndicators::RSI,py::arg("period"));
}
