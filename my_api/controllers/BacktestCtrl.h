#pragma once
#include <drogon/HttpController.h>


class BacktestCtrl : public drogon::HttpController<BacktestCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(BacktestCtrl::asyncHandleHttpRequest, "/backtest", drogon::Post);
    METHOD_LIST_END

    void asyncHandleHttpRequest(const drogon::HttpRequestPtr& req, std::function<void (const drogon::HttpResponsePtr &)> &&callback);
};
