#include <unordered_map>

#include "dsl/token/TokenTypes.hpp"
#include "dsl/token/Token.hpp"
#include "dsl/fields/session/sessionByName.hpp"
#include "dsl/fields/mode/modeByName.hpp"
#include "dsl/fields/slippage/slippageByName.hpp"
#include "dsl/errors/Errors.hpp"

#include "namespace/namespace.hpp"

template <class Error, class Dict, class Field>
auto lookupSetter(const Dict& dictName, Field config::*member){
    return [&dictName, member](Token& token, config& cfg){
        auto it = dictName.find(token.lexeme);
        if (it == dictName.end()){
            throw Error(token);
        }
        cfg.*member = it->second;
    };
}

static const std::unordered_map<tokenType,Octurn::setter> configSetterMap = {
    {tokenType::_session, lookupSetter<SessionError>(sessionByName, &config::session)},
    {tokenType::_mode, lookupSetter<ModeError>(modeByName, &config::mode)},
    {tokenType::_slippage, lookupSetter<SlippageError>(slippageByName, &config::slippage)}
};
