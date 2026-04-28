#include "parser.hpp"
#include "dsl/parser/config/configTokens.hpp"
#include "dsl/parser/config/configSetters.hpp"
#include "dsl/parser/config/config.hpp"

parser::parser(const std::vector<Token> tokens):tokens_(std::move(tokens)){}

void parser::parse(){
    for (Token token: tokens_){
        
    }
}

void parser::consumeToken(){
    idx_++;
}

Token parser::currentToken(){
    return tokens_[idx_];
}