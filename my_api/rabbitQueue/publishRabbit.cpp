#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>
#include "publishRabbit.h"
#include  "apiConnection.h"
#include "worker.h"

rabbitMQ::rabbitMQ(){}

void rabbitMQ::publish_to_queue(std::string queue_name,std::string backtest_strategy){
        
    if (!channel_) {
    throw std::runtime_error("Channel is not authorized. Call authorize() first.");
    }
    
    // ==== Create a queue if not exist ==== //
    channel_->DeclareQueue(queue_name, false, true, false, false);

    // ==== Create a message then send it to queue ==== //
    auto message = AmqpClient::BasicMessage::Create(backtest_strategy);
    channel_->BasicPublish("", queue_name, message);
}
