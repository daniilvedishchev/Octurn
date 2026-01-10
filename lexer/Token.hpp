#pragma once

#include <string>
#include <optional>
#include <unordered_map>

// ===============================================
//                Token Type Enum
// ===============================================
enum class Tokentype {
    Identifier,Greater,Less,
    Comma,LeftParen,RightParen,
    LeftBrace,RightBrace,End,Number,LeftSBracket,RightSBracket,
    Colon,When,Crosses,Above,Below,
    Buy,Sell,All,Strategy,Parameters,
    Indicators, Entry, Exit, Config, False, True,
    Equals,Function,Operator,Ticker,String, Data, Date
};

// ================================================
//   Saves operator types, easier to parse and expand
// ===============================================
enum class OperatorType {
    Crosses,CrossesAbove,CrossesBelow,
    And,Or,Plus,Minus,Greater,Less,Multiply,Divide,Equal,None
};

enum class Origin {
    Mathematical,Logical,Domain,Comparison,None
};

inline const std::unordered_map<OperatorType,Origin>operator_type_origin_mapper{
    {OperatorType::Plus,Origin::Mathematical},
    {OperatorType::Minus,Origin::Mathematical},
    {OperatorType::Multiply,Origin::Mathematical},
    {OperatorType::Divide,Origin::Mathematical},
    {OperatorType::And,Origin::Logical},
    {OperatorType::Or,Origin::Logical},
    {OperatorType::Equal, Origin::Comparison},
    {OperatorType::Greater, Origin::Comparison},
    {OperatorType::Less, Origin::Comparison},
    {OperatorType::CrossesAbove,Origin::Domain},
    {OperatorType::CrossesBelow,Origin::Domain}
};

// ===============================================
//         Actions for entry/exit sections
// ===============================================
enum class ActionType {
    Buy,
    Sell,
    Close,
    Alert
};

// ===============================================
//                Token Structure
// ===============================================
struct Token {

    // ==== Token characteristics ==== //
    Tokentype token_type;
    std::string value;

    // ==== Track token position ==== //
    size_t lineNum;
    size_t colNum;

    // =================== OPTIONAL =================== //
    // ==== Token may have operator type in case if it's initially an operator ==== //
    std::optional<OperatorType> operator_type;

    Origin operator_origin_ = operator_type ? 
        operator_type_origin_mapper.at(*operator_type) : Origin::None;
};

// ===============================================
//        Utility function to print tokens 
//                  (debugging)
// ===============================================
inline std::string to_string(Tokentype type) {
    switch (type) {
        case Tokentype::Identifier: return "Identifier";
        case Tokentype::Greater: return "Greater";
        case Tokentype::Less: return "Less";
        case Tokentype::Comma: return "Comma";
        case Tokentype::LeftParen: return "LeftParen";
        case Tokentype::RightParen: return "RightParen";
        case Tokentype::LeftBrace: return "LeftBrace";
        case Tokentype::RightBrace: return "RightBrace";
        case Tokentype::End: return "End";
        case Tokentype::Number: return "Number";
        case Tokentype::Colon: return "Colon";
        case Tokentype::When: return "When";
        case Tokentype::Crosses: return "Crosses";
        case Tokentype::Above: return "Above";
        case Tokentype::Below: return "Below";
        case Tokentype::Buy: return "Buy";
        case Tokentype::Sell: return "Sell";
        case Tokentype::All: return "All";
        case Tokentype::Strategy: return "Strategy";
        case Tokentype::Parameters: return "Parameters";
        case Tokentype::Indicators: return "Indicators";
        case Tokentype::Entry: return "Entry";
        case Tokentype::Exit: return "Exit";
        case Tokentype::Config: return "Config";
        case Tokentype::False: return "False";
        case Tokentype::True: return "True";
        case Tokentype::RightSBracket: return "RightSBracket";
        case Tokentype::LeftSBracket: return "LeftSBracket";
        case Tokentype::Date: return "Date";
        case Tokentype::Operator: return "Operator";      
        default: return "Unknown";
    }
}
