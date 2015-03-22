cmake_minimum_required(VERSION 3.0)
project(mqtt)

set (src main.cpp
	mqtt_connection.cpp
	mqtt_server.cpp
	mqtt_connection_mamager.cpp)
find_package(Boost 1.50.0 REQUIRED COMPONENTS program_options regex system)

include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-std=c++14")
add_executable(testmqtt ${src})
target_link_libraries(testmqtt ${Boost_LIBRARIES})
