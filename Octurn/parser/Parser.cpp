#include "Parser.hpp"
#include "node/Node.hpp"
#include "utils/Utils.hpp"
#include <stdexcept>
#include <format>
#include <iostream>
#include "log/logHandler.hpp"
#include <string>

std::string Parser::operator_to_string(OperatorType op) {
    switch (op) {
        case OperatorType::And: return "and";
        case OperatorType::Or: return "or";
        default: return "unknown";
    }
}

// ===== Init Parser Class + Handler to avoid explicit if - else problems =====
Parser::Parser(const std::vector<Token>& tokens)
        : tokens_(tokens), pos_(0) 
    {
        // ==== Scalable maps ==== //
        block_parsers = {
            {Tokentype::Parameters, [this](){ return parse_parameters(); }},
            {Tokentype::Config, [this](){ return parse_config(); }},
            {Tokentype::Strategy, [this](){ return parse_strategy(); }},
            {Tokentype::Indicators, [this](){ return parse_indicators(); }},
            {Tokentype::Data,      [this](){ return parse_data(); }},
            {Tokentype::Entry,      [this](){ return parse_entry(); }},
            {Tokentype::Exit,       [this](){ return parse_exit(); }}
        };

        SpecialTypes = {
            {"ticker", "string"},
            {"exchange", "string"},
            {"symbol", "string"},
            {"timespan", "string"},
            {"from", "string"},
            {"to", "string"}
        };

    }


const Token& Parser::current_token() const {
    return tokens_[pos_];
}

const Token& Parser::peek_next_token(){
    if (pos_ + 1 < tokens_.size()) {
        return tokens_[pos_ + 1];
    }
    throw std::runtime_error("Unexpected end of token stream");
}

bool Parser::match(Tokentype type) const {
    return current_token().token_type == type;
}


void Parser::consume_token(Tokentype type) {
    std::cout<<std::format("Consumed+{}+{}",to_string(type),current_token().value)<<"\n";
    if (pos_ < tokens_.size() && match(type)) {
        ++pos_;
    } else {
        throw_error();
    }
}

// =============================================================== //
//              Accepts Numbers/Booleans/Strings
//              For the structures : {key: value} 
//     ---- Strings are only accepted for some keywords ----
// =============================================================== //
void Parser::parse_key_value(std::string& key, NodeMap& map) {
    auto assign_value = [&](auto value) {
        map[key] = std::make_shared<ASTValueNode>(value);
    };
    if (match(Tokentype::Number)) {
        assign_value(std::stod(current_token().value));
        consume_token(Tokentype::Number);
    } else if (match(Tokentype::True) || match(Tokentype::False)) {
        assign_value(str_to_bool(current_token().value));
        consume_token(current_token().token_type);
    } else if (match(Tokentype::Identifier) && SpecialTypes.contains(key)){
        std::cout<<std::format("key (special):{}->{}",key,current_token().value)<<"\n";

        assign_value(current_token().value);
        consume_token(Tokentype::Identifier);
    }else if (match(Tokentype::Date)){
        assign_value(current_token().value);
        consume_token(Tokentype::Date);}
    else {
        throw_error();
    }
}
// =============================================================== //


std::shared_ptr<ASTAssignment> Parser::create_assignment_node(const std::string& identifierName) {
    
    // ========= Assignment operation ============ //
    // Parse the function
    auto function = parse_function();
    consume_token(Tokentype::Identifier);

    // ==== Create assignement node ==== //
    auto assignment = std::make_shared<ASTAssignment>();
    assignment->name = identifierName;
    assignment->expr = function;

    return assignment;
}

std::shared_ptr<ASTBlock> Parser::create_block_node(Tokentype block_type, NodeMap&& map) {
    auto block = std::make_shared<ASTBlock>();
    block->block_type = block_type;
    block->entries = std::move(map);
    return block;
}

