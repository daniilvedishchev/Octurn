#pragma once

enum class tokenType{
    // SPECIAL
    EndOfFile,
    NewLine,
    Unknown,

    // BOOLEAN
    And,
    Or,

    // LITERALS
    String,
    Number,
    Identifier,
    Timeframe,
    Date,
    Percent,

    // PUNCTUATION
    Assign,
    LeftParen,
    RightParen,
    Comma,

    // ARITHMETIC
    Minus,
    Plus,
    Divide,
    Multiply,

    // COMPARISON OPERATORS
    LessThan,
    GreaterThan,

    // KEYWORDS
    _strategy,
    _slippage,
    _mode,
    _stop,
    _take,
    _market,
    _session,
    _capital,
    _execution,
    _buy,
    _risk,
    _brokerfee,
    _stoploss,
    _takeprofit,
    _broker,
    _range,
    _date,
    _dailyMaxLoss,
    _maxAccountLoss
};