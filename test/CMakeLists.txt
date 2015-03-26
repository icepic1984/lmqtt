
find_package(Boost 1.50.0 REQUIRED COMPONENTS unit_test_framework program_options log regex system)

include_directories(${Boost_INCLUDE_DIRS}
					${mqtt_SOURCE_DIR}/src)
					
add_definitions (-DBOOST_TEST_DYN_LINK)
add_executable(parser parser.cpp)
target_link_libraries(parser libmqtt ${Boost_LIBRARIES} pthread) 
