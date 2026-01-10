#include "apiConnection.h"

APIConnect::APIConnect(std::string login,std::string password) : login_(std::move(login)),password_(std::move(password)){}


void APIConnect::authorize() {
    connection_options.auth = AmqpClient::Channel::OpenOpts::BasicAuth(login_, password_);
    channel_ = AmqpClient::Channel::Open(connection_options);
}

void APIConnect::init_options(std::string host, std::string vhost, int port){
    // ==== Connection options object ==== //
    AmqpClient::Channel::OpenOpts opts;

    connection_options = opts; // Save it for further modifications

    // ==== Init options ==== //
    connection_options.host = std::move(host);
    connection_options.vhost = std::move(vhost);
    connection_options.port = port;
};