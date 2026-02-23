#include "Interpretor.hpp"
#include "utils/Utils.hpp"
#include "log/logHandler.hpp"
#include <iostream>
#include <numeric>
#include <string>
#include <regex>


#define OHLC_SIZE 4

// ====================================================== //
//            Interpreter class constructor
// - Initializes strategy block map
// - Initializes function map -> exec function written by user
// ====================================================== //

// ====================================================== //
Interpreter::Interpreter(std::shared_ptr<ASTNode>& root,polygonDataFeed& feeder)
    : root_(std::move(root)),feeder_(std::move(feeder))
{
    strategy_blocks = {
        {Tokentype::Config, [this](const std::shared_ptr<ASTBlock>& block){
            this->eval_config(block);
        }},
        { Tokentype::Parameters, [this](const std::shared_ptr<ASTBlock>& block) {
            this->eval_parameters(block);
        }},
        { Tokentype::Indicators, [this](const std::shared_ptr<ASTBlock>& block) {
            this->eval_indicators(block);
        }},
        { Tokentype::Entry, [this](const std::shared_ptr<ASTBlock>& block) {
            this->eval_entry(block);
        }}
    };
}
// ------------------------------------------------------------------------------------------------------------------- //

// ====================================================== //
//                    Evaluate Data
// @brief Takes data block and retrieves OHLC data,then putting it in variable map
// ====================================================== //

void Interpreter::eval_data(const std::shared_ptr<ASTList>& list){
    if (auto list_node = std::dynamic_pointer_cast<ASTList>(list)){
        for (auto& data_block : list_node->list){
            std::string ticker,timespan,from,to;
            int multiplier;
            if (auto fetching_params_node = std::dynamic_pointer_cast<ASTBlock>(data_block)){
                auto fetching_params = fetching_params_node->entries;
                for (auto& [key,value]:fetching_params){
                    if (auto value_node = std::dynamic_pointer_cast<ASTValueNode>(value)){
                        if (std::holds_alternative<std::string>(value_node->value)){
                            std::string strVal = std::get<std::string>(value_node->value);
                            if (key == "ticker") ticker = strVal;
                            else if (key == "from") from = strVal;
                            else if (key == "to") to = strVal;
                            else if (key == "timespan") timespan = strVal;
                        } else if (std::holds_alternative<double>(value_node->value)) {
                        if (key == "multiplier") multiplier = static_cast<int>(std::get<double>(value_node->value));}
                    }
                }
            }
            if (ticker.empty()){
                throw std::runtime_error("No ticker found");
            }

            feeder_.loadBars(ticker,multiplier,from,to,timespan);
        }
    }
}

AnyValue Interpreter::eval_entry(const std::shared_ptr<ASTBlock>& block){
    g_logger.report("Interpretation of entry block has started.");
    if (block->block_type == Tokentype::Entry){

        // ==== Start to interpret conditions ==== //
        auto condition = std::dynamic_pointer_cast<ASTNode>(block->entries["Conditions"]);

        // ==== Create visitor to get callable type ==== //
        ExecutionContext ctx{variables_, data_, functionMap};
        Visitor visitor(ctx);

        // ==== Recursive propagation through all childs ==== //
        auto evaluated_expression = condition->accept(visitor);
        variables_["Entry"] = evaluated_expression;

        return evaluated_expression;
    } else throw std::runtime_error("\"Entry\" block is not defined!");
    
}

// ====================================================== //

// ------------------------------------------------------------------------------------------------------------------- //


// ====================================================== //

// ------------------------------------------------------------------------------------------------------------------- //

// ====================================================== //
//                     Interpreter
// - Runs DSL Interpreter from tree root
// ====================================================== //
void Interpreter::run(){

    // ==== Cast ASTNode object to ASTRoot ==== //
    auto root_cast = std::dynamic_pointer_cast<ASTRoot>(root_);

    // ==== If invalid ==== //
    if (!root_cast){
        g_logger.report("Tree root is not recognizable.");
    }

    // ==== Evaluate script if cast is successful ==== //
    g_logger.report("Interpretation has started.");
    eval_program(root_cast);
}
// ====================================================== //

// ------------------------------------------------------------------------------------------------------------------- //

// ====================================================== //

// ------------------------------------------------------------------------------------------------------------------- //

// ====================================================== //
//                   Evaluate Indicators
// - Loops through all assignments
// - key -> function interpretor (appends to variables_) 
// ====================================================== //

void Interpreter::eval_indicators(const std::shared_ptr<ASTBlock>& block){
    g_logger.report(std::string("Indicator interpreter is launched."));

    // ==== Evaluates indicator section and expands indicator section ==== //
    ExecutionContext ctx{variables_, data_, functionMap};
    for (auto& [key,assignment] : block->entries){
        g_logger.report(std::string("Indicator found: " + key));

        // ==== Loop through all key-value pair ==== //
        if (auto assign_node = std::dynamic_pointer_cast<ASTAssignment>(assignment)){
            // ==== If corresponding value is function call ==== //
            std::cout<<key<<"\n";
            auto function_node = std::dynamic_pointer_cast<ASTFunctionCall>(assign_node->expr);
            if (function_node){
                function_node->ctx = &ctx;
                std::optional<std::shared_ptr<ASTNode>> func_ptr = function_node;
                variables_[key] = function_node->eval_node(std::nullopt,std::nullopt,func_ptr);
            }
        }
    }
    std::cout<<"All indicators are evaluated"<<"\n";
    return ;
}
// ====================================================== //

// ------------------------------------------------------------------------------------------------------------------- //

