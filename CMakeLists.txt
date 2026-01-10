cmake_minimum_required(VERSION 3.10)
project(TestParserInterpretor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(/opt/homebrew/include/eigen3
                    /Users/kodalir/emsdk/upstream
)

add_executable(Octurn 
                main.cpp
                node/Node.cpp 
                parser/Parser.cpp
                lexer/Lexer.cpp
                interpreter/Interpretator.cpp
                utils/Utils.cpp
                log/logHandler.cpp
                TA/taLib.cpp
                mappers/maps.cpp)

target_link_libraries(Octurn PRIVATE cpr::cpr nlohmann_json::nlohmann_json)

target_include_directories(Octurn
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
