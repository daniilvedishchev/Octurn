#include <vector>
#include <unordered_set>

#include "namespace/namespace.hpp"
#include "dsl/token/Token.hpp"
#include "dsl/parser/strategy/strategy.hpp"
#include "dsl/parser/config/config.hpp"
class parser {
    private:
        std::unique_ptr<strategy> root_;
        std::vector<Token> tokens_;
        std::unordered_set<tokenType> seen_;
        size_t idx_;
        config cfg_;
    public:
        parser(const std::vector<Token> tokens);
        void parse();
        void consumeToken();
        Token currentToken();
};
