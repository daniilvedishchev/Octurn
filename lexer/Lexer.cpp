#include "Lexer.hpp"
#include <fstream>
#include <iostream>
#include <cctype>
#include <unordered_map>
#include <sstream>
#include <string>
#include <regex>

// ==== Reverved words by DSL ==== //
const std::unordered_map<std::string, Tokentype> Lexer::keywords_ = {
    {"when", Tokentype::When},
    {"crosses", Tokentype::Crosses},
    {"above", Tokentype::Above},
    {"below", Tokentype::Below},
    {"buy", Tokentype::Buy},
    {"sell", Tokentype::Sell},
    {"all", Tokentype::All},
    {"false", Tokentype::False},
    {"true", Tokentype::True},
    {"strategy", Tokentype::Strategy},
    {"parameters", Tokentype::Parameters},
    {"indicators", Tokentype::Indicators},
    {"data", Tokentype::Data},
    {"entry", Tokentype::Entry},
    {"exit", Tokentype::Exit},
    {"config", Tokentype::Config},
};

// ==== Defines operator types ==== //
const std::unordered_map<std::string, OperatorType> Lexer::operators_ = {
    // Logical operators
    {"and", OperatorType::And},
    {"or", OperatorType::Or},

    // Comparison operators
    {">", OperatorType::Greater},
    {"<", OperatorType::Less},
    {"==", OperatorType::Equal},

    // Arithmetic operators
    {"+", OperatorType::Plus},
    {"-", OperatorType::Minus},
    {"*", OperatorType::Multiply},
    {"/", OperatorType::Divide},

    // Special domain operators
    {"crosses", OperatorType::Crosses},
    {"crosses_above", OperatorType::CrossesAbove},
    {"crosses_below", OperatorType::CrossesBelow}
};

Lexer::Lexer(const std::string& strategy_text) : pos_(0), lineNum_(1), colNum_(1) {
    // ==== Read strategy as string ==== //
    input_ = strategy_text;
    input_size_ = input_.size();
}

void Lexer::read_file_to_string(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf(); // Считываем весь файл в строковый поток

    input_ = buffer.str();
}

void Lexer::skip_whitespace() {
    while (pos_ < input_size_) {
        char c = input_[pos_];
        if (c == ' ' || c == '\t') {
            ++pos_;
            ++colNum_;
        } else if (c == '\n') {
            ++pos_;
            ++lineNum_;
            colNum_ = 1;
        } else {
            break;
        }
    }
}


std::string Lexer::get_word_or_symbol() {
    size_t start = pos_;

    // Word or '_'
    if (std::isalpha(input_[pos_]) || input_[pos_] == '_') {
        while (pos_ < input_size_ && (std::isalnum(input_[pos_]) || input_[pos_] == '_')) {
            ++pos_;
            ++colNum_;
        }
    } 
    // Operator-symbol (+ - * / > < >= <=)
    else if (std::string("+-*/><=").find(input_[pos_]) != std::string::npos) {
        ++pos_;
        ++colNum_;

        if (pos_ < input_size_ && input_[pos_] == '=') { // for "==", ">=", "<="
            ++pos_;
            ++colNum_;
        }
    } 
    // Special symbols
    else {
        ++pos_;
        ++colNum_;
    }

    return input_.substr(start, pos_ - start);
}


std::string Lexer::get_number() {
    size_t start = pos_;
    while (pos_ < input_size_ && std::isdigit(input_[pos_])) {
        ++pos_;
        ++colNum_;
    }
    return input_.substr(start, pos_ - start);
}

// ====================================================== //
//                     Get Date 
// - Returns date as string
// - Doesn't move position till it's confirmed to be a date
// - Format : %Y-%M-%D
// ====================================================== //

std::string Lexer::get_date() {
    
    size_t start = pos_;
    size_t tmp_pos = pos_;
    size_t tmp_colNum = colNum_;

    while (tmp_pos < input_size_ && (std::isdigit(input_[tmp_pos])||input_[tmp_pos] == '-')) {
        ++tmp_pos;
        ++tmp_colNum;
    }
    auto date = input_.substr(start, tmp_pos - start);
    if (std::regex_match(date,std::regex(R"(^\d{4}-\d{2}-\d{2}$)"))){
            colNum_ = tmp_colNum;
            pos_ = tmp_pos;
            std::cout<<date<<"DATE PARSED\n";
            return date;
        }
    return "";
}
// ====================================================== //


bool Lexer::is_operator_symbol(char character) {
    return std::string("+-*/><").find(character) != std::string::npos;
}

Token Lexer::next_token() {
    skip_whitespace();
    if (pos_ >= input_size_) return {Tokentype::End, "", lineNum_, colNum_};

    char c = input_[pos_];
    size_t startCol = colNum_;

    // ====== Symbol operators ( >, <, >=, <=, +, -, * etc ) ======
    if (is_operator_symbol(c)) {
        std::string symbol = get_word_or_symbol(); 
        auto it_operator_type = operators_.find(symbol);
        if (it_operator_type != operators_.end()) {
            std::cout<<"Found operator:"<<symbol<<"\n";
            return {Tokentype::Operator, symbol, lineNum_, startCol, it_operator_type->second};
        }
    } else if (std::isalpha(c) || c == '_'){
        std::string word = get_word_or_symbol();

        auto it_keywords = keywords_.find(word);
        auto it_operator_type = operators_.find(word);

        if (it_keywords != keywords_.end()) {
            return {it_keywords->second, word, lineNum_, startCol};
        } else if (it_operator_type != operators_.end()){
            return {Tokentype::Operator, word, lineNum_, startCol, it_operator_type->second};
        }
        else {
            return {Tokentype::Identifier, word, lineNum_, startCol};
        }
    } else if (std::isdigit(c)) {
        auto date = get_date();
        if (!date.empty()){
            return {Tokentype::Date,date,lineNum_, startCol};
        }
        std::string number = get_number();
        return {Tokentype::Number, number, lineNum_, startCol};
    }
    // ====== Punctuation ======
    switch (c) {
        case '(': ++pos_; ++colNum_; return {Tokentype::LeftParen, "(", lineNum_, startCol};
        case ')': ++pos_; ++colNum_; return {Tokentype::RightParen, ")", lineNum_, startCol};
        case ',': ++pos_; ++colNum_; return {Tokentype::Comma, ",", lineNum_, startCol};
        case '{': ++pos_; ++colNum_; return {Tokentype::LeftBrace, "{", lineNum_, startCol};
        case '}': ++pos_; ++colNum_; return {Tokentype::RightBrace, "}", lineNum_, startCol};
        case ':': ++pos_; ++colNum_; return {Tokentype::Colon, ":", lineNum_, startCol};
        case '=': ++pos_; ++colNum_; return {Tokentype::Equals, "=", lineNum_, startCol};
        case '[': ++pos_; ++colNum_; return {Tokentype::LeftSBracket, "[", lineNum_, startCol};
        case ']': ++pos_; ++colNum_; return {Tokentype::RightSBracket, "]", lineNum_, startCol};
    }

    // ====== Unknown character ======
    ++pos_;
    ++colNum_;
    return {Tokentype::End, "", lineNum_, startCol};
}

void Lexer::tokenize() {
    while (true) {
        Token token = next_token();
        tokens_.push_back(token);
        if (token.token_type == Tokentype::End)
            break;
    }
}

const std::vector<Token>& Lexer::get_tokens(){
    return tokens_;
}
