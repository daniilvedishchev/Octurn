#pragma once
#include <string>
#include <fstream>

class logHandler {
    public:
        logHandler();
        void report(std::string message);
    private:
        void create_file();
        std::string current_time();
        std::ofstream file;

};

extern logHandler g_logger;

