#include <iostream>
#include <string>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "interpreter/Interpretor.hpp"
#include "utils/Utils.hpp"
#include <emscripten.h>

static std::unique_ptr<Interpreter> interp;

static void loop(){
    interp->tick();
    if (interp->is_done()){
        std::cout << "\nVariables:\n";
        for (auto& [key, value] : interp->get_variables()) {
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
        for (const auto& [key, value] : interp->get_flags()) {
            std::cout << "  " << key << " = " << value << "\n";
        }

        emscripten_cancel_main_loop();
    }
}



int main() {
    std::cout<<"Begin"<<"\n";
    std::string script = "config { user {max_risk: 5} slippage: true max_drawdown: 10 } data [ { ticker: AAPL timespan: day multiplier: 1 from: 2025-09-01 to: 2025-10-27}, { ticker: MSFT timespan: day multiplier: 1 from: 2025-08-01 to: 2025-10-27} ] strategy SimpleMA { parameters { fast_ma: 5 slow_ma: 30 } indicators {RSI1=RSI(AAPL,12)} entry {when RSI1>50}}";

    Lexer lexer(script);
    lexer.tokenize();

    const auto& tokens = lexer.get_tokens();
    // for (const auto& token : tokens) {
    //     std::cout << "Token(" << to_string(token.token_type)
    //               << ", \"" << token.value << "\")\n";
    // }
    Parser parser(tokens);
    try {
        std::cout << "\nParsed Configuration:\n";
        auto root = parser.parse();

        interp = std::make_unique<Interpreter>(root);
        interp -> run();

        emscripten_set_main_loop(loop, 0, true);

    } catch (const std::exception& e) {
        std::cerr << "Parsing failed: " << e.what() << "\n";
    }
}