std::shared_ptr<ASTNode> Parser::parse_nested_block(Tokentype& block_type){
    g_logger.report("Starting to parse a nested block...");
    NodeMap map;

    while (match(Tokentype::Identifier)){

        std::string IdentifierName = current_token().value;
        g_logger.report("Parsing key: " + IdentifierName);

        consume_token(Tokentype::Identifier);
        
        if (match(Tokentype::Colon)){
            consume_token(Tokentype::Colon);
            parse_key_value(IdentifierName,map);
        }
        else if (match(Tokentype::LeftBrace)){
            consume_token(Tokentype::LeftBrace);
            map[IdentifierName] = parse_nested_block(block_type);
        }
        else if (match(Tokentype::Equals)){
            consume_token(Tokentype::Equals);
            if(match(Tokentype::Identifier)) {
                auto assignment_node = create_assignment_node(IdentifierName);
                map[IdentifierName] = std::static_pointer_cast<ASTNode>(assignment_node);
            }
        }
        else{
            g_logger.report("Error: Unexpected token while parsing key: " + IdentifierName);
            throw_error();
        }
    }

    auto block = create_block_node(block_type,std::move(map));
    consume_token(Tokentype::RightBrace);

    return block;
}

std::shared_ptr<ASTNode> Parser::parse_config(){
    g_logger.report(std::string("Config parser is started."));
    if (match(Tokentype::Config)){
        // ==== Specify block type ==== //
        auto type = current_token().token_type;

        // ==== Keep parsing , eat 'config' keyword and '{' ==== //
        consume_token(Tokentype::Config);
        consume_token(Tokentype::LeftBrace);

        auto block = parse_nested_block(type);
        

        return block;
    }

    throw std::runtime_error("Missing 'config' block.");

}

void Parser::throw_error(){
    throw std::runtime_error(std::format(
        "Unexpected token '{}' at line {}, col {}",
        current_token().value,
        current_token().lineNum,
        current_token().colNum
    ));
}

std::shared_ptr<ASTNode> Parser::parse_argument(){
    if (is_function_call()) {
        return parse_function();
    }
    else if (match(Tokentype::Identifier)) {
        std::string varName = current_token().value;
        consume_token(Tokentype::Identifier);
        return std::make_shared<ASTValueNode>(varName);
    }
    else if (match(Tokentype::Number)) {
        double number = std::stod(current_token().value);
        consume_token(Tokentype::Number);
        return std::make_shared<ASTValueNode>(number);
    }
    else if (match(Tokentype::True) || match(Tokentype::False)) {
        bool boolean_val = str_to_bool(current_token().value);
        consume_token(current_token().token_type);
        return std::make_shared<ASTValueNode>(boolean_val);
    }
    throw_error();

    return nullptr;
}

std::shared_ptr<ASTNode> Parser::parse_parameters(){
    if (!match(Tokentype::Parameters)){
        throw std::runtime_error("Can't find parameters block.");
    }

    consume_token(Tokentype::Parameters);
    consume_token(Tokentype::LeftBrace);

    Tokentype type = Tokentype::Parameters;
    auto block = parse_nested_block(type);

    return block;
}

// ==== if Identifier + '(' -> function call ==== //
bool Parser::is_function_call() {
    return current_token().token_type == Tokentype::Identifier &&
           peek_next_token().token_type == Tokentype::LeftParen;
}


std::shared_ptr<ASTNode> Parser::parse_function() {

    // ==== Function call is created for every present function ==== //
    auto current_func_call = std::make_shared<ASTFunctionCall>();

    // ==== Function name ==== //
    std::string name = current_token().value;
    consume_token(Tokentype::Identifier);
    current_func_call->name = name;

    // ==== Opening parenthesis ==== //
    consume_token(Tokentype::LeftParen);

    while (!match(Tokentype::RightParen)) {
        
        // ==== Parse function parameters ==== //
        current_func_call->expr.push_back(parse_argument());

        // Consume comma if present
        if (match(Tokentype::Comma)) {
            consume_token(Tokentype::Comma);
            // ==== Case when MSI(12,20,) ==== //
            // ==== Presence of ',' without passing an argument ==== //
            if (match(Tokentype::RightParen)){
                throw std::runtime_error("Trailling comma in function call");
            }
        }
    }

    // Closing parenthesis
    consume_token(Tokentype::RightParen);
    current_func_call->ptr = current_func_call;
    return current_func_call;
}

