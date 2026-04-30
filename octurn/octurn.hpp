#include <string>

#include "dsl/parser/config/config.hpp"
#include "dsl/parser/strategy/strategy.hpp"
#include "dsl/parser/root/parser.hpp"

class octurn {
    private:
        parser parser_;
        strategy strategy_;
    public:
        octurn();
};
