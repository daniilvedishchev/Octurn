#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>

class APIConnect{

    public:
    // ==== Constructor ==== //
    APIConnect(std::string login,std::string password);
    // ==== Set up connection options ==== //
    void init_options(std::string host, std::string vhost, int port);
    
    // ==== Auth ==== //
    void authorize();

    // ==== Keeps channel pointer in rabbitMQ, connection options ==== //
    AmqpClient::Channel::ptr_t channel_;
    AmqpClient::Channel::OpenOpts connection_options;

    private:

    std::string password_;
    std::string login_;
};