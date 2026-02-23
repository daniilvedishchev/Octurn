#pragma once
#include <functional>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include "node/Node.hpp"
#include "types/types.hpp"
#include "mappers/maps.hpp"
#include "config/configStruc.hpp"
#include "config/cfgTemp.hpp"
#include "src/polygon/polygonDataFeed.hpp"


using Octurn::AnyValue;
using Octurn::multiValue;
using Octurn::taFunctionCall;
using vect_of_vect = std::vector<std::vector<double>>;

class Interpreter {
    public:

        // ============= Constructor + Run + Getters ============ //
        // ===== Constructor takes root pointer to the tree ===== //

        Interpreter(std::shared_ptr<ASTNode>& root,polygonDataFeed& feeder);
        void run();

        // ========== Getters for variables and flags =========== //
        std::unordered_map<std::string,AnyValue>& get_variables();
        std::unordered_map<std::string,bool> get_flags();
        // ====================================================== //

        // ================= Principal evaluators ================= // 
        void eval_program(const std::shared_ptr<ASTRoot>& root);
        void eval_config(const std::shared_ptr<ASTBlock>& block);
        void eval_strategy(const std::shared_ptr<Strategy>& strategy);
        void eval_parameters(const std::shared_ptr<ASTBlock>& block);
        void eval_indicators(const std::shared_ptr<ASTBlock>& block);
        void eval_data(const std::shared_ptr<ASTList>& list);

        AnyValue eval_entry(const std::shared_ptr<ASTBlock>& block);

        // =============== Principal evaluators END =============== // 

        // ================== Optional methods ==================== //
        bool eval_condition(const std::shared_ptr<ASTNode>& node);
        void execute_action(const std::shared_ptr<ASTAction>& action);
        void eval_config_map(const NodeMap& map);
        void build_config(std::unordered_map<std::string, Rule>::iterator& cfgIt,
        std::unordered_map<std::string, Octurn::AnyValue>::iterator& varIt);
        void required_config_parameteters_in();
        // ================ Optional methods END ================== //


        std::unordered_map<Tokentype, std::function<void(const std::shared_ptr<ASTBlock>&)>> strategy_blocks;


    private:
        // ==== Environment to keep variables and flags ==== //
        std::shared_ptr<ASTNode> root_;
        std::unordered_map<std::string,AnyValue> variables_;
        std::unordered_map<std::string,bool> flags_;

        polygonDataFeed feeder_;

        // ==== Variable storage for each block ==== //
        config cfg_;
        std::unordered_map<std::string,AnyValue> parameters_;
        std::unordered_map<std::string, AnyValue> data_;
};
