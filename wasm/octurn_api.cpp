#include "octurn_api.hpp"

void Engine::runLexer(const std::string script){
    Lexer lexer(script);
    lexer.tokenize();

    tokens_ = lexer.get_tokens();
}

void Engine::runParser(){
    Parser parser(tokens_);
    try {
        root_ = parser.parse();
    }catch (const std::exception& e) {
        std::cerr << "Parsing failed: " << e.what() << "\n";
    }
}

void Engine::runInterpreter(){
    interpreter_ = std::make_unique<Interpreter>(root_);
    interpreter_->run();
}

void Engine::tick(){
    try {
        if (interpreter_) interpreter_->tick();
    } catch (const std::exception& e) {
        std::cerr << "Engine::tick exception: " << e.what() << "\n";
        throw;
    } catch (...) {
        std::cerr << "Engine::tick unknown exception\n";
        throw;
    }
}

bool Engine::isDone(){
    return interpreter_ && interpreter_->is_done();
}

void Engine::printVariables(){
    if (isDone()){
        for (auto& [key, value] : interpreter_->get_variables()) {
            if (std::holds_alternative<std::vector<bool>>(value) || std::holds_alternative<std::vector<double>>(value)){
                auto str =  print_any_value(value);
                std::cout << "  " << key << " = " << str << "\n";
            }
            else {
                if (std::holds_alternative<bool>(value)){
                    auto val = std::get<bool>(value);
                    std::cout << "  " << key << " = " << val << "\n";
                }
                else if (std::holds_alternative<double>(value)){
                    auto val = std::get<double>(value);
                    std::cout << "  " << key << " = " << val << "\n";
                }
                else if (std::holds_alternative<std::string>(value)){
                    auto val = std::get<std::string>(value);
                    std::cout << "  " << key << " = " << val << "\n";
                }
            }
        }
        std::cout << "\n Flags:\n";
        for (const auto& [key, value] : interpreter_->get_flags()) {
            std::cout << "  " << key << " = " << value << "\n";
        }
    }
}

EMSCRIPTEN_BINDINGS(wasm){
    emscripten::class_<Engine>("Engine")
    .constructor<>()
    .function("runLexer",&Engine::runLexer)
    .function("isDone",&Engine::isDone)
    .function("tick",&Engine::tick)
    .function("runParser",&Engine::runParser)
    .function("printVariables",&Engine::printVariables)
    .function("runInterpreter",&Engine::runInterpreter);
}