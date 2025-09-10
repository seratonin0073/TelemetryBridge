cmake_minimum_required(VERSION 3.10)
project(TelemetryBridge)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp")

include_directories(include mavlink)

add_executable(TelemetryBridge ${SOURCES})

target_link_libraries(TelemetryBridge pthread serial)