cmake_minimum_required(VERSION 4.0)
project(TelemetryBridge)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/mavlink
)

set(EXTRA_LIBS
        serial
        pthread
        ${Boost_LIBRARIES}
)

add_executable(TelemetryBridge
        src/main.cpp
        include/mavlink_converter.h
        include/msp_parser.h
        include/serial_interface.h
        include/telemetry_bridge.h
        include/upd_interface.h
        src/mavlink_converter.cpp
        src/serial_interface.cpp
        src/telemetry_bridge.cpp
        src/upd_interface.cpp
        src/test_file.cpp
        include/test_file.h
)

target_link_libraries(TelemetryBridge
        Threads::Threads
        ${EXTRA_LIBS}
)