std::shared_ptr<ASTNode> Parser::parse_indicators(){

    auto indicator_block = std::make_shared<ASTBlock>();
    indicator_block->block_type = Tokentype::Indicators;

    if (!match(Tokentype::Indicators)){
        throw std::runtime_error(std::format("'Indicators' blocktype not found."));
    }

    // ====== Consume Indicator keyword and block beginning ====== //
    consume_token(Tokentype::Indicators);
    consume_token(Tokentype::LeftBrace);

    // ====== Keep looping while there are existing variables ====== //
    while (match(Tokentype::Identifier)){

        // ====== Keep track of all variables (identifiers) of type ASTAssignment ====== //
        auto assignment_operator = std::make_shared<ASTAssignment>();
        assignment_operator->name = current_token().value;;
        consume_token(Tokentype::Identifier);

        // ====== Checks if syntax structure is correct '=' required after assignement ====== //
        // ====== Consume to parse ====== //
        consume_token(Tokentype::Equals);

        // ====== Parses function and its arguments , tranforms into AST type object ====== //
        assignment_operator->expr = parse_function();
        indicator_block->entries[assignment_operator->name] = assignment_operator;
    }

    // ==== Close 'indicators' block ==== //
    g_logger.report("Indicator block is successfully parsed!");
    consume_token(Tokentype::RightBrace);

    return indicator_block;
}

std::shared_ptr<ASTNode> Parser::parse_comparison(){
    auto left = parse_arithmetics();

    g_logger.report("Parsing Comparisons");

    while (current_token().operator_type.has_value() &&
       (current_token().operator_type == OperatorType::Less ||
        current_token().operator_type == OperatorType::Greater)) {

        update_node<ASTComparison>(left, [&]() {
            return parse_arithmetics();
        });
    }
    return left;
}

std::shared_ptr<ASTNode> Parser::parse_arithmetics(){
    auto left = parse_factor();
    g_logger.report("Parsing Arithmetics");

    while (current_token().operator_type.has_value() &&
        (current_token().operator_type == OperatorType::Plus ||
            current_token().operator_type == OperatorType::Minus ||
            current_token().operator_type == OperatorType::Multiply ||
            current_token().operator_type == OperatorType::Divide)) {
                
            std::cout<<"Currently parsing arithmetics"<<"\n";
            update_node<ASTArithmetics>(left, [&]() {
                return parse_factor();
            });
    }
    return left;
}


std::shared_ptr<ASTNode> Parser::parse_expression(){
    auto left = parse_term();
    g_logger.report("Parsing Expressions");

    while (current_token().operator_type.has_value() &&
       (current_token().operator_type == OperatorType::Or)) {

        update_node<ASTExpression>(left, [&]() {
            return parse_term();
        });
    }
    return left;
}

std::shared_ptr<ASTNode> Parser::parse_term(){

    auto left = parse_comparison();
    g_logger.report("Parsing Terms");

    while (current_token().operator_type.has_value() &&
       (current_token().operator_type == OperatorType::And)) {

        update_node<ASTExpression>(left, [&]() {
            return parse_comparison();
        });
    }

    return left;

}

std::shared_ptr<ASTNode> Parser::parse_factor() {
    g_logger.report("Parsing Factors");

    if (is_function_call()) {
        return parse_function();
    }
    else if (current_token().token_type == Tokentype::Number) {
        auto node = std::make_shared<ASTValueNode>(std::stod(current_token().value));
        consume_token(Tokentype::Number);
        return node;
    }
    else if (match(Tokentype::LeftParen)) {
        consume_token(Tokentype::LeftParen);
        auto node = parse_expression();
        consume_token(Tokentype::RightParen);
        return node;
    }
    else if (match(Tokentype::Identifier)) {
        auto node = std::make_shared<ASTValueNode>(current_token().value); 
        consume_token(Tokentype::Identifier);
        return node;
    }

    throw_error();
    return nullptr;
}

