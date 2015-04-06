#define BOOST_TEST_MODULE parser

#include <boost/test/unit_test.hpp>
#include "mqtt_parser.hpp"
#include "mqtt_serialize.hpp"


BOOST_AUTO_TEST_CASE(serialize_test)
{
	 std::vector<uint8_t> buffer(20,0);
	/// Test byte serialization
	uint8_t byte1 = 0;
	uint8_t byte2 = 255;
	auto begin = lmqtt::serialize(buffer.begin(),buffer.end(),byte1);
	begin = lmqtt::serialize(begin,buffer.end(),byte2);	

	uint8_t check1;
	uint8_t check2;
	begin = lmqtt::deserialize(buffer.begin(),buffer.end(),check1);
	begin = lmqtt::deserialize(begin,buffer.end(),check2);
	BOOST_CHECK_EQUAL(check1,byte1);	
	BOOST_CHECK_EQUAL(check2,byte2);	

	// // Test uint16_t aka big endian
	uint16_t twobyte1 = 0;
	uint16_t twobyte2 = 65536-1;
	uint16_t check_two1 = 0;
	uint16_t check_two2 = 0; 
	begin= lmqtt::serialize(buffer.begin(),buffer.end(),twobyte1);
	begin = lmqtt::serialize(begin,buffer.end(),twobyte2);	
	begin = lmqtt::deserialize(buffer.begin(),buffer.end(),check_two1);
	begin = lmqtt::deserialize(begin,buffer.end(),check_two2);
	BOOST_CHECK_EQUAL(check_two1,twobyte1);	
	BOOST_CHECK_EQUAL(check_two2,twobyte2);	

	// Test string
	std::string str("blalalalallalal");
	std::string test_str(str.size(),' ');
	begin = lmqtt::serialize(buffer.begin(),buffer.end(),str);	
	lmqtt::deserialize(buffer.begin(), buffer.end(),test_str); 
	BOOST_CHECK_EQUAL(test_str,str);

	// Overflow
	str = std::string("aaaaaaaaaaaaaaaaaaaaaaaaaaa");
	test_str = std::string(str.size(),' ');
	//lmqtt::serialize(buffer.begin(), str);	
	BOOST_CHECK_THROW(lmqtt::deserialize(buffer.begin(), buffer.end(),test_str),
	                  std::out_of_range);
	BOOST_CHECK_THROW(lmqtt::serialize(buffer.begin(),buffer.end(),test_str),
	                  std::out_of_range);

}

