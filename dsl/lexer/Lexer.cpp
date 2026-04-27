#include "Lexer.hpp"

#include <cctype>

Lexer::Lexer(std::string_view rawTxt) : input_(rawTxt), status_{status::pending}{}

void Lexer::tokenize(){
    tokens.clear();
    position_ = {};
    status_ = status::pending;
    while (status_ == status::pending){
        skipSpaces_();
        newLineCheck_();
        scanToken_();
    }
}

void Lexer::advance_(){
    if (position_.txtIdx < input_.size()){
        position_.txtIdx++;
        position_.column++;
    }
}

char Lexer::currentSymbol_(){
    if (input_.empty() || position_.txtIdx == input_.size()){
        return '\0';
    }
    return input_.at(position_.txtIdx);
}

void Lexer::scanToken_(){
    const auto startLine = position_.row;
    const auto startCol = position_.column;
    std::string word;
    char c = currentSymbol_();
    if (c == '\0') {
        status_ = status::over;
        emit_(tokenType::EndOfFile, "", startLine, startCol);
        return;
    }

    if (c == '"'){
        parseString_(word);
        emit_(tokenType::String, word, startLine, startCol);
        return;
    }

    if (std::isalpha((unsigned char)c) || c == '_') {
        parseWord_(word);
        tokenType type = wordMatcher::matchWordType(word);
        emit_(type, word, startLine, startCol);
        return;
    }

    if (std::isdigit((unsigned char)c)){
        if (isDate_()) {parseDate_(word); emit_(tokenType::Date,word,startLine,startCol); return;}
        if (isTimeframe_()) {parseTimeframe_(word); emit_(tokenType::Timeframe,word,startLine,startCol); return;}
        if (isPercent_()) {parsePercent_(word); emit_(tokenType::Percent,word,startLine,startCol); return;}
        if (isNumber_()) {parseNumber_(word); emit_(tokenType::Number, word, startLine, startCol); return;}
    }

    tokenType op = switcher::matchOpPunctType(c);
    emit_(op, std::string(1, c), startLine, startCol);
    advance_();
}

void Lexer::emit_(tokenType type, const std::string& lexeme, std::size_t line, std::size_t col){
    tokens.emplace_back(Token{.type = type, .lexeme = lexeme, .line = line, .col = col});
}