#include "mqtt_parser.hpp"
#include <boost/log/trivial.hpp>

namespace lmqtt{

mqtt_header_parser::mqtt_header_parser():
	multiplier_(1),
	state_(type)
{}

void mqtt_header_parser::reset()
{
	state_ = type;
	multiplier_ = 1;
}

mqtt_header_parser::result_type mqtt_header_parser::consume(mqtt_header& header, uint8_t byte)
{
	switch(state_) {
	case type:
		if((byte >> 4) == 15 &&  (byte >> 4 ) == 0){
			BOOST_LOG_TRIVIAL(debug) << "Bad header received: type field invalid";
			return bad;
		}
		header.type = static_cast<mqtt_control_packet_type>(byte >>4);
		header.flags = std::bitset<4>(byte & 0xF);
		header.remaining_length = 0;
		state_ = length;
		return indeterminate;
	case length:
		header.remaining_length += byte & 127 * multiplier_;
		if(multiplier_ > 128 * 128 * 128){
			BOOST_LOG_TRIVIAL(debug) << "Bad header received: remaining_lengt invalid";
			return bad;
		}
		if((byte & 128) == 0)
			return good;
		 else 
		   return indeterminate;
	}
}

std::string type_string(mqtt_control_packet_type type) {
	uint32_t field = static_cast<int>(type);
	return mqtt_control_packet_type_strings[field-1];
}

}


