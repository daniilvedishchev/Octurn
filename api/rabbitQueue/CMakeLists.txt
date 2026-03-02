# // Use minimum cmake version of 3.22 // #
cmake_minimum_required(VERSION 3.22)
project(RabbitWorker CXX)

#// ==== Use C++20 ==== //#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# // ==== Path to headers ==== // #
# Compiler looks for these "includes" so he can find function declarations used in main.cpp #
include_directories(
    /usr/local/include
    /opt/homebrew/include
)

# // ==== Path to libraries ==== // #
# Linker uses libraries to execute functions in main.cpp #
link_directories(
    /Users/kodalir/SimpleAmqpClient/build
    /opt/homebrew/lib
)

# // ==== Create an executable file ==== // #
# Add files what are necessary for executing main.cpp #
add_executable(main 
    main.cpp
    apiConnection.cpp
    worker.cpp
    publishRabbit.cpp
)

# // ==== Link all libriaries up in main.cpp ==== // #
target_link_libraries(main
    SimpleAmqpClient
    rabbitmq.4
    pthread
)
