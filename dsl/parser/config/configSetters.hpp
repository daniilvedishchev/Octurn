#include <unordered_map>
#include "dsl/token/TokenTypes.hpp"
#include "dsl/token/Token.hpp"
#include "namespace/namespace.hpp"
#include "dsl/fields/session/sessionByName.hpp"
#include "dsl/fields/mode/modeByName.hpp"
#include "dsl/errors/Errors.hpp"

static const std::unordered_map<tokenType,Octurn::setter> configSetterMap = {
    {tokenType::_session, [](Token& token, config& cfg){
        auto it = sessionByName.find(token.lexeme);
        if (!(it != sessionByName.end())){
            throw SessionError(token);
        }
        cfg.session = it->second;
    }},
    {tokenType::_mode, [](Token& token, config& cfg){
        auto it = modeByName.find(token.lexeme);
        if (!(it != modeByName.end())){
            throw ModeError(token);
        }
        cfg.mode = it->second;
    }}
}