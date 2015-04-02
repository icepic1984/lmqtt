#define BOOST_TEST_MODULE parser

#include <boost/test/unit_test.hpp>
#include "mqtt_parser.hpp"
#include "mqtt_serialize.hpp"


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

BOOST_AUTO_TEST_CASE(header_parser_message)
{
	 
	std::vector<uint8_t> buffer(128,0);
	lmqtt::mqtt_header header;
	lmqtt::mqtt_connect_type message;
	
	// Test for wrong length field
	uint16_t length = 3;
	lmqtt::serialize(buffer.begin(),length);
	BOOST_CHECK_EQUAL(
		static_cast<int>(lmqtt::parse_buffer(buffer.begin(),
		                                     buffer.end(), header,message)),
		static_cast<int>(lmqtt::result_type::bad));

	// Test for wrong protocol string 
	length = 4;
	std::string name("MQtt");
	lmqtt::serialize(buffer.begin(),length);
	lmqtt::serialize(buffer.begin()+sizeof(length),name);
	BOOST_CHECK_EQUAL(
		static_cast<int>(lmqtt::parse_buffer(buffer.begin(),
		                                     buffer.end(), header,message)),
		static_cast<int>(lmqtt::result_type::bad));

	// Test reserved flag
	length = 4;
	name = std::string("MQTT");
	uint8_t protocol = 4;
	uint8_t flags = 0b00000001;
	auto iter = buffer.begin();
	lmqtt::serialize(iter,length);
	iter += sizeof(length);
	lmqtt::serialize(iter,name);
	iter += name.size();
	lmqtt::serialize(iter,protocol);
	iter += sizeof(protocol);
	lmqtt::serialize(iter,flags);
	BOOST_CHECK_EQUAL(
		static_cast<int>(lmqtt::parse_buffer(buffer.begin(),
		                                     buffer.end(), header,message)),
		static_cast<int>(lmqtt::result_type::bad));


	// Test qos flags
	length = 4;
	name = std::string("MQTT");
	protocol = 4;
	flags = 0b000001000;
	iter = buffer.begin();
	lmqtt::serialize(iter,length);
	iter += sizeof(length);
	lmqtt::serialize(iter,name);
	iter += name.size();
	lmqtt::serialize(iter,protocol);
	iter += sizeof(protocol);
	lmqtt::serialize(iter,flags);
	BOOST_CHECK_EQUAL(
		static_cast<int>(lmqtt::parse_buffer(buffer.begin(),
		                                     buffer.end(), header,message)),
		static_cast<int>(lmqtt::result_type::bad));
	// Test password flags
	length = 4;
	name = std::string("MQTT");
	protocol = 4;
	flags = 0b010001000;
	iter = buffer.begin();
	lmqtt::serialize(iter,length);
	iter += sizeof(length);
	lmqtt::serialize(iter,name);
	iter += name.size();
	lmqtt::serialize(iter,protocol);
	iter += sizeof(protocol);
	lmqtt::serialize(iter,flags);
	BOOST_CHECK_EQUAL(
		static_cast<int>(lmqtt::parse_buffer(buffer.begin(),
		                                     buffer.end(), header,message)),
		static_cast<int>(lmqtt::result_type::bad));

	// Test password flags
	length = 4;
	name = std::string("MQTT");
	protocol = 4;
	flags = static_cast<uint8_t>(0b110001100);
	uint16_t keep_alive = 12345;
	iter = buffer.begin();
	lmqtt::serialize(iter,length);
	iter += sizeof(length);
	lmqtt::serialize(iter,name);
	iter += name.size();
	lmqtt::serialize(iter,protocol);
	iter += sizeof(protocol);
	lmqtt::serialize(iter,flags);
	iter += sizeof(flags);
	lmqtt::serialize(iter,keep_alive);
	BOOST_CHECK_EQUAL(
		static_cast<int>(lmqtt::parse_buffer(buffer.begin(),
		                                     buffer.end(), header,message)),
		static_cast<int>(lmqtt::result_type::good));
	BOOST_CHECK_EQUAL(message.keep_alive,keep_alive);
}
