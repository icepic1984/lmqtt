set (src
	mqtt_connection.cpp
	mqtt_server.cpp
	mqtt_connection_manager.cpp
	mqtt_parser.cpp)

find_package(Boost 1.50.0 REQUIRED COMPONENTS program_options log regex system)

include_directories(${Boost_INCLUDE_DIRS})

add_definitions(-DBOOST_LOG_DYN_LINK)

add_library(libmqtt ${src})
add_executable(mqtt main.cpp)
target_link_libraries(mqtt libmqtt ${Boost_LIBRARIES} pthread)
