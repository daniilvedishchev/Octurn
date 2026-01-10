#include "taLib.hpp"
#include <format>
#include <iostream>

// ================================================================================== //
// All functions in this file:
//   * Read market data from `variables_` (global variable environment)
//   * Read function arguments (periods, variable names, etc.) from `args`
// ================================================================================== //


// ================================================================================== //
// @brief Simple Moving Average (MA)
//
// @attention
//   args[0] - either:
//              * string: name of a variable in `variables_` that holds a price vector
//              * std::vector<double>: the price data directly
//   args[1] - period to smooth over:
//              * double: the period as a number
//              * string: name of a variable that contains a double period
//
// @return std::vector<double> of the same size as the input series.
//         First (period - 1) values are set to 0.0 (you can later replace them with NaN).
// ================================================================================== //

std::vector<double> MA(const multiValue& args,
                       std::unordered_map<std::string, AnyValue>& variables_) 
{
    std::vector<double> result;

    // --- 1. Validate type of the first argument: must be string or vector<double> --- //
    if (!std::holds_alternative<std::string>(args[0]) &&
        !std::holds_alternative<std::vector<double>>(args[0])) 
    {
        throw std::runtime_error("MA: first argument must be a ticker (string) or an array (vector<double>).");
    }

    std::string        series_name; // used if args[0] is a string (variable name)
    std::vector<double> series;     // actual price data

    // --- 2. Get the price series: either from variables_ or directly from args[0] --- //
    if (std::holds_alternative<std::string>(args[0])) {
        // args[0] is the name of a variable that holds a vector<double>
        series_name = std::get<std::string>(args[0]);
        series      = std::get<std::vector<double>>(variables_[series_name]);
    } else {
        // args[0] is already a vector<double>
        series = std::get<std::vector<double>>(args[0]);
    }

    // --- 3. Extract the period --- //
    double period = 0.0;

    if (std::holds_alternative<double>(args[1])) {
        // Period is given directly as a number
        period = std::get<double>(args[1]);
    } else if (std::holds_alternative<std::string>(args[1])) {
        // Period is stored in a variable, whose name is in args[1]
        const std::string& period_name = std::get<std::string>(args[1]);
        period = std::get<double>(variables_[period_name]);
        std::cout << std::format("MA: period taken from variable: {}\n", std::to_string(period));
    }

    // --- 4. Validate period and data size --- //
    if (period <= 0.0 || series.size() < static_cast<std::size_t>(period)) {
        throw std::runtime_error("MA: incorrect data or period used to calculate MA.");
    }

    std::size_t p = static_cast<std::size_t>(period);
    result.reserve(series.size()); // avoid reallocations

    // --- 5. Compute the sum of the first 'p' elements --- //
    double sum = 0.0;
    for (std::size_t i = 0; i < p; ++i) {
        sum += series[i];
    }

    // First (p - 1) values do not have a full window, we mark them as 0.0 for now.
    for (std::size_t i = 0; i < p - 1; ++i) {
        result.push_back(0.0); // can be changed to NaN later if desired
    }

    // First valid MA value corresponds to index (p - 1)
    result.push_back(sum / period);

    // --- 6. Sliding window for the remaining elements --- //
    for (std::size_t i = p; i < series.size(); ++i) {
        // Remove the element that falls out of the window and add the new one
        sum += series[i] - series[i - p];
        result.push_back(sum / period);
    }

    std::cout << "Successfully calculated MA\n";
    return result;
}

// ================================================================================== //
// @brief RSI (Relative Strength Index) using Wilder's smoothing.
//
// @attention
//   args[0] - string: name of the variable in `variables_` that contains price data
//   args[1] - double: period used to calculate average gain and loss
//
// @return std::vector<double> of the same size as the input price vector.
//         Values range from 0 to 100. Early values before the period are NaN.
// ================================================================================== //
std::vector<double> RSI(const multiValue& args,
                        const std::unordered_map<std::string, AnyValue>& variables_) 
{
    // --- 1. Extract and validate inputs --- //

    // 1.1 Get series name from args[0] (must be a string)
    const auto* series_name_ptr = std::get_if<std::string>(&args[0]);
    if (!series_name_ptr) {
        throw std::runtime_error("RSI: first argument must be a string with series name.");
    }

    // 1.2 Look up the series in the variables_ map
    auto it = variables_.find(*series_name_ptr);
    if (it == variables_.end()) {
        throw std::runtime_error("RSI: variable passed to the function is not defined.");
    }

    // 1.3 Extract the price vector (std::vector<double>) from AnyValue
    const auto* data_ptr = std::get_if<std::vector<double>>(&it->second);
    if (!data_ptr) {
        throw std::runtime_error("RSI: variable must be a std::vector<double>.");
    }
    const auto& data = *data_ptr;

    // 1.4 Extract the period from args[1] (must be a double)
    const auto* period_double_ptr = std::get_if<double>(&args[1]);
    if (!period_double_ptr) {
        throw std::runtime_error("RSI: second argument must be a number (double) for period.");
    }

    if (*period_double_ptr <= 0.0) {
        throw std::runtime_error("RSI: period must be a positive number.");
    }

    std::size_t period = static_cast<std::size_t>(*period_double_ptr);
    if (period < 1 || period >= data.size()) {
        std::cout << "period:" << period << "data-size:" << data.size() << "\n";
        throw std::runtime_error("RSI: period must be >= 1 and < data size.");
    }

    // --- 2. Prepare output vector --- //

    std::size_t n = data.size();
    // Initialize RSI with NaN, since the first 'period' values are not fully defined
    std::vector<double> rsi(n, std::numeric_limits<double>::quiet_NaN());

    // --- 3. Initial average gain and loss over the first 'period' bars --- //

    double gain_sum = 0.0;
    double loss_sum = 0.0;

    // Start from i = 1 because we use data[i] - data[i-1]
    for (std::size_t i = 1; i <= period; ++i) {
        double difference = data[i] - data[i - 1];
        if (difference > 0.0) {
            gain_sum += difference;              // upward move
        } else {
            loss_sum += std::abs(difference);    // downward move
        }
    }

    double inv_period = 1.0 / static_cast<double>(period);

    double avg_gain = gain_sum * inv_period;
    double avg_loss = loss_sum * inv_period;

    // Helper lambda to transform (avg_gain, avg_loss) into RSI value
    auto compute_rsi = [](double avg_gain, double avg_loss) -> double {
        // If there is no average loss:
        if (avg_loss == 0.0) {
            if (avg_gain == 0.0) return 50.0; // flat market: no gains, no losses
            return 100.0;                     // only gains: RSI at 100
        }

        double RS = avg_gain / avg_loss;
        return 100.0 - (100.0 / (1.0 + RS));
    };

    // First "valid" RSI value is at index = period
    rsi[period] = compute_rsi(avg_gain, avg_loss);

    // --- 4. Wilder's smoothing for the remaining bars --- //

    for (std::size_t i = period + 1; i < n; ++i) {
        double change = data[i] - data[i - 1];

        // Split the price change into gain or loss
        double gain = (change > 0.0) ? change : 0.0;
        double loss = (change < 0.0) ? -change : 0.0;

        // Wilder's smoothing:
        // avg_new = (avg_prev * (period - 1) + current_gain_or_loss) / period
        avg_gain = (avg_gain * (period - 1) + gain) * inv_period;
        avg_loss = (avg_loss * (period - 1) + loss) * inv_period;

        rsi[i] = compute_rsi(avg_gain, avg_loss);
    }

    return rsi;
}
