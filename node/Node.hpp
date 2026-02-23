#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <variant>
#include <map>
#include "lexer/Lexer.hpp"
#include "types/types.hpp"

using Octurn::NodeMap;
using Octurn::AnyValue;
using Octurn::taFunctionCall;
using Octurn::multiValue;

AnyValue compare_vectors_values(AnyValue& left, AnyValue& right, const std::string& op);

struct ExecutionContext {
    std::unordered_map<std::string, AnyValue>& variables;
    std::unordered_map<std::string, AnyValue>& data;
    std::unordered_map<std::string, taFunctionCall>& functionMapper;
};

struct Visitor;

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual AnyValue accept(Visitor& visitor) = 0;
    virtual AnyValue eval_node(std::optional<AnyValue>left,std::optional<AnyValue>right,std::optional<std::shared_ptr<ASTNode>>func_ptr_node);

};

struct ASTList : public ASTNode {
    std::vector<std::shared_ptr<ASTNode>> list;

    AnyValue accept(Visitor& visitor) override;
};

// ==== Declare <Value Node> ==== //
struct ASTValueNode;

// ==== Definition of value node , that could be of type of bool,double,string,map ====//
struct ASTValueNode : ASTNode {
    std::variant<std::string, double, bool, NodeMap> value;

    ASTValueNode(double value_);
    ASTValueNode(bool value_);
    ASTValueNode(std::string value_);
    ASTValueNode(NodeMap map = {});

    ExecutionContext* ctx = nullptr;

    AnyValue accept(Visitor& visitor) override;

    AnyValue eval_node(std::optional<AnyValue>left,std::optional<AnyValue>right,std::optional<std::shared_ptr<ASTNode>>func_ptr_node) override {
        return std::visit([this](auto&& val) -> AnyValue {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, NodeMap>) {
                throw std::runtime_error("Unable to cast NodeMap to AnyValue type.");
            } else {
                if constexpr (std::is_same_v<T, std::string>) {
                    auto& variables_ = ctx->variables;
                    auto it = variables_.find(val);
                    if (it != variables_.end()){
                        if (std::holds_alternative<std::vector<double>>(it->second)) {
                            return std::get<std::vector<double>>(it->second);
                        }
                    }
                }
                return val;
            }
        }, value);
    }

};

// ==== Defines entier blocks (inner content of '{}') in AST structure ==== //
struct ASTBlock : ASTNode{
    NodeMap entries;
    std::optional<Tokentype> block_type;

    AnyValue accept(Visitor& visitor) override;
};


// ==== Defines function call ==== //
struct ASTFunctionCall : ASTNode {
    std::string name;
    std::vector<std::shared_ptr<ASTNode>> expr;
    std::optional<std::shared_ptr<ASTNode>> ptr;

    ExecutionContext* ctx = nullptr;

    AnyValue accept(Visitor& visitor) override;

    AnyValue eval_node(std::optional<AnyValue>left_node_value,std::optional<AnyValue>right_node_value,std::optional<std::shared_ptr<ASTNode>>func_ptr_node) override;

};

// ==== Used to treat conditions ==== //
struct ASTCondition : ASTNode {
    // ==== Left - Right expressions ==== //
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;

    // ==== Operator between left and right expressions and its direction ==== //
    //                  operator direction
    // **** when fast_ma crosses above slow_ma ****    
    std::string op;
    Origin origin_op;

    std::string direction;

    AnyValue accept(Visitor& visitor) override;


};

// ==== Used to treat comparisons ==== //
struct ASTComparison : ASTNode {
    // ==== Left - Right expressions ==== //
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
    std::string op;
    Origin origin_op;

    AnyValue accept(Visitor& visitor) override;

    AnyValue eval_node(std::optional<AnyValue>left_node_value,std::optional<AnyValue>right_node_value,std::optional<std::shared_ptr<ASTFunctionCall>>func_ptr_node){
        if (left_node_value.has_value() && right_node_value.has_value()){
            return compare_vectors_values(left_node_value.value(),right_node_value.value(),op);
        } else throw std::runtime_error("Impossible to evaluate comparison node, no left/right value");
    }

};

// ==== Used to treat terms ==== //
struct ASTTerm : ASTNode {
    // ==== Left , right operands ==== //
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
    std::string op;
    Origin origin_op;

    AnyValue accept(Visitor& visitor) override;

    AnyValue eval_node(std::optional<AnyValue>left_node_value,std::optional<AnyValue>right_node_value,std::optional<std::shared_ptr<ASTFunctionCall>>func_ptr_node){
        if (left_node_value.has_value() && right_node_value.has_value()){
            return compare_vectors_values(left_node_value.value(),right_node_value.value(),op);
        }else throw std::runtime_error("Impossible to evaluate term node, no left/right value");
    };

};

// ==== Used to treat actions ==== //
struct ASTAction : ASTNode {
    ActionType type; 
    std::optional<double> quantity;
    bool all = false;

    AnyValue accept(Visitor& visitor) override;
};

// ==== Crosses operator ==== //
struct ASTCrosses : ASTNode {
    // ==== Left , right operands ==== //
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
    std::string op;
    Origin origin_op;

    AnyValue accept(Visitor& visitor) override;

};

struct ASTExpression : ASTNode {
    // ==== Left - Right terms ==== //
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
    std::string op;
    Origin origin_op;

    AnyValue accept(Visitor& visitor) override;

    AnyValue eval_node(std::optional<AnyValue>left_node_value,std::optional<AnyValue>right_node_value,std::optional<std::shared_ptr<ASTFunctionCall>>func_ptr_node){
        if (left_node_value.has_value() && right_node_value.has_value()){
            return compare_vectors_values(left_node_value.value(),right_node_value.value(),op);
        }
        return {};
    };

};

