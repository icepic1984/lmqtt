#define BOOST_TEST_MODULE parser

#include <boost/test/unit_test.hpp>
#include "mqtt_parser.hpp"
#include "mqtt_serialize.hpp"


BOOST_AUTO_TEST_CASE(serialize_test)
{
	std::vector<uint8_t> buffer(20,0);
	// Test byte serialization
	uint8_t byte1 = 0;
	uint8_t byte2 = 255;
	lmqtt::serialize(buffer.begin(),byte1);
	lmqtt::serialize(buffer.begin()+1,byte2);	
	BOOST_CHECK_EQUAL(lmqtt::deserialize<uint8_t>(buffer.begin()),byte1);	
	BOOST_CHECK_EQUAL(lmqtt::deserialize<uint8_t>(buffer.begin()+1),byte2);	

	// Test uint16_t aka big endian
	uint16_t twobyte1 = 0;
	uint16_t twobyte2 = 65536-1;
	lmqtt::serialize(buffer.begin(),twobyte1);
	lmqtt::serialize(buffer.begin()+ sizeof(twobyte1),twobyte2);	
	BOOST_CHECK_EQUAL(lmqtt::deserialize<uint16_t>(buffer.begin()),twobyte1);	
	BOOST_CHECK_EQUAL(lmqtt::deserialize<uint16_t>(buffer.begin() +
	                                               sizeof(twobyte1)),twobyte2);	
	// Test string
	std::string str("blalalalallalal");
	lmqtt::serialize(buffer.begin(),str);	
	BOOST_CHECK_EQUAL(lmqtt::deserialize<std::string>(
		                  buffer.begin(), str.size()), str);
}