// std::shared_ptr<ASTNode> Parser::parse_language_operators(){
//     auto left = parse_expression();

//     while (current_token().operator_type.has_value()){
//         if (current_token().operator_type == OperatorType::CrossesAbove || current_token().operator_type == OperatorType::CrossesBelow){
//             update_node<ASTCrosses>(left);
//         }
//     }

//     return left;
// }


// ==== Parses actions when condition is met ==== //
std::shared_ptr<ASTAction> Parser::parse_action() {
    auto action = std::make_shared<ASTAction>();

    if (match(Tokentype::Buy)) {
        action->type = ActionType::Buy;
        consume_token(Tokentype::Buy);
    
        if (match(Tokentype::All)) {
            action->all = true;
            consume_token(Tokentype::All);
        } else {
            action->quantity = std::stod(current_token().value);
            consume_token(Tokentype::Number);
        }
    }
    else if (match(Tokentype::Sell)) {
        action->type = ActionType::Sell;
        consume_token(Tokentype::Sell);

        if (match(Tokentype::All)) {
            action->all = true;
            consume_token(Tokentype::All);
        } else {
            action->quantity = std::stod(current_token().value);
            consume_token(Tokentype::Number);
        }
    }

    return action;

}

// ==== Complete 'strategy' block ==== //
std::shared_ptr<Strategy> Parser::append_strategy_blocks() {

    // ==== Node <Strategy> ==== //
    // ** Containts strategy name and entries (vector of nodes) ** // 
    auto strategy = std::make_shared<Strategy>();

    // ==== Seen tokentypes , avoids block repitition, safety check ==== //
    std::unordered_set<Tokentype> seen_types;

    while (!match(Tokentype::RightBrace)) {

        // ==== Finds an iterator to existing tokentype, provinig us with function call ==== // 
        auto it = block_parsers.find(current_token().token_type);
       
        if (it != block_parsers.end() && !seen_types.count(current_token().token_type)) {

            // ==== Get current token type and call block parser ==== //
            auto type = current_token().token_type;
            auto block_node = it->second();

            // ==== Pushes block into 'strategy' tree and marks block as seen ==== //
            strategy->blocks.push_back(block_node);
            g_logger.report(std::format("Successfully parsed {} in 'Strategy' section" ,to_string(type)));
            seen_types.insert(type);

        } else {
            throw_error();
        }
    }
    for (auto& block :seen_types){
        std::cout<<to_string(block)<<"\n";
    }

    // Ensure that all required blocks exist
    required_blocks_in(seen_types);

    return strategy;
}


// ==== Verifies if all required blocks are written by the user ==== //
bool Parser::required_blocks_in(const std::unordered_set<Tokentype>& types){
    // std::vector<Tokentype> to_find {Tokentype::Entry, Tokentype::Exit};
    std::vector<Tokentype> to_find {Tokentype::Parameters}; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //
    bool authorized_exec = true;
    for (auto block : to_find) {
        if (!types.count(block)) {
            authorized_exec = false;
            std::cout<<"error is here"<<"\n";
            throw std::runtime_error(std::format("Missing {} block.", to_string(block)));
        }
    }

    return authorized_exec;
}

// ====================================================== //
//            Parse Data
// - Fetches tickers chosen by user -> puts in variable section
// - Enclosed by '[]'
// - Returns list of nodemaps, since 
// ====================================================== //

std::shared_ptr<ASTNode> Parser::parse_data(){
    if (!match(Tokentype::Data)){
        throw std::runtime_error("Can't find data block. No data to make calculations on.");
    }

    auto list = std::make_shared<ASTList>();

    consume_token(Tokentype::Data);
    consume_token(Tokentype::LeftSBracket);

    Tokentype type = Tokentype::Data;
    while (!match(Tokentype::RightSBracket)){
        consume_token(Tokentype::LeftBrace);
        auto block = parse_nested_block(type);
        list->list.push_back(block);
        if (match(Tokentype::Comma)){
            consume_token(Tokentype::Comma);
        }
    }
   
    consume_token(Tokentype::RightSBracket);

    return list;
}