// ====================================================== //
//                   Evaluate Program
// - Child of .run()
// - Responsable for evaluation of all parts of DSL
// -> "Config" + "Strategy" blocks
// ====================================================== //

void Interpreter::eval_program(const std::shared_ptr<ASTRoot>& root){

    // ==== Cast general purpose objects into their specific substructs ==== //
    if (root->config.has_value()) {
        auto config_block = std::dynamic_pointer_cast<ASTBlock>(root->config.value());
        eval_config(config_block);
    }

    if (root->data) {
        auto data_block = std::dynamic_pointer_cast<ASTList>(root->data);
        eval_data(data_block);
    }
};
// ====================================================== //

// ------------------------------------------------------------------------------------------------------------------- //

// ====================================================== //
//                   Evaluate Straregy
// - Takes an Strategy as an input
// - Loops through internal blocks
// - Calls specified functions related to these blocks
// ====================================================== //

void Interpreter::eval_strategy(const std::shared_ptr<Strategy>& strategy){

    // ==== Interprets all internal blocks in strategy section ==== //
    // ==== Entry/Exit -> are necessary , others : indicators, parameters are optional ==== // 

    for (auto& block : strategy->blocks){
        if (auto block_node = std::dynamic_pointer_cast<ASTBlock>(block)){
            auto type = block_node->block_type.value();
            auto it = strategy_blocks.find(type);
            std::cout << "Interpretation f " << to_string(type) << std::endl;
            std::cout << "sdqsdsqd of " << to_string(type) << std::endl;
            if (it!=strategy_blocks.end()){
                it->second(block_node);
            }
        }
    }
}
// ====================================================== //

// ------------------------------------------------------------------------------------------------------------------- //

// ====================================================== //
//                   Evaluate Parameters
// - Parameter block -> key-value pairs (Nodemap)
// - If value is string, check for key
// -> for key "ticker.*" retrives related data via API 
// ====================================================== //

void Interpreter::eval_parameters(const std::shared_ptr<ASTBlock>& block){
    
    // ====================================== //
    // If value is double -> put this in variable section
    // If value is bool -> put this in flag section
    // ====================================== //

    // ==== Evaluates parameter section and expands variable section ==== //
    for (auto& [key,value] : block->entries){
        // ==== Loop through all key-value pair ==== //
        if (auto node_value = std::dynamic_pointer_cast<ASTValueNode>(value)){
            // ==== If corresponding value is double ==== //
            if (std::holds_alternative<double>(node_value->value)) {
                variables_[key] = std::get<double>(node_value->value);
            }
            // ==== If corresponding value is boolean ==== //
            else if (std::holds_alternative<bool>(node_value->value)) {
                flags_[key] = std::get<bool>(node_value->value);
            }
        }
    }
}
// ====================================================== //

// ------------------------------------------------------------------------------------------------------------------- //

// ====================================================== //
//                      Getters
// ====================================================== //

std::unordered_map<std::string,AnyValue>& Interpreter::get_variables() {
    return variables_;
}

std::unordered_map<std::string,bool> Interpreter::get_flags(){
    return flags_;
}
// ====================================================== //

// ------------------------------------------------------------------------------------------------------------------- //

// ====================================================== //
//                   Evaluate Config Map
// - Helper function for evaluate parameters
// ====================================================== //
void Interpreter::eval_config_map(const NodeMap& map){

    // ====================================== //
    // If value is double -> put this in variable section
    // If value is bool -> put this in flag section
    // ====================================== //

    // Value in map could contain children map so it's treated as block //
    for (auto& [key, node_value] : map) {

        if (auto value = std::dynamic_pointer_cast<ASTValueNode>(node_value)) {

            if (std::holds_alternative<double>(value->value)) {

                variables_[key] = std::get<double>(value->value);
            }
            else if (std::holds_alternative<bool>(value->value)) {
                flags_[key] = std::get<bool>(value->value);
            }
            else if (std::holds_alternative<NodeMap>(value->value)) {
                eval_config_map(std::get<NodeMap>(value->value));
            }

        } else if (auto block = std::dynamic_pointer_cast<ASTBlock>(node_value)) {
            eval_config_map(block->entries);
        }
    }
}
// ALL OTHER VARIABLES NOT MEETING CONDITIONS FOR CONFIG BLOCK WILL BE DELETED!!!!!
// PS make variables section for each block!!!!
void Interpreter::build_config(std::unordered_map<std::string, Rule>::iterator& cfgIt,
std::unordered_map<std::string, Octurn::AnyValue>::iterator& varIt){
    std::string error;
    bool validated {cfgIt->second.validate(varIt->second,cfg_,error)};
    if (!validated){
        std::runtime_error(std::format("Unable to build config, error occured: {}",error));
    }
}

void Interpreter::required_config_parameteters_in(){
    for (auto cfgIt = cfgTemplate.begin(); cfgIt != cfgTemplate.end();cfgIt++){
        auto varIt = variables_.find(cfgIt->first);
        // if required and not in the variable section -> throw
        if (cfgIt->second.required == true && varIt == variables_.end()){
            std::runtime_error(std::format("Required config parameter \"{}\" is missing",cfgIt->first));
            std::cout << std::format("Missing config parameter {}\n",cfgIt->first);
        } else if (cfgIt->second.required == false && varIt == variables_.end()){
            continue;
        } else build_config(cfgIt,varIt);
    }
}

void Interpreter::eval_config(const std::shared_ptr<ASTBlock>& block){
    eval_config_map(block->entries);
    required_config_parameteters_in();
}
