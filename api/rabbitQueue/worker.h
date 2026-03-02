#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>

class Worker{
    public:

    // ==== Constructor ==== //
    Worker(AmqpClient::Channel::ptr_t channel_ptr);

    // ==== Attach worker to the queue ==== //
    void subscribe_to_queue(std::string queue_name);
    
    // ==== Retrieve message ==== //
    void consume_message();

    private:

    std::string consumer_tag;
    AmqpClient::Channel::ptr_t channel_;
};