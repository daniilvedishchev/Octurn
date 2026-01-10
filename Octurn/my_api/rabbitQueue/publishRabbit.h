#pragma once
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>

class rabbitMQ{
    public:

    rabbitMQ();

    // ==== Publish to queue channel ==== //
    void publish_to_queue(std::string queue_name,std::string backtest_strategy);

    AmqpClient::Channel::ptr_t channel_;
    AmqpClient::Channel::OpenOpts connection_options;

    private:

    std::string login_;
    std::string password_;
};