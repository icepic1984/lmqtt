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

	// Check flags: mqtt-v3.1.1 2.2.2 
	parser.reset();
	buffer = {(static_cast<uint8_t>(lmqtt::mqtt_control_packet_type::connect)
	           << 4) | 1, 193,2};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::bad);	
	parser.reset();
	buffer = {(static_cast<uint8_t>(lmqtt::mqtt_control_packet_type::publish)
	           << 4) | 15, 193,2};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::good);
	parser.reset();
	buffer = {(static_cast<uint8_t>(lmqtt::mqtt_control_packet_type::pubrel)
	           << 4) | 1, 193,2};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::bad);	
	parser.reset();
	buffer = {(static_cast<uint8_t>(lmqtt::mqtt_control_packet_type::pubrel)
	           << 4) | 2, 193,2};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::good);	
	parser.reset();
	buffer = {(static_cast<uint8_t>(lmqtt::mqtt_control_packet_type::unsubscribe)
	           << 4) | 2, 193,2};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::good);
	parser.reset();
	buffer = {(static_cast<uint8_t>(lmqtt::mqtt_control_packet_type::subscribe)
	           << 4) | 2, 193,2};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::good);	


}

BOOST_AUTO_TEST_CASE(header_remaining_length)
{
	//Check remaining length field (incomplete)
	lmqtt::mqtt_header header;
	std::vector<uint8_t> buffer;
	lmqtt::mqtt_header_parser parser;
	buffer = {static_cast<uint8_t>(lmqtt::mqtt_control_packet_type::publish) << 4 ,
	          193};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::indeterminate);	
		   
	// Check remaining length field (complete)
	// Example from mqtt-v3.1.1 2.2.3
	parser.reset();
	buffer = {static_cast<uint8_t>(lmqtt::mqtt_control_packet_type::publish) << 4 ,
	          193,2};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::good);	
	BOOST_CHECK_EQUAL(header.remaining_length,321);

	// Min bytes
	parser.reset();
	buffer = {static_cast<uint8_t>(lmqtt::mqtt_control_packet_type::publish) << 4 ,
	          0};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::good);	
	BOOST_CHECK_EQUAL(header.remaining_length,0);

	// Max bytes
	parser.reset();
	buffer = {static_cast<uint8_t>(lmqtt::mqtt_control_packet_type::publish) << 4 ,
	          255,255,255,127};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::good);	
	BOOST_CHECK_EQUAL(header.remaining_length,268435455);

	// Max bytes
	parser.reset();
	buffer = {static_cast<uint8_t>(lmqtt::mqtt_control_packet_type::publish) << 4 ,
	          128,128,128,1};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::good);	
	BOOST_CHECK_EQUAL(header.remaining_length,2097152);

	// Overflow
	parser.reset();
	buffer = {static_cast<uint8_t>(lmqtt::mqtt_control_packet_type::publish) << 4 ,
	          128,128,128,128,1};
	BOOST_CHECK_EQUAL(std::get<0>(parser.parse(header,buffer.begin(),buffer.end())),
	                  lmqtt::mqtt_header_parser::bad);	
}

BOOST_AUTO_TEST_CASE(header_parser_bad)
{

	BOOST_CHECK_EQUAL(4,4);
}
