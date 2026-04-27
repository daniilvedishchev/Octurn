#pragma once
#include <variant>
#include <functional>

#include "dsl/fields/execution/execution.hpp"
#include "dsl/fields/session/session.hpp"
#include "dsl/fields/mode/mode.hpp"
#include "dsl/validator/ctx/ValidationCtx.hpp"
#include "dsl/token/Token.hpp"
#include "dsl/parser/config/config.hpp"

namespace Octurn {
    using value = std::optional<std::variant<std::string,execution,session,mode>>;
    using validationFn = std::function<bool()>;
    using percent = double;
    using setter = std::function<void(Token& token, config& cfg)>;
}