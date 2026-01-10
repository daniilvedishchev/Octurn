#pragma once
#include <vector>
#include <memory>
#include "lexer/Token.hpp"
#include "node/Node.hpp"
#include <unordered_map>
#include <string>


class Parser {
public:
    using BlockHandler = std::function<std::shared_ptr<ASTNode>(Parser&)>;
    using ptrVector = std::vector<std::shared_ptr<ASTNode>>;
    Parser(const std::vector<Token>& tokens);

    // ====================================================== //
    //                    Token logic
    // ====================================================== //

    const Token& current_token() const;
    const Token& peek_next_token();
    bool match(Tokentype type) const;
    void consume_token(Tokentype type);

    // ====================================================== //
    
    //----------------------------------------------------------------------------------------------------------------//

    // ====================================================== //
    //                 Entry/Exit logic
    // ====================================================== //


    std::shared_ptr<ASTNode> parse_comparison();
    std::shared_ptr<ASTNode> parse_expression();
    std::shared_ptr<ASTNode> parse_language_operators();
    std::shared_ptr<ASTNode> parse_term();
    std::shared_ptr<ASTNode> parse_arithmetics();
    std::shared_ptr<ASTNode> parse_factor();
    std::shared_ptr<ASTAction> parse_action();

    // ====================================================== //

    //----------------------------------------------------------------------------------------------------------------//


    // ====================================================== //
    //                  Parse DSL blocks
    // ====================================================== //

    std::shared_ptr<ASTNode> parse_config();
    std::shared_ptr<ASTNode> parse_strategy();
    std::shared_ptr<ASTNode> parse_parameters();
    std::shared_ptr<ASTNode> parse_indicators();
    std::shared_ptr<ASTNode> parse_entry();
    std::shared_ptr<ASTNode> parse_exit();
    std::shared_ptr<ASTNode> parse_block_entry_exit(Tokentype bloctype);
    std::shared_ptr<ASTNode> parse_data();

    std::shared_ptr<ASTNode> parse();

    // ====================================================== //
    
    //----------------------------------------------------------------------------------------------------------------//

    // ====================================================== //
    //                   Utility functions
    // ====================================================== //
    std::shared_ptr<ASTNode> parse_nested_block(Tokentype& block_name);
    std::shared_ptr<ASTNode> parse_function();
    std::shared_ptr<ASTNode> parse_argument();
    std::shared_ptr<Strategy> append_strategy_blocks();
    std::shared_ptr<ASTAssignment> create_assignment_node(const std::string& identifierName);
    std::shared_ptr<ASTBlock> create_block_node(Tokentype block_type, NodeMap&& map);
    void exec_config_parser(std::shared_ptr<ASTRoot>& root);
    void exec_strategy_parser(std::shared_ptr<ASTRoot>& root);


    std::string operator_to_string(OperatorType op);

    // ====================================================== //

    //----------------------------------------------------------------------------------------------------------------//


    // ====================================================== //
    //                      Variables
    // ====================================================== //
    std::unordered_map<std::string, std::string> SpecialTypes;
    // ====================================================== //

    //----------------------------------------------------------------------------------------------------------------//
   

    // ---------------- Updates state of the left member while parsing conditions ---------------- //
    void update_node(std::shared_ptr<ASTNode>& left);
    void parse_key_value(std::string& key, NodeMap& map);
    void throw_error();

    bool required_blocks_in(const std::unordered_set<Tokentype>& types);
    bool is_function_call();
    
    // ===================================== //

    // === Binary node utility function === //

    template <typename T>
    void update_node(std::shared_ptr<ASTNode>& left ,std::function<std::shared_ptr<ASTNode>()> func){

        // ==== Initialize parameters for logical , arithmetical nodes ==== //
        std::string op = current_token().value;
        Origin origin_operator_type = current_token().operator_origin_;
        // ================================================================ //

        consume_token(Tokentype::Operator);
        auto right = func();
        
        // ==== Create node of type T ==== //
        auto node = std::make_shared<T>();

        // ==== Initialize parameters for logical , arithmetical nodes ==== //
        node->op = op;
        node->origin_op = origin_operator_type;
        node->left = left;
        node->right = right;
        left = node;
        // ==== Initialize parameters for logical , arithmetical nodes ==== //
    }

    // ===================================== //
private:
    const std::vector<Token>& tokens_;
    size_t pos_;
    // ============== Scalable ============= //
    std::unordered_map<Tokentype, std::function<std::shared_ptr<ASTNode>()>> block_parsers;
    // ===================================== //

};
