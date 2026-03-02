cmake_minimum_required(VERSION 3.14)
project(Octurn)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)

# ---- CORE ---- #
add_library(octurn_core STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/node/Node.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/parser/Parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lexer/Lexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/interpreter/Interpreter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/Utils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/log/logHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ta/taLib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mappers/maps.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/config/cfgTemp.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/polygon/polygonClient.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/polygon/polygonDataFeed.cpp
)
  
target_include_directories(octurn_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  "/opt/homebrew/include"
)

# ----- Native CLI (только native) -----
add_executable(Octurn main.cpp)
target_link_libraries(Octurn PRIVATE octurn_core nlohmann_json::nlohmann_json cpr::cpr)
