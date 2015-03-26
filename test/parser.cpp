#define BOOST_TEST_MODULE parser

#include <boost/test/unit_test.hpp>
#include "mqtt_parser.hpp"


BOOST_AUTO_TEST_CASE(header_firstbyte)
{
	// Check invalid type field
	lmqtt::mqtt_header header;
	std::vector<uint8_t> buffer{0,193,2};
	lmqtt::mqtt_header_parser parser;
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::bad);	
	parser.reset();
	buffer = {static_cast<uint8_t>(15) << 4, 193,2};
	std::cout << static_cast<int>(buffer.front()) << std::endl;
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::bad);	

	


}

BOOST_AUTO_TEST_CASE(header_remaining_length)
{
	//Check remaining length field (incomplete)
	lmqtt::mqtt_header header;
	std::vector<uint8_t> buffer{1};
	lmqtt::mqtt_header_parser parser;
	buffer = {static_cast<uint8_t>(1) << 4 , 193};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::indeterminate);	
		   
	// Check remaining length field (complete)
	// Example from mqtt-v3.1.1 2.2.3
	parser.reset();
	buffer = {static_cast<uint8_t>(1) << 4 , 193,2};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::good);	
	BOOST_CHECK_EQUAL(header.remaining_length,321);

	// Min bytes
	parser.reset();
	buffer = {static_cast<uint8_t>(1) << 4 , 0};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::good);	
	BOOST_CHECK_EQUAL(header.remaining_length,0);

	// Max bytes
	parser.reset();
	buffer = {static_cast<uint8_t>(1) << 4 , 255,255,255,127};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::good);	
	BOOST_CHECK_EQUAL(header.remaining_length,268435455);

	// Max bytes
	parser.reset();
	buffer = {static_cast<uint8_t>(1) << 4 , 128,128,128,1};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::good);	
	BOOST_CHECK_EQUAL(header.remaining_length,2097152);

	// Overflow
	parser.reset();
	buffer = {static_cast<uint8_t>(1) << 4 , 128,128,128,128,1};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::bad);	
}

BOOST_AUTO_TEST_CASE(header_parser_bad)
{

	BOOST_CHECK_EQUAL(4,4);
}
