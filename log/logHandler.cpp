#include "logHandler.hpp"
#include <iostream>


logHandler g_logger;

logHandler::logHandler() {
    create_file();
}

void logHandler::create_file(){
    // Open in write mode
    file.open("logs.txt", std::ios::app);
    if (!file) {
        std::cerr << "Couldn't open file\n";
    }
}

void logHandler::report(std::string message){
    if(file.is_open()){
        file << "[ " + current_time() + " ] " + message +"\n";
        file.flush();
    }
}

std::string logHandler::current_time() {

    // Gets current local time //
    auto now = std::chrono::system_clock::now();
    // Convert to C++ friendly t_time time object *nb of seconds since Jan 1 1970 //
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    // C object that keeps time objects in our expected format (seconds,minutes etc.)
    std::tm tm_now;
    #ifdef _WIN32
        // Get local time on Windows machine converts seconds to human readable date
        localtime_s(&tm_now, &now_c);
    #else
        // Get local time on Linux/Mac machine converts seconds to human readable date
        localtime_r(&now_c, &tm_now);
    #endif
        // Reserve space //
        char buffer[20];
        // Format to Y-m-d H:M:S datetype
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_now);
        return std::string(buffer);
}
