#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Token.hpp"
#include <unordered_set>

class Lexer {
public:
    // Constructor with direct user input strategy string
    Lexer(const std::string& strategy_text);

    // Main method to tokenize the input
    void tokenize();

    // Get the list of tokens
    const std::vector<Token>& get_tokens();
    // Read the file content into the input buffer
    void read_file_to_string(const std::string& filename);

    // Skip whitespace characters and update line/column counters
    void skip_whitespace();

    // Extract a word or identifier
    std::string get_word_or_symbol();

    // Extract a number (integer or floating-point)
    std::string get_number();

    std::string get_date();

    // Produce the next token from input
    Token next_token();

    // Check next token
    Token peek_next_token();

    // Helper: get the current character (or '\0' if EOF)
    char current_char() const { return pos_ < input_size_ ? input_[pos_] : '\0'; }

    // Advance the current position by count (default 1)
    void advance(size_t count = 1);

    // Check if we've reached the end of input
    bool eof() const { return pos_ >= input_size_; }

    bool is_operator_symbol(char character);

    std::string get_symbol();

private:
    std::string input_;                 // File content as a string
    size_t pos_ = 0;                    // Current index in input
    size_t lineNum_ = 1;                // Current line number
    size_t colNum_ = 1;                 // Current column number
    size_t input_size_ = 0;             // Length of input string
    std::vector<Token> tokens_;         // Output token list

    // Map of keywords (e.g., "strategy", "parameters", "true", "false")
    static const std::unordered_map<std::string, Tokentype> keywords_;
    static const std::unordered_map<std::string, OperatorType> operators_;
};