// ==== Defines logical operators ==== //
struct ASTLogicalCondition : ASTNode {
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
    std::string op;
    Origin origin_op;

    AnyValue accept(Visitor& visitor) override;
};

// ==== Defines assignement operator ==== //
struct ASTAssignment : ASTNode {
    std::string name;
    std::shared_ptr<ASTNode> expr;

    AnyValue accept(Visitor& visitor) override;


};

// ==== Assembels entire strategy structure ==== //
struct Strategy : ASTNode {
    std::string name;
    std::vector<std::shared_ptr<ASTNode>> blocks;

    AnyValue accept(Visitor& visitor) override;
};

struct ASTRoot : public ASTNode {
    std::optional<std::shared_ptr<ASTNode>> config;
    std::shared_ptr<ASTNode> strategy;
    std::shared_ptr<ASTNode> data;

    AnyValue accept(Visitor& visitor) override;

};

struct ASTArithmetics : ASTNode{
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
    std::string op; /// ---------- WANR TOKN HEREEEEE
    Origin origin_op;

    AnyValue accept(Visitor& visitor);

    AnyValue eval_node(std::optional<AnyValue>left_node_value,std::optional<AnyValue>right_node_value){
        if (left_node_value.has_value() && right_node_value.has_value()){
            return compare_vectors_values(left_node_value.value(),right_node_value.value(),op);
        } else throw std::runtime_error("Impossible to evaluate arithmetic node, no left/right value");
    };

};

struct Visitor {
    public:
        Visitor(ExecutionContext& ctx):context(ctx){};

        ExecutionContext& context;

        AnyValue visit(ASTArithmetics& node){
            std::cout<<"Arithmetic node"<<"\n";
            auto left_value = node.left->accept(*this);
            auto right_value = node.right->accept(*this);
            auto node_value = node.eval_node(left_value,right_value);
            return node_value;
        }

        AnyValue visit(ASTValueNode& node){
            std::cout<<"ASTVALUE node"<<"\n";
            node.ctx = &context;
            return node.eval_node(std::nullopt,std::nullopt,std::nullopt);
        }

        AnyValue visit(ASTAction& node){
            std::cout<<"Action node"<<"\n";
            return false;
        }

        AnyValue visit(ASTCrosses& node){
            std::cout<<"Crosses node"<<"\n";
            auto left_value = node.left->accept(*this);
            auto right_value = node.right->accept(*this);
            return node.eval_node(left_value,right_value,std::nullopt);
        }

        AnyValue visit(ASTBlock& node){
            std::cout<<"Block node"<<"\n";
            return false;
        }

        AnyValue visit(ASTList& node){
            std::cout<<"List node"<<"\n";
            return false;
        }

        AnyValue visit(ASTRoot& node){
            std::cout<<"Root node"<<"\n";
            return false;
        }

        AnyValue visit(Strategy& node){
            std::cout<<"Strategy node"<<"\n";
            return false;
        }

        AnyValue visit(ASTAssignment& node){
            std::cout<<"Assignment node"<<"\n";
            return false;
        }

        AnyValue visit(ASTFunctionCall& node){

            std::cout<<"ASTFunctionCall node"<<"\n";
            std::shared_ptr<ASTFunctionCall> fn_ptr;
            node.ctx = &context;

            if (auto call = std::dynamic_pointer_cast<ASTFunctionCall>(node.ptr.value())){
                fn_ptr = call;
            }

            auto value = node.eval_node(std::nullopt,std::nullopt,fn_ptr);
            return value;
        }

        AnyValue visit(ASTLogicalCondition& node){
            std::cout<<"Logical node"<<"\n";
            auto left_value = node.left->accept(*this);
            auto right_value = node.right->accept(*this);
            return node.eval_node(left_value,right_value,std::nullopt);
        }

        AnyValue visit(ASTExpression& node){
            std::cout<<"Expression node"<<"\n";
            auto left_value = node.left->accept(*this);
            auto right_value = node.right->accept(*this);
            return node.eval_node(left_value,right_value,std::nullopt);
        }

        AnyValue visit(ASTCondition& node){
            std::cout<<"Condition node"<<"\n";
            auto left_value = node.left->accept(*this);
            auto right_value = node.right->accept(*this);
            return node.eval_node(left_value,right_value,std::nullopt);
        }

        AnyValue visit(ASTTerm& node){
            std::cout<<"Term node"<<"\n";
            return false;
        }

        AnyValue visit(ASTComparison& node){
            std::cout<<"Comparison node"<<"\n";
            auto left_value = node.left->accept(*this);
            auto right_value = node.right->accept(*this);
            return node.eval_node(left_value,right_value,std::nullopt);
        }
};

// ===== ASTList =====
inline  AnyValue ASTList::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTValueNode =====
inline AnyValue ASTValueNode::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTBlock =====
inline AnyValue ASTBlock::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTCondition =====
inline AnyValue ASTCondition::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTComparison =====
inline AnyValue ASTComparison::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTTerm =====
inline AnyValue ASTTerm::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTAction =====
inline AnyValue ASTAction::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTCrosses =====
inline AnyValue ASTCrosses::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTExpression =====
inline AnyValue ASTExpression::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTLogicalCondition =====
inline AnyValue ASTLogicalCondition::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTAssignment =====
inline AnyValue ASTAssignment::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTFunctionCall =====
inline AnyValue ASTFunctionCall::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== Strategy =====
inline AnyValue Strategy::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTRoot =====
inline AnyValue ASTRoot::accept(Visitor& visitor) {
    return visitor.visit(*this);
}

// ===== ASTArithmetics =====
inline AnyValue ASTArithmetics::accept(Visitor& visitor) {
    return visitor.visit(*this);
}
