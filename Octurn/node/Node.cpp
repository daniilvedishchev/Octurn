#include "node/Node.hpp"
#include "lexer/operators.hpp"

using Octurn::AnyValue;
using Octurn::multiValue;


// ASTValueNode constructors
ASTValueNode::ASTValueNode(bool value_) : value(value_) {}

ASTValueNode::ASTValueNode(double value_) : value(value_) {}

ASTValueNode::ASTValueNode(NodeMap value_) : value(value_) {}

ASTValueNode::ASTValueNode(std::string value_) : value(value_) {}


AnyValue compare_vectors_values(AnyValue& left, AnyValue& right, const std::string& op) {
    const auto& functor_variant = OperatorMap.at(op);
    return std::visit([&](auto&& functor) -> AnyValue {
        return std::visit([&](auto&& lhs, auto&& rhs) -> AnyValue {

            using L = std::decay_t<decltype(lhs)>;
            using R = std::decay_t<decltype(rhs)>;

            constexpr bool both_numeric = (std::is_same_v<L,double> || std::is_same_v<L,std::vector<double>>) && 
                                (std::is_same_v<R,double> || std::is_same_v<R,std::vector<double>>);
            constexpr bool both_bool = (std::is_same_v<L,bool> || std::is_same_v<L,std::vector<bool>>) && 
                            (std::is_same_v<R,bool> || std::is_same_v<R,std::vector<bool>>);

            if constexpr (both_numeric || both_bool){
                auto result = vector_op(lhs, rhs, functor);
                return AnyValue{std::move(result)};
            } else{
                throw std::runtime_error("Compare vector function, unsopported types.");
            }
        }, left, right);
    }, functor_variant);
}



AnyValue ASTNode::eval_node(std::optional<AnyValue>,std::optional<AnyValue>,std::optional<std::shared_ptr<ASTNode>>){
    throw std::runtime_error("ASTNode::eval_node not implemented for this node type.");
}

// ====================================================== //
//                     Evaluate function
// - Gets function name and its arguments 
// -> when all arguments are parsed, executes function from map
// - Supports nested functions
// - Returns result of func execution
// ====================================================== //

AnyValue ASTFunctionCall::eval_node(std::optional<AnyValue>left,std::optional<AnyValue>right,std::optional<std::shared_ptr<ASTNode>>func_ptr_node){
    auto func_node = std::dynamic_pointer_cast<ASTFunctionCall>(func_ptr_node.value());
    multiValue args;

    // ==== Loop through every argument in the function call -> treat it ==== //
    for (auto& arg : func_node->expr){
        // ==== If value or bool -> just append to use it ==== //
        if (auto val_node = std::dynamic_pointer_cast<ASTValueNode>(arg)){
            if (std::holds_alternative<double>(val_node->value)){
                args.push_back(std::get<double>(val_node->value));
                }
            else if (std::holds_alternative<bool>(val_node->value)){
                args.push_back(std::get<bool>(val_node->value));
            }
            else if (std::holds_alternative<std::string>(val_node->value)){
                args.push_back(std::get<std::string>(val_node->value));
            }
        }
        // ==== If nested function -> recursively evaluate it ==== //
        else if (auto val_node = std::dynamic_pointer_cast<ASTFunctionCall>(arg)){
            std::cout<<"Function call in a function"<<"\n";

            if (!val_node->ctx){
                val_node->ctx = ctx;
            }

            std::optional<std::shared_ptr<ASTNode>> nested_ptr = val_node;
            args.push_back(val_node->eval_node(std::nullopt,std::nullopt,nested_ptr));
        }
    }

    auto name = func_node->name;
    g_logger.report("Evaluating function: "+name);
    std::cout<<"Looking for a func "+name<<"\n";

    auto it = ctx->functionMapper.find(name);
    auto& variables_ = ctx->variables;
    auto& data_ = ctx->data;

    std::vector<double> output;

    try {
        output = it->second(args, variables_);
    } catch (const std::exception& e) {
        std::cout << "First variable:" << variables_.begin()->first << "\n";
        std::cerr << "TA func " << name << " failed: " << e.what() << "\n";
        throw;
    } catch (...) {
        std::cerr << "TA func " << name << " failed: unknown\n";
        throw;
    }

    return output;

}
