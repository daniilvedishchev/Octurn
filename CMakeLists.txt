cmake_minimum_required(VERSION 3.14)
project(TestParserInterpretor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT EMSCRIPTEN)
  find_package(cpr REQUIRED)
  find_package(nlohmann_json REQUIRED)
endif()

# ---- CORE ---- #
add_library(octurn_core STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Octurn/node/Node.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Octurn/parser/Parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Octurn/lexer/Lexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Octurn/interpreter/Interpretator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Octurn/utils/Utils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Octurn/log/logHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Octurn/TA/taLib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Octurn/mappers/maps.cpp
)

target_include_directories(octurn_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Octurn
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# ----- Native CLI (только native) -----
if(NOT EMSCRIPTEN)
  add_executable(Octurn main.cpp)
  target_link_libraries(Octurn PRIVATE octurn_core nlohmann_json::nlohmann_json cpr::cpr)
endif()

# ----- WASM wrapper -----

# ----- WASM wrapper -----
if(EMSCRIPTEN)
  add_executable(octurn_wasm
    wasm/octurn_api.cpp
    src/data/polygon/polygon_wasm_bridge.cpp
  )
  target_link_libraries(octurn_wasm PRIVATE octurn_core)

  target_compile_options(octurn_core PRIVATE -fexceptions)
  target_compile_options(octurn_wasm PRIVATE -fexceptions)

  target_link_options(octurn_wasm PRIVATE
    "-fexceptions"
    "-sDISABLE_EXCEPTION_CATCHING=0"
    "-sEXCEPTION_DEBUG=1"
    "-g"
    "-O3"
    "-lembind"
    "-sMODULARIZE=1"
    "-sEXPORT_NAME=createModule"
    "-sENVIRONMENT=web"
    "-sALLOW_MEMORY_GROWTH=1"
  )

  set_target_properties(octurn_wasm PROPERTIES
    OUTPUT_NAME "octurn_api"
    SUFFIX ".js"
  )
endif()


