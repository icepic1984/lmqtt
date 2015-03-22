#ifndef MQTT_PARSER_HPP
#define MQTT_PARSER_HPP

#include <iostream>
#include <bitset>
#include <iterator>
#include <stdexcept>
#include <tuple>
namespace lmqtt {

enum class mqtt_control_packet_type 
{
   CONNECT = 1,
   CONNACK,
   PUBLISH,
   PUBACK,
   PUBREC,
   PUBREL,
   PUBCOMP,
   SUBSCRIBE,
   SUBACK,
   UNSUBSCRIBE,
   UNSUBACK,
   PINGREQ,
   PINGRESP,
   DISCONNECT,
};

struct mqtt_fixed_header 
{
   mqtt_control_packet_type type;
   std::bitset<4> flags;
   uint32_t remaining_length;
};
	
mqtt_control_packet_type get_control_packet_type(uint8_t byte)
{return(static_cast<mqtt_control_packet_type>(byte >>4));}

template<typename InputIterator>
uint32_t get_remaining_bytes(InputIterator begin, InputIterator end)
{
	uint32_t multiplier = 1;
	uint32_t result = 0;
	begin++;
	do{
		result += ((*begin++) & 127) * multiplier;
		multiplier *= 128;
		if(multiplier > 128*128*128)
		   throw std::length_error("Invalid header size");
	} while((begin != end) && (*begin & 128) != 0);
	return result;
}


template <typename InputIterator>
std::tuple<mqtt_fixed_header,InputIterator>
parse_fixed_header(InputIterator begin, InputIterator end)
{
	mqtt_fixed_header header;
	header.type = get_control_packet_type(*begin);
	header.flags = std::bitset<4>(*begin & 0xF);
	header.remaining_length = get_remaining_bytes(begin,end);
	return std::make_tuple(header,begin);
}

template <typename InputIterator>
bool fixed_header_complete(InputIterator begin, InputIterator end)
{
	if(std::distance(begin,end) < 2) return false;
	while(begin != end) {
		if(((*begin++) & 128)  == 0) return true;
	}
	return false;
};
}

#endif
   
   
   
