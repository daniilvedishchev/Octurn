#include "octurn.hpp"
#include "utils/Utils.hpp"

engine::engine(std::string& api,std::string& script)
    : lexer_(script),
      tokens_(lexer_.get_tokens()),
      parser_(tokens_),
      client_(api),
      feeder_(std::move(client_)),
      root_(parser_.parse()),
      interpreter_(root_,feeder_) {}

octurn::octurn(std::string& Script,std::string& API)
    : script_(std::move(Script)),
      engine_(API,script_) {}

void octurn::run(){
    try {
        engine_.interpreter_.run();
        printVariables(engine_.interpreter_);
    } catch (const std::exception& e){
        std::cerr<<"Interpretation failed: "<< e.what()<<"\n";
    }
}
