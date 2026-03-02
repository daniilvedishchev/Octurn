#include "BacktestCtrl.h"
#include "Parser.hpp"
#include "Lexer.hpp"
#include <drogon/HttpAppFramework.h>
#include <json/json.h>
#include <memory>

void BacktestCtrl::asyncHandleHttpRequest(const drogon::HttpRequestPtr& req, std::function<void (const drogon::HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();

    // ==== If request is incorrect ====//
    if(!json){
        
        // ==== Create response pointer ==== //
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Invalid JSON");

        // ==== Send response ==== //
        callback(resp);
        return;
    }

    std::string strategyText = (*json)["source_code"].asString();

    try {

        // ==== Lexer init + tokenization ==== //
        Lexer lexer(strategyText);
        lexer.tokenize();
        std::vector<Token> tokens_ = lexer.get_tokens();

        // ==== Create parser class object ==== //
        Parser parser(tokens_);

        // ==== Parsed AST tree ==== //
        auto ast = parser.parse_strategy(); // возвращает AST дерева стратегии

        // publish_to_rabbitmq(ast);
        auto resp = drogon::HttpResponse::newHttpJsonResponse({{"status","ok"}});
        callback(resp);

    } catch (const std::exception &e) {

        // ==== Create JSON for response ==== //
        Json::Value json;
        json["status"] = "error";
        json["message"] = e.what();

        // ==== Pass JSON as response ==== //
        // **** ::newHttpJsonResponse() method doesnt take std::initializer_list as an argument, only Json::Value accepted **** //
        auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
    }

}
