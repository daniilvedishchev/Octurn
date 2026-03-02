#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include "log/logHandler.hpp"
#include "worker.h"

Worker::Worker(AmqpClient::Channel::ptr_t channel_ptr) : channel_(std::move(channel_ptr)){}

void Worker::subscribe_to_queue(std::string queue_name){
    consumer_tag = channel_->BasicConsume(queue_name);
}

void Worker::consume_message(){
    while (true) {
            AmqpClient::Envelope::ptr_t envelope = channel_->BasicConsumeMessage(consumer_tag);
            std::string message = envelope->Message()->Body();

            g_logger.report(std::string("[x] Received: ") + message);

            // Тут можно сделать обработку, например вызвать функцию бэктеста
        }
}
