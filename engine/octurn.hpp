#include <string>
#include <vector>
#include "lexer/Token.hpp"
#include "node/Node.hpp"
#include "interpreter/Interpreter.hpp"
#include "parser/Parser.hpp"
#include "lexer/Lexer.hpp"

struct engine {
    Lexer lexer_;
    std::vector<Token> tokens_;
    Parser parser_;
    std::shared_ptr<ASTNode> root_;
    Interpreter interpreter_;

    engine(std::string& api,std::string& script);
};

class octurn{
    private:
        std::string script_;
        engine engine_;
    public:
        octurn(std::string& Script,std::string& API);
        void run();
};
