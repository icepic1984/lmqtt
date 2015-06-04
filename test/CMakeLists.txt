
find_package(Boost 1.50.0 REQUIRED COMPONENTS unit_test_framework program_options log regex system)


include_directories(${Boost_INCLUDE_DIRS}
					${mqtt_SOURCE_DIR}/src)
					
add_definitions (-DBOOST_TEST_DYN_LINK)
add_executable(parser parser.cpp)
add_executable(serialize serialize.cpp)
add_executable(helper helper.cpp)
add_executable(broker broker.cpp)
target_link_libraries(parser libmqtt ${Boost_LIBRARIES} pthread) 
target_link_libraries(serialize libmqtt ${Boost_LIBRARIES} pthread) 
target_link_libraries(helper libmqtt ${Boost_LIBRARIES} pthread) 
target_link_libraries(broker libmqtt ${Boost_LIBRARIES} pthread)
