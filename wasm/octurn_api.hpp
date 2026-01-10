#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "Octurn/lexer/Lexer.hpp"
#include "Octurn/parser/Parser.hpp"
#include "Octurn/interpreter/Interpretor.hpp"
#include "Octurn/utils/Utils.hpp"


static emscripten::val Float64Array(const std::vector<double>& vector){

    auto view = emscripten::val(emscripten::typed_memory_view(vector.size(),vector.data()));
    auto arr = emscripten::val::global("Float64Array").new_(vector.size());
    
    arr.call<void>("set", view);

    return arr;

}

static emscripten::val toUint8ArrayCopyFromBoolVec (const std::vector<double>& vector){
    std::vector<uint8_t> tmp;
    tmp.reserve(vector.size());

    for (bool b : vector) tmp.push_back(b ? 1:0);

    auto view = emscripten::val(emscripten::typed_memory_view(tmp.size(),tmp.data()));
    auto arr = emscripten::val::global("Uint8Array").new_(tmp.size());
    
    arr.call<void>("set", view);

    return arr;

}

class Engine {
    public:
        void runLexer(const std::string script);
        void runParser();
        void runInterpreter();
        bool isDone();
        void tick();
        void printVariables();
    private:
        std::vector<Token> tokens_;
        std::shared_ptr<ASTNode> root_;
        std::shared_ptr<Interpreter> interpreter_;

};