// ==== Utility function to parse entry/exit ==== //
std::shared_ptr<ASTNode> Parser::parse_block_entry_exit(Tokentype blockType) {
    auto block = std::make_shared<ASTBlock>();
    g_logger.report("Parsing entry/exit");

    if (match(blockType)) {
        consume_token(blockType);
        consume_token(Tokentype::LeftBrace);
        // ==== If 'when' is defined -> parse condition ==== //
        if (match(Tokentype::When)) {

            consume_token(Tokentype::When);
            std::cout<<"Started parsing condition ate 'When'"<<"\n";
            block->block_type = Tokentype::Entry;
            block->entries["Conditions"] = parse_expression();
            g_logger.report("Conditions are parsed!");

            // consume_token(Tokentype::LeftBrace);
            // block->entries["Action"] = parse_action();
            // consume_token(Tokentype::RightBrace);

        } else {
            throw_error();
        }

        consume_token(Tokentype::RightBrace);
    }

    return block;
}

// ==== Parse exit block ==== //
std::shared_ptr<ASTNode> Parser::parse_entry() {
    return parse_block_entry_exit(Tokentype::Entry);
}

// ==== Parse entry block ==== //
std::shared_ptr<ASTNode> Parser::parse_exit() {
    return parse_block_entry_exit(Tokentype::Exit);
}

// ==== Build entier 'strategy' block ==== //
std::shared_ptr<ASTNode> Parser::parse_strategy(){

    if (!match(Tokentype::Strategy)){
        throw std::runtime_error("Missing 'strategy' block.");
    }

    // ==== Verify if 'strategy' keyword exist, extract strategy name ==== //
    consume_token(Tokentype::Strategy);

    if (!match(Tokentype::Identifier)){
        throw std::runtime_error("Missing strategy name.");
    }

    // ==== Saves strategy name ==== //
    std::string name = current_token().value; 
    consume_token(Tokentype::Identifier);
    
    consume_token(Tokentype::LeftBrace);

    // ==== Builds strategy tree ==== //
    auto strategy = append_strategy_blocks();
    strategy->name = name;

    // ==== End parsing ==== //
    consume_token(Tokentype::RightBrace);

    return strategy;
}

void Parser::exec_config_parser(std::shared_ptr<ASTRoot>& root){
    g_logger.report("Config keyword found.");
    if (root->config) {
        throw std::runtime_error("Duplicate 'config' block.");
    }
    root->config = parse_config();
    g_logger.report("Config is successfully parsed.");
    if (root->config.has_value() && root->config.value()) {
        g_logger.report("Config block exists.");
    } else {
        g_logger.report("No config block found.");
    }
}

void Parser::exec_strategy_parser(std::shared_ptr<ASTRoot>& root){
    g_logger.report("Started parsing strategy block.");
    if (root->strategy){
        throw std::runtime_error("Duplicate 'strategy' block.");
    }
    root->strategy = parse_strategy();
    consume_token(Tokentype::End);
    if (root->strategy){
        g_logger.report("Strategy block parsed successfully.");
    }
    else{
        throw std::runtime_error("Missing 'strategy' block.");
    }
}

// ==== Organizes all block parsers in one function ==== //
std::shared_ptr<ASTNode> Parser::parse() {
    g_logger.report("Started parsing.");
    auto root = std::make_shared<ASTRoot>();

    // ==== Parse DSL in not order dependent manner ==== //
    auto it = block_parsers.find(current_token().token_type);
    
    while (pos_ < tokens_.size()) {
        std::cout<<current_token().value<<"\n";
        if (match(Tokentype::Config)){
            exec_config_parser(root);
        }else if (match(Tokentype::Data)){
            g_logger.report("Data keyword found.");
            root->data = parse_data();
        }
        else if (match(Tokentype::Strategy)){
            exec_strategy_parser(root);
        }
    }

    return root;

}