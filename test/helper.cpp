#define BOOST_TEST_MODULE helper

#include <boost/test/unit_test.hpp>
#include "mqtt_parser.hpp"
#include "mqtt_serialize.hpp"


BOOST_AUTO_TEST_CASE(remaining_byes)
{
	std::vector<uint8_t> bytes = lmqtt::calculate_remaining_bytes(0);
	BOOST_CHECK_EQUAL(bytes.size(), 1);
	BOOST_CHECK_EQUAL(bytes[0], 0);	
	bytes = lmqtt::calculate_remaining_bytes(64);
	BOOST_CHECK_EQUAL(bytes.size(), 1);
	BOOST_CHECK_EQUAL(bytes[0], 64);	
	bytes = lmqtt::calculate_remaining_bytes(321);
	BOOST_CHECK_EQUAL(bytes.size(), 2);
	BOOST_CHECK_EQUAL(bytes[0], 193);	
	BOOST_CHECK_EQUAL(bytes[1], 2);	
	bytes = lmqtt::calculate_remaining_bytes(2097151);
	BOOST_CHECK_EQUAL(bytes.size(), 3);
	BOOST_CHECK_EQUAL(bytes[0], 0xFF);	
	BOOST_CHECK_EQUAL(bytes[1], 0XFF);	
	BOOST_CHECK_EQUAL(bytes[2], 0X7F);	
	
	bytes = lmqtt::calculate_remaining_bytes(268435455);
	BOOST_CHECK_EQUAL(bytes.size(), 4);
	BOOST_CHECK_EQUAL(bytes[0], 0xFF);	
	BOOST_CHECK_EQUAL(bytes[1], 0XFF);	
	BOOST_CHECK_EQUAL(bytes[2], 0XFF);	
	BOOST_CHECK_EQUAL(bytes[3], 0X7F);	
	
}


	
