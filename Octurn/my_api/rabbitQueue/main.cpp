#include "apiConnection.h"
#include "publishRabbit.h"
#include "worker.h"

int main() {
    try {
        // ==== Publish input from Drogon ==== //
        APIConnect API("guest","guest");
        API.init_options("localhost","/",5672);
        API.authorize();
        rabbitMQ rabbit;
        rabbit.channel_ = API.channel_;
        rabbit.publish_to_queue("backtest_tasks","Daniel!");
        Worker Worker(API.channel_);
        Worker.subscribe_to_queue("backtest_tasks");
        Worker.consume_message();


    } catch (const std::exception &e) {
        std::cerr << "Error RabbitMQ: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}